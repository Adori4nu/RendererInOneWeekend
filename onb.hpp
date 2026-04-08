#pragma once

#include "vec3.hpp"

#include <array>

#pragma region ONB declaration
class onb {
    std::array<vec3, 3> axis;
    // vec3 axis[3];

public:
    onb(const vec3& n)
    {
        axis[2] = unit_vector(n);
        vec3 a{ (std::fabs(axis[2].x()) > 0.9f) ? vec3{ 0.f, 1.f, 0.f } : vec3{ 1.f, 0.f, 0.f } };
        axis[1] = unit_vector(cross(axis[2], a));
        axis[0] = cross(axis[2], axis[1]);
    }

    const vec3& u() const { return axis[0]; }
    const vec3& v() const { return axis[1]; }
    const vec3& w() const { return axis[2]; }

    vec3 transform(const vec3& vec) const {
        return vec.x() * u() + vec.y() * v() + vec.z() * w();
    }
};
#pragma endregion 