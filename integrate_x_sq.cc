#include "rtweekend.hpp"

#include <cmath>
#include <iostream>
#include <iomanip>

auto icd(float f) -> float
{
    return 8.0f * std::pow(f, 1.0f / 3.0f);
}

auto pdf(float x) -> float
{
    return (3.0f / 8.0f) * x * x;
}

auto main() -> int
{
    constexpr unsigned int N{ 1 };
    auto sum{ 0.0f };

    for (unsigned int i{ 0 }; i < N; ++i)
    {
        auto z{ random_float() };
        if (z == 0.0f)
            continue; // Avoid division by zero

        auto x{ icd(z) };
        sum += x * x / pdf(x);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}