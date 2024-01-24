#pragma once
#include "rtweekend.hpp"


class uber_material;

#pragma region declaration of hit record
struct hit_record {
    point3 p{};
    vec3 normal{};
    std::shared_ptr<uber_material> mat;
    float t{};
    bool front_face{};
};
#pragma endregion

void set_face_normal(hit_record& hit_rec, const ray& r, const vec3& outward_normal) {
    hit_rec.front_face = dot(r.direction(), outward_normal) < 0.f;
    hit_rec.normal = hit_rec.front_face ? outward_normal : -outward_normal;
}

#pragma region declaration of entity
class entity {
public:
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};
#pragma endregion