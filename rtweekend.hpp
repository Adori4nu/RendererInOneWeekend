#pragma once
#include <numbers>
#include <random>


inline std::mt19937& get_generator() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

__forceinline float degrees_to_radians(float degrees) {
    return degrees * std::numbers::pi / 180.0f;
}

__forceinline int random_int() {
    std::uniform_int_distribution<int> distribution(0, 1);
    return distribution(get_generator());
}

__forceinline int random_int(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(get_generator());
}

__forceinline float random_float() {
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(get_generator());
}

__forceinline float random_float(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(get_generator());
}

__forceinline double random_double() {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(get_generator());
}

__forceinline double random_double(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(get_generator());
}

#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"