#pragma once

#include "vec3.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "aabb.hpp"
#include "entity.hpp"
#include "entitylist.hpp"
#include "rtweekend.hpp"

#include <algorithm>

#pragma region BHV decl
class bhv_node : public entity {

    std::shared_ptr<entity> left;
    std::shared_ptr<entity> right;
    aabb bbox;

    static bool box_compare(const std::shared_ptr<entity> a, const std::shared_ptr<entity> b
        , int axis_index) {
        auto a_axis_interval{ a->bounding_box().axis_interval(axis_index) };
        auto b_axis_interval{ b->bounding_box().axis_interval(axis_index) };
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const std::shared_ptr<entity> a, const std::shared_ptr<entity> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<entity> a, const std::shared_ptr<entity> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<entity> a, const std::shared_ptr<entity> b) {
        return box_compare(a, b, 2);
    }

public:

    bhv_node(entity_list list) : bhv_node(list.entities, 0, list.entities.size()) {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    bhv_node(std::vector<std::shared_ptr<entity>>& entities, size_t start, size_t end) {
        
        bbox = aabb::empty;
        for (size_t ent_index=start; ent_index < end; ++ent_index)
            bbox = aabb(bbox, entities[ent_index]->bounding_box());

        int axis = bbox.longest_axis();

        auto comparator{ (axis == 0) ? box_x_compare
                       : (axis == 1) ? box_y_compare
                                     : box_z_compare };

        size_t object_span{ end - start };

        if (object_span == 1) {
            left = right = entities[start];
        } else if (object_span == 2) {
            left = entities[start];
            right = entities[start + 1];
        } else {
            std::sort(std::begin(entities) + start, std::begin(entities) + end, comparator );

            auto mid{ start + object_span / 2 };
            left = std::make_shared<bhv_node>(entities, start, mid);
            right = std::make_shared<bhv_node>(entities, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;
        
        bool hit_left{ left->hit(r, ray_t, rec) };
        bool hit_right{ right->hit(r
                , interval(ray_t.min, hit_left ? rec.t : ray_t.max)
                , rec) };
        
        return hit_left || hit_right;
    }
    
    aabb bounding_box() const override { return bbox; }
};
#pragma endregion