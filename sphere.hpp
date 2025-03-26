#pragma once
#include "entity.hpp"

#pragma region declaration of sphere
class sphere : public entity {
public:
    sphere() {}
        sphere(const point3& _static_center, float r, std::shared_ptr<material> _material)
        : center(_static_center, vec3(0.f, 0.f, 0.f))
        , radius(std::fmax(0, r))
        , mat(_material)
         {};

    sphere(const point3& _center1, const point3& _center2, float r, std::shared_ptr<material> _material)
        : center(_center1, _center2 - _center1)
        , radius(std::fmax(0, r))
        , mat(_material)
         {};

    // sphere(point3 _center, float r, std::shared_ptr<material> _material)
    //     : center(_center)
    //     , radius(r)
    //     , mat(_material)
    //      {};

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    // point3 center;
    ray center;
    float radius{};
    std::shared_ptr<material> mat;
};
#pragma endregion

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    point3 current_center{ center.at(r.time()) };// here could be the issue geting float vector from double time
    vec3 oc{ r.origin() - current_center  /*- center*/ }; // it needs to stay in this order otherwise its not rendering
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
    vec3 otward_normal{ ( rec.p - current_center /*center*/) / radius };
    set_face_normal(rec, r, otward_normal);
    rec.mat = mat;
    
    return true;
}