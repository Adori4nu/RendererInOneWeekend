#include "rtweekend.hpp"

#include <iostream>
#include <math.h>
#include <fstream>

auto main() -> int
{
    std::ofstream file("points_in_sphere.txt");
    if (!file) {
        std::cerr << "\033[1;31mCannot open file for writing.\033[0m\n";
        return 1;
    }

    for (int i{0}; i < 200; ++i)
    {
        auto r1{random_float()};
        auto r2{random_float()};
        auto x{ std::cos(2 * pi * r1) * 2 * std::sqrt(r2 * (1 - r2)) };
        auto y{ std::sin(2 * pi * r1) * 2 * std::sqrt(r2 * (1 - r2)) };
        auto z{ 1 - 2 * r2 };
        std::cout << "x: " << x << ", y: " << y << ", z: " << z << "\n";
        file << x << " " << y << " " << z << "\n";
    }

    file.close();

    std::cout << "Points written to points_in_sphere.txt\n";
}

