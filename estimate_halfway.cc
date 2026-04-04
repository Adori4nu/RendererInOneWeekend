#include "rtweekend.hpp"

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>

struct sample
{
    float x;
    float p_x;
};

bool compare_by_x(const sample& a, const sample& b)
{
    return a.x < b.x;
}

int main()
{
    std::vector<sample> samples;
    constexpr unsigned int N = 1'000;
    samples.reserve(N);
    float sum{ 0.0f };

    // Iterate through all the samples

    for (unsigned int i{ 0 }; i < N; ++i)
    {
        auto x{ random_float(0, 2 * pi) };
        auto sin_x{ std::sin(x) };
        auto p_x{ exp(-x / (2 * pi)) * sin_x * sin_x};
        sum += p_x;
        
        samples.emplace_back( sample{x, p_x} );
    }

    // Sort the samples by x value
    std::sort(samples.begin(), samples.end(), compare_by_x);

    auto half_sum{ sum / 2.0f };
    auto halfway_x{ 0.0f };
    auto cumulative_sum{ 0.0f };
    for (unsigned int i{ 0 }; i < N; ++i)
    {
        cumulative_sum += samples[i].p_x;
        if (cumulative_sum >= half_sum)
        {
            halfway_x = samples[i].x;
            break;
        }
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Average = " << sum / N << '\n';
    std::cout << "Area under curve = " << 2 * pi * sum / N << '\n';
    std::cout << "Halfway = " << halfway_x << '\n';
}