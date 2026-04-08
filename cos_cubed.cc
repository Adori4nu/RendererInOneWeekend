#include "rtweekend.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

auto f(float r2) -> float
{
    auto z{ 1 - r2 };
    float cos_theta{ z };
    return cos_theta * cos_theta * cos_theta;
}

auto pdf() -> float
{
    return 1.0f / (2.f * pi);
}

auto main() -> int
{
    constexpr int N{ 1'000 };
    float sum{};

    std::ofstream file("cos_cubed.txt");
    if (!file) {
        std::cerr << "\033[1;31mCannot open file for writing.\033[0m\n";
        return 1;
    }

    file << "SampleIndex Estimated Integral\n";

    for (int i{0}; i < N; ++i)
    {
        auto r2{ random_float() };
        sum += f(r2) / pdf();
        file << i + 1 << " " << sum / (i + 1) << "\n";
    }
    
    file.close();

    std::cout << "Points written to cos_cubed.txt\n";

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "PI/2: " << pi / 2.f << "\n";
    std::cout << "Estimated integral: " << sum / N << "\n";
}