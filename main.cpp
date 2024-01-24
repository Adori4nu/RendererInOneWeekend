#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

#include "camera.hpp"
#include "color.hpp"
#include "entitylist.hpp"
#include "sphere.hpp"

vec3 ray_color(const ray& r, const entity& world, int depth) {

    if (depth <= 0)
        return color{ 0, 0, 0 };

    hit_record rec;
    float near_zero{ std::nextafterf(0.f, 1.f) };
    //, std::numeric_limits<float>::max()
    if (world.hit(r, interval(near_zero, pos_infinity), rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered)) // material dont have scatter function yet
            return attenuation * ray_color(scattered, world, depth - 1);
        return color{ 0, 0, 0 };
    }

    vec3 unit_direction{ unit_vector(r.direction()) };
    float t{ 0.5f * ( unit_direction.y() + 1.0f ) };
    return ( 1.0f - t ) * vec3{ 1.0f, 1.0f, 1.0f } + t * vec3{ 0.5f, 0.7f, 1.0f };
}

int main()
{
    float from{ 0.f };
    float to{ std::nexttowardf(1.f, 0.f) };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(from, to);

    int nx{ 200 };
    int ny{ 100 };
    int ns{ 100 };
    
    entity *list[2];
    list[0] = new sphere{ vec3{ 0.f, 0.f, -1.f }, 0.5f };
    list[1] = new sphere{ vec3{ 0.f, -100.5f, -1.f }, 100.f };
    entity *world{ new entity_list{ list, 2 } };

    camera cam;

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
            std::cout << dis(gen) << "\n";
            for (int i{ 0 }; i < nx; i++)
            {
                vec3 col{ 0.f, 0.f, 0.f };
                for (int s{ 0 }; s < ns; s++)
                {
                    float u{ float(i + dis(gen)) / float(nx) };
                    float v{ float(j + dis(gen)) / float(ny) };
                    ray r{ cam.get_ray(u, v) };
                    vec3 p{ r.at(2.f) };
                    col += ray_color(r, world);
                }
                col /= float(ns);
                col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
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