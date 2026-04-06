#include "rtweekend.hpp"
#include "vec3.hpp"

#include <iostream>
#include <iomanip>

auto f(const vec3& d) -> float
{
    auto cosine_squared{ d.z() * d.z() };
    return cosine_squared;
}

auto pdf(const vec3& d) -> float
{
    return 1 / (4 * pi);
}

auto main() -> int
{
    constexpr unsigned int N{ 1'000'000 };
    auto sum{ 0.0f };

    for (unsigned int i{ 0 }; i < N; ++i)
    {
        vec3 d{ random_unit_vector() };
        auto f_d{ f(d) };
        sum += f_d / pdf(d);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}