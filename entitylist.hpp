#pragma once
#include "aabb.hpp"
#include "entity.hpp"
#include "rtweekend.hpp"

#include <memory>
#include <vector>

#pragma region entity_list declaration
class entity_list : public entity {

    aabb bbox;

public:
    std::vector<std::shared_ptr<entity>> entities;

    entity_list() {}

    entity_list(std::shared_ptr<entity> ent) { add(ent); }

    void clear() { entities.clear(); }

    void add(std::shared_ptr<entity> ent) { 
        entities.push_back(ent);
        bbox = aabb(bbox, ent->bounding_box());
    }

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override { return bbox; }

    float pdf_value(const point3& origin, const vec3& direction) const override {
        float weight{ 1.f / entities.size() };
        float sum{ 0.f };

        for (const auto& ent : entities)
            sum += weight * ent->pdf_value(origin, direction);

        return sum;
    }

    vec3 random(const point3& origin) const override {
        int index{ random_int(0, static_cast<int>(entities.size() - 1)) };
        return entities[index]->random(origin);
    }

};
#pragma endregion

#pragma region entity_list definition


bool entity_list::hit(const ray& r, interval ray_t, hit_record& rec) const {
    hit_record tmp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max };
    for (const auto& ent : entities)
    {
        if (ent->hit(r, interval(ray_t.min, closest_so_far), tmp_rec)) {
            hit_anything = true;
            closest_so_far = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    return hit_anything;
}

#pragma endregion