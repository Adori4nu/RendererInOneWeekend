#pragma once
#include <cmath>
#include <cstdlib>
#include <iostream>

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

    __forceinline float length() const { return sqrt( e[0] * e[0] + e[1] * e[1] + e[2] * e[2] ); }
    __forceinline float sqared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    __forceinline void make_unit_vector();

    float e[3];
};
#pragma endregion

