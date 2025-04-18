#pragma once

#include "interval.hpp"
#include "vec3.hpp"

#pragma region AABB declaration
class aabb {

    auto pad_to_minimums() -> void {
        float delta{ 0.000'1f };
        
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }

public:
    // ther will be an issue with plane bounding boxes if there will be implementation in the future
    interval x, y, z;

    aabb() {}

    aabb(const interval& x, const interval& y, const interval& z)
        : x{x}, y{y}, z{z} { pad_to_minimums(); }

    aabb(const point3& a, const point3& b) {
        x = interval(std::fmin(a[0],b[0]), std::fmax(a[0],b[0]));
        y = interval(std::fmin(a[1],b[1]), std::fmax(a[1],b[1]));
        z = interval(std::fmin(a[2],b[2]), std::fmax(a[2],b[2]));

        pad_to_minimums();
    }

    aabb(const aabb& box0, const aabb& box1)
     : x{ interval(box0.x, box1.x) }
     , y{ interval(box0.y, box1.y) }
     , z{ interval(box0.z, box1.z) }
    {}

    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_origin{ r.origin() };
        const vec3& ray_dir{ r.direction() };

        for (int axis{}; axis < 3; ++axis) {
            const interval& ax{ axis_interval(axis) };
            const float adinv{ 1.0f / ray_dir[axis] }; // maybe switch to float to be consistent

            auto t0{ (ax.min - ray_origin[axis]) * adinv };
            auto t1{ (ax.max - ray_origin[axis]) * adinv };

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    int longest_axis() const {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty, universe;
    
};
#pragma endregion

const aabb aabb::empty    (interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe (interval::universe, interval::universe, interval::universe);

aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}