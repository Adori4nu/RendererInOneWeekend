#pragma once
#include "entity.hpp"

#pragma region declaration of sphere
class sphere : public entity {
public:
    sphere() {}
    sphere(vec3 cen, float r) : center(cen), radius(r) {};
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    vec3 center{};
    float radius{};
};
#pragma endregion

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc{ r.origin() - center };
    float a{ dot(r.direction(), r.direction()) };
    float b{ dot(oc, r.direction()) };
    float c{ dot(oc, oc) - radius * radius };
    float discriminant{ b * b - a * c };
    if (discriminant > 0.f) {
        float temp{ ( -b - sqrtf(b*b-a*c) ) / a };
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = ( rec.p - center) / radius;
            return true;
        }
        temp = ( -b + sqrtf( b * b - a * c ) / a );
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = ( rec.p - center) / radius;
            return true;
        }
    }
    return false;
}