#pragma once
#include "aabb.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"


class material;

#pragma region declaration of hit record
struct hit_record {
    point3 p{};
    vec3 normal{};
    std::shared_ptr<material> mat;
    float t{};
    double u{};
    double v{};
    bool front_face;
};
#pragma endregion

void set_face_normal(hit_record& hit_rec, const ray& r, const vec3& outward_normal) {
    hit_rec.front_face = dot(r.direction(), outward_normal) < 0;
    hit_rec.normal = hit_rec.front_face ? outward_normal : -outward_normal;
}

#pragma region declaration of entity
class entity {
public:

    virtual ~entity() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};
#pragma endregion