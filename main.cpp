#include <fstream>
#include <iostream>
#include "ray.hpp"

float hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc{ r.origin() - center };
    float a{ dot(r.direction(), r.direction()) };
    float b{ 2.0f * dot(oc, r.direction()) };
    float c{ dot(oc, oc) - radius * radius };
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return -1.0f;
    else return ( -b - sqrtf(discriminant) ) / ( 2.0f * a );
}

vec3 color(const ray& r) {
    float t{ hit_sphere(vec3{ 0.f, 0.f, -1.f }, 0.5f, r) };
    if ( t > 0.0f ) {
        vec3 N{ unit_vector(r.point_at_parameter(t) - vec3{ 0.f, 0.f, -1.f }) };
        return 0.5f * vec3{ N.x() + 1.f, N.y() + 1.f, N.z() + 1.f };
    }
    vec3 unit_direction{ unit_vector(r.direction()) };
    t = {0.5f * ( unit_direction.y() + 1.0f )};
    return ( 1.0f - t ) * vec3{ 1.0f, 1.0f, 1.0f } + t * vec3{ 0.5f, 0.7f, 1.0f };
}

int main()
{
    int nx{200};
    int ny{100};

    vec3 lower_left_corner{ -2.0f, -1.0f, -1.0f };
    vec3 horizontal{ 4.0f, 0.0f, 0.0f };
    vec3 vertical{ 0.0f, 2.0f, 0.0f };
    vec3 origin{ 0.0f, 0.0f, 0.0f };

    std::ofstream file("renderer_output.ppm");
    if(!file.is_open())
    {
        file.clear();
        file.open("renderer_output.ppm", std::ios::out);
    }
    if (file.is_open())
    {
        file.clear();
        file << "P3\n" << nx << " " << ny << "\n255\n";
        for (int j{ ny - 1 }; j >= 0; j--)
        {
            for (int i{ 0 }; i < nx; i++)
            {
                float u{ float(i) / float(nx) };
                float v{ float(j) / float(ny) };
                ray r{ origin, lower_left_corner + u * horizontal + v * vertical };
                vec3 col{ color(r) };
                int ir{ int(255.99f * col.r()) };
                int ig{ int(255.99f * col.g()) };
                int ib{ int(255.99f * col.b()) };
                file << ir << " " << ig << " " << ib << "\n";
            }
        }
        file.close();
    }
    else { std::cerr << "Unable to open file for writing." << std::endl; }
}