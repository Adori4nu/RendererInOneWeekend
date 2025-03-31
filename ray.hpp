#pragma once
#include "vec3.hpp"

#pragma region declaration of ray
class ray {

    point3 orig;
    vec3 dir;
    double tm;

public:

    ray() {}

    ray(const point3& origin, const vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time) {}

    vec3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    point3 at(float t) const { return orig + t * dir; } 

    double time() const { return tm; }

};
#pragma endregion