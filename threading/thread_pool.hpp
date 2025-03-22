#pragma once

#include <thread>
#include <vector>

#pragma region RAII thread vec
class joining_threads
{
    std::vector<std::thread>& threads;

public:
    explicit joining_threads(std::vector<std::thread>& _threads) :
        threads(_threads)
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
        impl_type(F&& _f) : f(std::move(_f)) {}
        void call() { f(); }
    };

    std::unique_ptr<impl_base> impl;

public:
    template <typename F>
    function_wrapper(F&& _f) : impl{ new impl_type<F>(std::move(_f)) }
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