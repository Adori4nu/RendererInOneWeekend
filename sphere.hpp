#pragma once
#include "aabb.hpp"
#include "entity.hpp"

#pragma region declaration of sphere
class sphere : public entity {
    
    ray center;
    float radius{};
    std::shared_ptr<material> mat;
    aabb bbox;

    static void get_sphere_uv(const point3& p, double& u, double& v) {
        auto theta{ std::acos(-p.y()) };
        auto phi{ std::atan2(-p.z(), p.x()) + std::numbers::pi };

        u = phi / (2 * std::numbers::pi);
        v = theta / std::numbers::pi;
    }

public:

    sphere() {}
        sphere(const point3& _static_center, float r, std::shared_ptr<material> _material)
        : center(_static_center, vec3(0.f, 0.f, 0.f))
        , radius(std::fmax(0, r))
        , mat(_material)
        {
            auto rvec{ vec3(r, r, r) };
            bbox = aabb{ _static_center - rvec, _static_center + rvec };
        };

    sphere(const point3& _center1, const point3& _center2, float r, std::shared_ptr<material> _material)
        : center(_center1, _center2 - _center1)
        , radius(std::fmax(0, r))
        , mat(_material)
        {
            auto rvec{ vec3(r, r, r) };
            aabb box1{ center.at(0) - rvec, center.at(0) + rvec };
            aabb box2{ center.at(1) - rvec, center.at(1) + rvec };
            bbox = aabb(box1, box2);
        };

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override { return bbox; }
    
};
#pragma endregion

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    point3 current_center{ center.at(r.time()) };
    vec3 oc{ r.origin() - current_center }; // it needs to stay in this order otherwise its not rendering
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
    vec3 outward_normal{ ( rec.p - current_center) / radius };
    set_face_normal(rec, r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;
    
    return true;
}