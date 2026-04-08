#include "rtweekend.hpp"
#include "vec3.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

auto f(const vec3& d) -> float
{
    auto cos_theta{ d.z() };
    return cos_theta * cos_theta * cos_theta;
}

auto pdf(const vec3& d) -> float
{
    return d.z() / pi;
}

auto main() -> int
{
    constexpr int N{ 1'000 };
    float sum{};

    std::ofstream file("cos_density.txt");
    if (!file) {
        std::cerr << "\033[1;31mCannot open file for writing.\033[0m\n";
        return 1;
    }

    file << "Sample_Index Cosine_of_Direction_Density\n";

    for (int i{0}; i < N; ++i)
    {
        auto d{ random_cosine_direction() };
        sum += f(d) / pdf(d);
        file << i + 1 << " " << sum / (i + 1) << "\n";
    }
    
    file.close();

    std::cout << "Points written to cos_density.txt\n";

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "PI/2: " << pi / 2.f << "\n";
    std::cout << "Estimate: " << sum / N << "\n";
}