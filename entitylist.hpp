#pragma once
#include "entity.hpp"

#include <memory>
#include <vector>

#pragma region entity_list declaration
class entity_list : public entity {
public:
    std::vector<std::shared_ptr<entity>> entities;

    entity_list() {}
    // entity_list(entity **l, size_t n) { list = l; list_size = n; }
    entity_list(std::shared_ptr<entity> ent) { add(ent); }

    void clear() { entities.clear(); }

    void add(std::shared_ptr<entity> ent) { entities.push_back(ent); }

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    // entity **list;
    // size_t list_size;
};
#pragma endregion

#pragma region entity_list definition

// bool entity_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
bool entity_list::hit(const ray& r, interval ray_t, hit_record& rec) const {
    hit_record tmp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max };
    for (const auto& ent : entities)
    // for (size_t i{ 0 }; i < list_size; i++)
    {
        // if (list[i]->hit(r, t_min, closest_so_far, tmp_rec)) {
        if (ent->hit(r, interval(ray_t.max, closest_so_far), tmp_rec)) {
            hit_anything = true;
            closest_so_far = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    return hit_anything;
}

#pragma endregion