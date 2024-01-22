#pragma once
#include "entity.hpp"

#pragma region entity_list declaration
class entity_list : public entity {
public:
    entity_list() {}
    entity_list(entity **l, size_t n) { list = l; list_size = n; }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    entity **list;
    size_t list_size;
};
#pragma endregion

#pragma region entity_list definition

bool entity_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record tmp_rec;
    bool hit_anything{ false };
    float closest_so_far{ t_max };
    for (size_t i{ 0 }; i < list_size; i++)
    {
        if (list[i]->hit(r, t_min, closest_so_far, tmp_rec)) {
            hit_anything = true;
            closest_so_far = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    return hit_anything;
}

#pragma endregion