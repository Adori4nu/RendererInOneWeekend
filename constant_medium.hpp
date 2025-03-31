#pragma once

#include "entity.hpp"
#include "material.hpp"
#include "texture.hpp"

#pragma region constant Fog volume
// volume and isotropic mat is naivly implemented only works for convex shapes that
// once the ray exit volume it cant never re-enter
// this wont work if for example we implement toruses 
class constant_medium : public entity {

    std::shared_ptr<entity> boundary;
    float neg_inv_density;
    std::shared_ptr<material> phase_function;

public:

    constant_medium(std::shared_ptr<entity> boundary, float density, std::shared_ptr<texture> tex)
        : boundary{ boundary }, neg_inv_density{ -1 / density }
        , phase_function{ std::make_shared<isotropic>(tex) }
        {}
    
    constant_medium(std::shared_ptr<entity> boundary, float density, const color& albedo)
        : boundary{ boundary }, neg_inv_density{ -1 / density }
        , phase_function{ std::make_shared<isotropic>(albedo) }
        {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record rec1, rec2;

        if (!boundary->hit(r, interval::universe, rec1))
            return false;
        
        if (!boundary->hit(r, interval{rec1.t + 0.000'1f, infinity}, rec2))
            return false;
        
        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length{ r.direction().length() };
        auto distance_inside_boundary{ (rec2.t - rec1.t) * ray_length };
        auto hit_distance{ neg_inv_density * std::log(random_float()) };

        if (hit_distance > distance_inside_boundary)
            return false;
        
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = vec3(1.f,0.f,0.f);  // arbitrary
        rec.front_face = true;           // also arbitrary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }
};
#pragma endregion