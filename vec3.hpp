#pragma once
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "rtweekend.hpp"

#pragma region vec3 declaration
class vec3 {
public:
    vec3() {}
    vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
    __forceinline float x() const { return e[0]; }
    __forceinline float y() const { return e[1]; }
    __forceinline float z() const { return e[2]; }
    __forceinline float r() const { return e[0]; }
    __forceinline float g() const { return e[1]; }
    __forceinline float b() const { return e[2]; }

    __forceinline const vec3& operator+() const { return *this; }
    __forceinline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    __forceinline float operator[](int i) const { return e[i]; }
    __forceinline float& operator[](int i) { return e[i]; }

    __forceinline vec3& operator+=(const vec3& v2);
    __forceinline vec3& operator-=(const vec3& v2);
    __forceinline vec3& operator*=(const vec3& v2);
    __forceinline vec3& operator/=(const vec3& v2);
    __forceinline vec3& operator*=(const float t);
    __forceinline vec3& operator/=(const float t);

    __forceinline float length() const { return sqrtf( e[0] * e[0] + e[1] * e[1] + e[2] * e[2] ); }
    __forceinline float sqared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    __forceinline void make_unit_vector();

    static vec3 random() {
        return vec3(random_float(), random_float(), random_float());
    }

    static vec3 random(double min, double max) {
        return vec3(random_float(min,max), random_float(min,max), random_float(min,max));
    }

    float e[3];
};
#pragma endregion

#pragma region vec3 helper function and definition
#pragma region helper functions
    using point3 = vec3;

    __forceinline std::istream& operator>>(std::istream& is, vec3& t) {
        is >> t.e[0] >> t.e[1] >> t.e[2];
        return is;
    }

    __forceinline std::ostream& operator<<(std::ostream& os, const vec3& t) {
        os << t.e[0] << " " << t.e[1] << " " << t.e[2];
        return os;
    }
#pragma endregion
#pragma region vec3 definition

    __forceinline void vec3::make_unit_vector() {
        float k{ 1.0f / sqrtf( e[0] * e[0] + e[1] * e[1] + e[2] * e[2] ) };
        e[0] *= k; e[1] *= k; e[2] *= k;
    }

    __forceinline vec3 operator+(const vec3& v1, const vec3& v2) {
        return vec3{ v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2] };
    }

    __forceinline vec3 operator-(const vec3& v1, const vec3& v2) {
        return vec3{ v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2] };
    }

    __forceinline vec3 operator*(const vec3& v1, const vec3& v2) {
        return vec3{ v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2] };
    }
    
    __forceinline vec3 operator/(const vec3& v1, const vec3& v2) {
        return vec3{ v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2] };
    }

    __forceinline vec3 operator*(float t, const vec3& v) {
        return vec3{ t * v.e[0], t * v.e[1], t * v.e[2] };
    }

    __forceinline vec3 operator/(vec3 v, float t) {
        return vec3{ v.e[0] / t, v.e[1] / t, v.e[2] / t };
    }

    __forceinline vec3 operator*(const vec3& v, float t) {
        return vec3{ t * v.e[0], t * v.e[1], t * v.e[2] };
    }

    __forceinline float dot(const vec3& v1, const vec3& v2) {
        return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
    }
    
    __forceinline vec3 cross(const vec3& v1, const vec3& v2) {
        return vec3{ (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1])
                    , (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]))
                    , (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]) };
    }

    __forceinline vec3& vec3::operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    __forceinline vec3& vec3::operator-=(const vec3& v) {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }

    __forceinline vec3& vec3::operator*=(const vec3& v) {
        e[0] *= v.e[0];
        e[1] *= v.e[1];
        e[2] *= v.e[2];
        return *this;
    }

    __forceinline vec3& vec3::operator/=(const vec3& v) {
        e[0] /= v.e[0];
        e[1] /= v.e[1];
        e[2] /= v.e[2];
        return *this;
    }

    __forceinline vec3& vec3::operator*=(float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    __forceinline vec3& vec3::operator/=(float t) {
        e[0] /= t;
        e[1] /= t;
        e[2] /= t;
        return *this;
    }

    __forceinline vec3 unit_vector(vec3 v) {
        return v / v.length();
    }

    __forceinline vec3 random_in_unit_sphere() {
        while (true) {
            auto p = vec3::random(-1.f,1.f);
            if (p.sqared_length() < 1)
                return p;
        }
    }

    __forceinline vec3 random_unit_vector() {
        return unit_vector(random_in_unit_sphere());
    }

    __forceinline vec3 random_on_hemisphere(const vec3& normal) {
        vec3 on_unit_sphere{ random_unit_vector() };
        if (dot(on_unit_sphere, normal) > 0.0f) // In the same hemisphere as the normal
            return on_unit_sphere;
        else
            return -on_unit_sphere;
    }

#pragma endregion
#pragma endregion