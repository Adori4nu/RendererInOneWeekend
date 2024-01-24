#pragma once
#include "color.hpp"
#include "entitylist.hpp"
#include "rtweekend.hpp"

class uber_material {
public:
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const;

private:
    color albedo;
};