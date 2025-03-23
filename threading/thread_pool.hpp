#pragma once

#include <thread>
#include <vector>

#pragma region RAII thread vec
class joining_threads
{
    std::vector<std::thread>& threads;

public:
    explicit joining_threads(std::vector<std::thread>& threads_) :
        threads(threads_)
    {}

    ~joining_threads()
    {
        for (auto& t : threads)
        {
            if (t.joinable())
                t.join();
        }
    }
};
#pragma endregion
#pragma region Type erased function wraper
class function_wrapper
{
    struct impl_base
    {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };

    template <typename F>
    struct impl_type : impl_base
    {
        F f;
        impl_type(F&& f_) : f(std::move(f_)) {}
        void call() { f(); }
    };

    std::unique_ptr<impl_base> impl;

public:
    template <typename F>
    function_wrapper(F&& f_) : impl{ new impl_type<F>(std::move(f_)) }
    {}

    void operator()() { impl->call(); }

    function_wrapper()
    {}

    function_wrapper(function_wrapper&& other) :
        impl{ std::move(other.impl) }
    {}

    function_wrapper& operator=(function_wrapper&& other)
    {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) = delete;
};
#pragma endregion

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#pragma region thread safe queue 
template <typename T>
class thread_safe_queue
{
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable data_cond;

public:

    thread_safe_queue() {}

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(*data_queue.front());
        data_queue.pop();
    }
    
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
        return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }
    
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res{ data_queue.front() };
        data_queue.pop();
        return res;
    }
    
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res{ data_queue.front() };
        data_queue.pop();
        return res;
    }
    
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
    
    void push(T new_value)
    {
        std::shared_ptr<T> data (
            std::make_shared<T>(std::move(new_value))
        );
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
};
#pragma endregion

#include <deque>

#pragma region work stealing queue

class work_stealing_queue
{
    typedef function_wrapper data_type;
    std::deque<data_type> the_queue;
    mutable std::mutex the_mutex;

public:

    work_stealing_queue() {}

    work_stealing_queue(const work_stealing_queue&) = delete;
    work_stealing_queue& operator=(const work_stealing_queue&) = delete;

    void push(data_type data)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }

    bool try_pop(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty())
        {
            return false;
        }

        res = std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }

    bool try_steal(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty())
        {
            return false;
        }

        res = std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};

#pragma endregion

#include <cstdint>
#include <future>
#include <type_traits>

#pragma region pool of threads with work stealing

class thread_pool_ws
{
    typedef function_wrapper task_type;

    std::atomic_bool done;
    thread_safe_queue<task_type> global_work_queue;
    std::vector<std::unique_ptr<work_stealing_queue>> per_thread_queues;
    std::vector<std::thread> threads;
    joining_threads joiner;

    static thread_local work_stealing_queue* local_work_queue;
    static thread_local unsigned my_index;

    void worker_thread(unsigned my_index_)
    {
        my_index = my_index_;
        local_work_queue = per_thread_queues[my_index].get();
        while (!done)
        {
            run_pending_task();
        }
    }

    bool pop_task_from_local_queue(task_type& task)
    {
        return local_work_queue && local_work_queue->try_pop(task);
    }

    bool pop_task_from_global_pool_queue(task_type& task)
    {
        return global_work_queue.try_pop(task);
    }

    bool pop_task_from_other_thread_queue(task_type& task)
    {
        for (unsigned i{ 0 }; i < per_thread_queues.size(); ++i)
        {
            unsigned const index{ (my_index + i + 1) % per_thread_queues.size() };
            if (per_thread_queues[index]->try_steal(task))
            {
                return true;
            }
        }

        return false;
    }

public:

    thread_pool_ws() : done{false}, joiner{threads}
    {
        uint64_t const thread_count{ std::thread::hardware_concurrency() - 2 };
        try
        {
            for (size_t i {0}; i < thread_count; ++i)
            {
                per_thread_queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));
                threads.push_back(std::thread(&thread_pool_ws::worker_thread, this, i));
            }
        }
        catch(...)
        {
            done = true;
            throw;
        }   
    }

    ~thread_pool_ws()
    {
        done = true;
        global_work_queue.push(task_type([]{}));  // Dummy task to wake up threads that are in waiting for work state
    }

    template <typename Function_type>
    std::future<typename std::invoke_result<Function_type>::type> submit(Function_type f)
    {
        typedef typename std::invoke_result<Function_type>::type result_type;

        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());

        if (local_work_queue)
        {
            local_work_queue->push(std::move(task));
        }
        else
        {
            global_work_queue.push(std::move(task));
        }
        return res;
    }

    void run_pending_task()
    {
        task_type task;
        if (pop_task_from_local_queue(task) ||
            pop_task_from_global_pool_queue(task) ||
            pop_task_from_other_thread_queue(task))
        {
            task();   
        }
        else
        {
            std::this_thread::yield();
        }
    }
};

thread_local work_stealing_queue* thread_pool_ws::local_work_queue;
thread_local unsigned thread_pool_ws::my_index;

#pragma endregion