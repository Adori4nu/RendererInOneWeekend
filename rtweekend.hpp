#pragma once
#include <numbers>
#include <random>


__forceinline float degrees_to_radians(float degrees) {
    using namespace std::numbers;
    return degrees * pi / 180.0;
}

__forceinline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.f, 1.f);
    static std::mt19937 generator;
    return distribution(generator);
}

__forceinline float random_float(float min, float max) {
    static std::uniform_real_distribution<float> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"