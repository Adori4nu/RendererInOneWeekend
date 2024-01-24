#pragma once
#include "vec3.hpp"

#pragma region declaration of ray
class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction) { orig = origin; dir = direction; }
    vec3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    point3 at(float t) const { return orig + t * dir; } 

    point3 orig;
    vec3 dir;
};
#pragma endregion