#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>

#include "camera.hpp"
#include "color.hpp"
#include "entitylist.hpp"
#include "sphere.hpp"

int main()
{
    entity_list world{};
    auto ground_material{ std::make_shared<lambertian>(color(0.5f, 0.5f, 0.5f)) };
    world.add(std::make_shared<sphere>(point3(0.f,-1000.f,0.f), 1000.f, ground_material));

    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            auto choose_mat{ random_float() };
            point3 center{ a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float() };
            
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;
                
                if (choose_mat < 0.8f ) {
                    auto albedo = color::random() * color::random();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                } else if (choose_mat < 0.95f) {
                    auto albedo = color::random(0.5f, 1.f);
                    auto fuzz = random_float(0.f, 0.5f);
                    sphere_material = std::make_shared<metalic>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                } else {
                    sphere_material = std::make_shared<dielectric>(1.5f);
                    world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5f);
    world.add(std::make_shared<sphere>(point3(0.f, 1.f, 0.f), 1.0f, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4f, 0.2f, 0.1f));
    world.add(std::make_shared<sphere>(point3(-4.f, 1.f, 0.f), 1.0f, material2));

    auto material3 = std::make_shared<metalic>(color(0.7f, 0.6f, 0.5f), 0.0f);
    world.add(std::make_shared<sphere>(point3(4.f, 1.f, 0.f), 1.0f, material3));

    camera cam;

    cam.aspect_ratio      = 16.0 / 10.0;
    cam.image_width       = 120;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 20;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
    std::string _;
    std::getline(std::cin, _);
}