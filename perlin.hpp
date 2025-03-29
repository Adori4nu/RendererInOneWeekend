#pragma once

#include "rtweekend.hpp"

#pragma region Perlin Noise decl
class perlin{

    static constexpr int point_count = 256;
    // float rand_float[point_count];
    vec3 randvec[point_count];
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

    static float trilinear_interp(float c[2][2][2], float u, float v, float w) {
        auto accum{ 0.f };
        for (int i{}; i < 2; ++i)
            for (int j{}; j < 2; ++j)
                for (int k{}; k < 2; ++k)
                    accum += (i*u + (1-i) * (1-u))
                            * (j*v + (1-j) * (1-v))
                            * (k*w + (1-k) * (1-w))
                            * c[i][j][k];
        return accum;
    }

    static float perlin_interp(const vec3 c[2][2][2], float u, float v, float w) {
        auto uu{u*u*(3-2*u)};
        auto vv{v*v*(3-2*v)};
        auto ww{w*w*(3-2*w)};
        auto accum{ 0.f };
        
        for (int i{}; i < 2; ++i)
            for (int j{}; j < 2; ++j)
                for (int k{}; k < 2; ++k) {
                    vec3 weight_v(u - i, v - j, w - k);
                    accum += (i*u + (1-i) * (1-u))
                            * (j*v + (1-j) * (1-v))
                            * (k*w + (1-k) * (1-w))
                            * dot(c[i][j][k], weight_v);
                }

        return accum;
    }

public:
    perlin() {
        for (int i{}; i < point_count; ++i) {
            randvec[i] = unit_vector(vec3::random(-1.f,1.f));
        }

        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    float noise(const point3& p) const {
        auto u{ p.x() - std::floor(p.x()) };
        auto v{ p.y() - std::floor(p.y()) };
        auto w{ p.z() - std::floor(p.z()) };
        // u = u*u*(3-2*u);
        // v = v*v*(3-2*v);
        // w = w*w*(3-2*w);

        auto i{ static_cast<int>(std::floor(p.x())) };
        auto j{ static_cast<int>(std::floor(p.y())) };
        auto k{ static_cast<int>(std::floor(p.z())) };
        vec3 c[2][2][2];

        for (int di{}; di < 2; ++di){
            for (int dj{}; dj < 2; ++dj) {
                for (int dk{}; dk < 2; ++dk) {
                    c[di][dj][dk] = randvec[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ];
                }
            }
        }

        // return trilinear_interp(c, u, v, w);
        return perlin_interp(c, u, v, w);
    }
};
#pragma endregion