#pragma once
#include "aabb.hpp"
#include "entity.hpp"
#include "onb.hpp"
#include "ray.hpp"

#pragma region declaration of sphere
class sphere : public entity {
    
    ray center;
    float radius{};
    std::shared_ptr<material> mat;
    aabb bbox;

    static void get_sphere_uv(const point3& p, float& u, float& v) {
        auto theta{ std::acos(-p.y()) };
        auto phi{ std::atan2(-p.z(), p.x()) + pi };

        u = phi / (2 * pi);
        v = theta / pi;
    }

    static vec3 ramdom_to_sphere(float radius, float distance_squared) {
        auto r1{ random_float() };
        auto r2{ random_float() };
        auto z{ 1 + r2 * (std::sqrt(1 - radius * radius / distance_squared) - 1) };

        auto phi{ 2 * pi * r1 };
        auto x{ std::cos(phi) * std::sqrt(1 - z * z) };
        auto y{ std::sin(phi) * std::sqrt(1 - z * z) };
        return vec3(x, y, z);
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

    point3 center_at_time(float time) const {
        // Normalize the time to [0,1] range based on the expected time range
        // If your shutter speed is 0.16, this would normalize times from [0,0.16] to [0,1]
        float normalized_time = time / 0.16;
        return center.at(normalized_time);
    }

    float pdf_value(const point3& origin, const vec3& direction) const override {
        // This method only works for stationary spheres. For moving spheres, we would need to account for the sphere's position at the time of intersection.
        hit_record rec{};
        if (!this->hit(ray(origin, direction), interval(0.001f, infinity), rec))
            return 0.f;
        
        auto dist_squared{ (center_at_time(0) - origin).squared_length() };
        auto cos_theta_max{ std::sqrt(1 - radius * radius / dist_squared) };
        auto solid_angle{ 2 * pi * (1 - cos_theta_max) };

        return 1 / solid_angle;
    }

    vec3 random(const point3& origin) const override {
        vec3 direction_to_center{ center_at_time(0) - origin };
        auto distance_squared{ direction_to_center.squared_length() };
        onb uvw{ direction_to_center };
        return uvw.transform(ramdom_to_sphere(radius, distance_squared));
    }
    
};
#pragma endregion

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    // point3 current_center{ center.at(r.time()) };
    point3 current_center{ center_at_time(r.time()) }; // normalized because of change in random number gen
    vec3 oc{ r.origin() - current_center }; // it needs to stay in this order otherwise its not rendering
    auto a{ r.direction().squared_length() };
    float half_b{ dot(oc, r.direction()) };
    float c{ oc.squared_length() - radius * radius };
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