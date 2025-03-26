#pragma once
#include <numbers>
#include <random>


__forceinline float degrees_to_radians(float degrees) {
    return degrees * std::numbers::pi / 180.0;
}

__forceinline float random_float() {
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<float> distribution(0.f, 1.f);
    return distribution(generator);
}

__forceinline float random_float(float min, float max) {
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

__forceinline double random_double() {
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<double> distribution(0., 1.);
    return distribution(generator);
}

__forceinline double random_double(double min, double max) {
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"