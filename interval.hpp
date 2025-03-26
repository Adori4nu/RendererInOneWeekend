#pragma once
#include <limits>

static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");

constexpr const float infinity{ std::numeric_limits<float>::infinity() };
static const float near_zero{ std::nextafterf(0.f, 1.f) };

#pragma region interval declaration
class interval {
public:
    float min, max;

    interval() : min(+infinity), max(-infinity) {}
    
    interval(float _min, float _max) : min(_min), max(_max) {}

    interval(const interval& a, const interval& b) {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    float size() const { return max - min; }

    interval expand(float delta) const;

    bool contains(float x) const { return min <= x && x <= max; }

    bool surrounds(float x) const { return min < x && x < max; }

    float clamp(float x) const;

    static const interval empty, universe;
};

const interval interval::empty   (+infinity, -infinity);
const interval interval::universe(-infinity, +infinity);
#pragma endregion

interval interval::expand(float delta) const { 
    auto padding = delta / 2;
    return interval{ min - padding, max + padding }; 
}

float interval::clamp(float x) const {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}