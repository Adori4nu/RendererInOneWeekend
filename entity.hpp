#pragma once
#include "ray.hpp"

#pragma region declaration of hit record
struct hit_record {
    float t{};
    vec3 p{};
    vec3 normal{};
};
#pragma endregion

#pragma region declaration of entity
class entity {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};
#pragma endregion