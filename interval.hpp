#pragma once
#include <limits>

static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");

constexpr const float pos_infinity{ std::numeric_limits<float>::infinity() };
constexpr const float neg_infinity{ -std::numeric_limits<float>::infinity() };

#pragma region interval declaration
class interval {
public:
    float min, max;

    interval() : min(pos_infinity), max(neg_infinity) {}
    interval(float _min, float _max) : min(_min), max(_max) {}

    float size() const { return max - min; }

    interval expand(float delta) const;

    bool contains(float x) const { return min <= x && x <= max; }

    bool surrounds(float x) const { return min < x && x < max; }

    float clamp(float x) const;

    static const interval empty, universe;
};

const static interval empty   (pos_infinity, neg_infinity);
const static interval universe(neg_infinity, pos_infinity);
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