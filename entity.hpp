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
    float u{};
    float v{};
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

#pragma region translation/rotation object dec
class translate : public entity {

    std::shared_ptr<entity> object;
    vec3 offset;
    aabb bbox;

public:

    translate(std::shared_ptr<entity> object, const vec3& offset)
    : object{object}, offset{offset} {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        ray offset_r{r.origin() - offset, r.direction(), r.time()};

        if (!object->hit(offset_r, ray_t, rec))
            return false; 

        rec.p += offset;

        return true;
    };

    aabb bounding_box() const override { return bbox; }
};
#pragma endregion

#pragma region Y Rotation 
class rotate_y : public entity {

    std::shared_ptr<entity> object;
    float sin_theta{};
    float cos_theta{};
    aabb bbox{};

public:

    rotate_y(std::shared_ptr<entity> object, float angle) : object{object} {
        auto randians{ degrees_to_radians(angle) };
        sin_theta = std::sin(randians);
        cos_theta = std::cos(randians);
        bbox = object->bounding_box();

        point3 min{ infinity,  infinity,  infinity };
        point3 max{ -infinity, -infinity, -infinity };

        for (int i{}; i < 2; ++i) {
            for (int j{}; j < 2; ++j) {
                for (int k{}; k < 2; ++k) {
                    auto x{ i*bbox.x.max + (1-i)*bbox.x.min };
                    auto y{ j*bbox.y.max + (1-j)*bbox.y.min };
                    auto z{ k*bbox.z.max + (1-k)*bbox.z.min };

                    auto newx{  cos_theta*x + sin_theta*z };
                    auto newz{ -sin_theta*x + cos_theta*z };

                    vec3 tester{ newx, y, newz };

                    for (int c{}; c < 3; ++c) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Transform the ray from world space to object space.
        auto origin{ point3{(cos_theta * r.origin().x()) - (sin_theta * r.origin().z())
            , r.origin().y()
            , (sin_theta * r.origin().x()) + (cos_theta * r.origin().z())} };

        auto direction{ vec3{(cos_theta * r.direction().x()) - (sin_theta * r.direction().z())
            , r.direction().y()
            , (sin_theta * r.direction().x()) + (cos_theta * r.direction().z())}};

        ray rotate_r{ origin, direction, r.time() };

        // Determine whether an intersection exists in object space (and if so, where).
        
        if (!object->hit(rotate_r, ray_t, rec))
            return false;

        // Transform the intersection from object space back to world space.

        rec.p = point3{
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
            rec.p.y(),
            (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
        };

        rec.normal = vec3{
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
            rec.normal.y(),
            (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
        };

        return true;
    }

    aabb bounding_box() const override { return bbox; }
};
#pragma endregion