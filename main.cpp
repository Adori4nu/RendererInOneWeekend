#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <numbers>

// iclude order matters 💩
#include "camera.hpp"
#include "color.hpp"
#include "entitylist.hpp"
#include "sphere.hpp"
#include "bhv.hpp"

int main()
{
    entity_list world{};
    auto ground_material{ std::make_shared<lambertian>(color(0.5f, 0.5f, 0.5f)) };
    world.add(std::make_shared<sphere>(point3(0.f,-1000.f,0.f), 1000.f, ground_material));

    auto material1 = std::make_shared<dielectric>(1.5f);
    auto material2 = std::make_shared<lambertian>(color(0.4f, 0.2f, 0.1f));
    auto material3 = std::make_shared<metalic>(color(0.7f, 0.6f, 0.5f), 0.0f);

    point3 sphere1_center(0.f, 1.f, 0.f);
    point3 sphere2_center(-4.f, 1.f, 0.f);
    point3 sphere3_center(4.f, 1.f, 0.f);

    world.add(std::make_shared<sphere>(sphere1_center, 1.0f, material1));

    world.add(std::make_shared<sphere>(sphere2_center, 1.0f, material2));

    world.add(std::make_shared<sphere>(sphere3_center, 1.0f, material3));

    std::vector<std::pair<point3, float>> all_spheres;

    all_spheres.push_back({sphere1_center, 1.f});
    all_spheres.push_back({sphere2_center, 1.f});
    all_spheres.push_back({sphere3_center, 1.f});

    int attempts{};
    int spheres_created{};
    constexpr int max_attempts = 1'000;
    constexpr int desired_spheres = 69;

    while (spheres_created < desired_spheres && attempts < max_attempts) {
        attempts++;

        int main_sphere{ static_cast<int>(random_float(0, 3)) };

        point3 reference_center;
        if (main_sphere == 0) reference_center = sphere1_center;
        else if (main_sphere == 1) reference_center = sphere2_center;
        else reference_center = sphere3_center;

        float distance = 1.5f + random_float() * 2.5f;

        // Random direction (spherical coordinates)
        float theta = random_float() * 2 * std::numbers::pi;
        float phi = random_float() * std::numbers::pi;
        
        // Convert spherical to Cartesian coordinates
        float x = reference_center.x() + distance * sin(phi) * cos(theta);
        float y{ 0.2f };  // Keep y at 0.2 for small spheres
        float z = reference_center.z() + distance * sin(phi) * sin(theta);
        
        point3 center(x, y, z);
        float radius{ 0.2f };
        
        // Check if the position is valid (not too close to any other sphere)
        bool valid_position = true;
        for (const auto& sphere : all_spheres) {
            float min_distance{ sphere.second + radius + 0.05f };
            if ((center - sphere.first).length() < min_distance) {
                valid_position = false;
                break;
            }
        }
    
        if (valid_position) {
            all_spheres.push_back({center, radius});
            auto choose_mat = random_float();
            std::shared_ptr<material> sphere_material;
            
            if (choose_mat < 0.8f) {
                auto albedo = color::random() * color::random();
                sphere_material = std::make_shared<lambertian>(albedo);
                auto center2{ center + vec3(0, random_float(0.f,.05f), 0)};
                world.add(std::make_shared<sphere>(center, center2, radius, sphere_material));
            } else if (choose_mat < 0.95f) {
                auto albedo = color::random(0.5f, 1.f);
                auto fuzz = random_float(0.f, 0.5f);
                sphere_material = std::make_shared<metalic>(albedo, fuzz);
                world.add(std::make_shared<sphere>(center, radius, sphere_material));
            } else {
                sphere_material = std::make_shared<dielectric>(1.5f);
                world.add(std::make_shared<sphere>(center, radius, sphere_material));
            }
            
            spheres_created++;
        }
    }

    world = entity_list(std::make_shared<bhv_node>(world));
    // auto optimized_world = entity_list();
    // optimized_world.add(std::make_shared<bhv_node>(world));
    // world = optimized_world;

    camera cam;

    cam.aspect_ratio      = 16.0 / 10.0;
    cam.image_width       = 720;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 6;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
    std::string _;
    std::getline(std::cin, _);
}