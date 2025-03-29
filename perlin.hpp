#pragma once

#include "rtweekend.hpp"

#pragma region Perlin Noise decl
class perlin{

    static constexpr int point_count = 256;
    float rand_float[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_perm(int* p) {
        if (!p) return;

        for (int i{}; i < point_count; i++)
            p[i] = i;
        
        permute(p, point_count);
    }

    static void permute(int* p, int n) {
        for (int i{n-1}; i > 0; --i) {
            int target{ random_int(0, i) };
            int tmp{ p[i] };
            p[i] = p[target];
            p[target] = tmp;
        }
    }

public:
    perlin() {
        for (int i{}; i < point_count; ++i) {
            rand_float[i] = random_float();
        }

        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    float noise(const point3& p) const {
        auto i{ static_cast<int>(4 * p.x()) & 255 };
        auto j{ static_cast<int>(4 * p.y()) & 255 };
        auto k{ static_cast<int>(4 * p.z()) & 255 };
        
        return rand_float[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    }
};
#pragma endregion