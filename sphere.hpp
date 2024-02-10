#pragma once
#include "entity.hpp"

#pragma region declaration of sphere
class sphere : public entity {
public:
    sphere() {}
    sphere(point3 _center, float r, std::shared_ptr<material> _material)
        : center(_center)
        , radius(r)
        , mat(_material)
         {};

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    point3 center{};
    float radius{};
    std::shared_ptr<material> mat;
};
#pragma endregion

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec3 oc{ r.origin() - center };
    auto a{ r.direction().sqared_length() };
    float half_b{ dot(oc, r.direction()) };
    float c{ oc.sqared_length() - radius * radius };
    float discriminant{ half_b * half_b - a * c };
    
    if (discriminant < 0.f)
        return false;

    float sqrtd{ sqrt(discriminant) };
    float root{ (-half_b - sqrtd) / a };
    if (!ray_t.surrounds(root)) {
        root = (-half_b + sqrtd) / a;
        if(!ray_t.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 otward_normal{ ( rec.p - center) / radius };
    set_face_normal(rec, r, otward_normal);
    rec.mat = mat;
    
    return true;
}