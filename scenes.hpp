#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <numbers>
#include <print>

// iclude order matters 💩
#include "camera.hpp"
#include "color.hpp"
#include "entitylist.hpp"
#include "sphere.hpp"
#include "bhv.hpp"
#include "texture.hpp"
#include "quad.hpp"

auto bouncing_spheres() -> int
{
    entity_list world{};
    auto ground_material{ std::make_shared<lambertian>(color(0.5f, 0.5f, 0.5f)) };
    auto checker_pattern{ std::make_shared<checker_texture>(.32f, color(.011f, 0.38f, 0.392f), color(0.878f, 0.509f, 0.615f)) };
    world.add(std::make_shared<sphere>(point3(0.f,-1000.f,0.f), 1000.f, std::make_shared<lambertian>(checker_pattern)));

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

    camera cam;

    cam.aspect_ratio      = 16.0 / 10.0;
    cam.image_width       = 720;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 6;
    cam.background        = color(0.70f, 0.80f, 1.00f);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31ERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}

auto two_spheres_scene() -> int {
    entity_list world;

    auto checker = std::make_shared<checker_texture>(0.32f, color(.2f, .3f, .1), color(.9f, .9f, .9f));

    world.add(std::make_shared<sphere>(point3(0.f,-10.f, 0.f), 10.f, std::make_shared<lambertian>(checker)));
    world.add(std::make_shared<sphere>(point3(0.f, 10.f, 0.f), 10.f, std::make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 10.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70f, 0.80f, 1.00f);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}

auto earth() -> int {
    auto earth_texture = std::make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = std::make_shared<lambertian>(earth_texture);
    auto globe = std::make_shared<sphere>(point3(0.f,0.f,0.f), 2.f, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0f / 9.0f;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70f, 0.80f, 1.00f);

    cam.vfov     = 20.f;
    cam.lookfrom = point3(0.f,0.f,12.f);
    cam.lookat   = point3(0.f,0.f,0.f);
    cam.vup      = vec3(0.f,1.f,0.f);

    cam.defocus_angle = 0.f;

    
    try {
        cam.render(entity_list(globe));
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}

auto perlin_spheres() -> int{
    entity_list world;

    auto pertext = std::make_shared<noise_texture>(4.f);
    world.add(std::make_shared<sphere>(point3(0.f,-1000.f,0.f), 1000.f, std::make_shared<lambertian>(pertext)));
    world.add(std::make_shared<sphere>(point3(0.f,2.f,0.f), 2.f, std::make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70f, 0.80f, 1.00f);

    cam.vfov     = 20.f;
    cam.lookfrom = point3(13.f,2.f,3.f);
    cam.lookat   = point3(0.f,0.f,0.f);
    cam.vup      = vec3(0.f,1.f,0.f);

    cam.defocus_angle = 0.f;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}

auto quads() -> int {
    entity_list world;

    // Materials
    auto left_red     = std::make_shared<lambertian>(color(1.0f, 0.2f, 0.2f));
    auto back_green   = std::make_shared<lambertian>(color(0.2f, 1.0f, 0.2f));
    auto right_blue   = std::make_shared<lambertian>(color(0.2f, 0.2f, 1.0f));
    auto upper_orange = std::make_shared<lambertian>(color(1.0f, 0.5f, 0.0f));
    auto lower_teal   = std::make_shared<lambertian>(color(0.2f, 0.8f, 0.8f));

    // Quads
    world.add(std::make_shared<quad>(point3(-3.f,-2.f, 5.f), vec3(0.f, 0.f,-4.f), vec3(0.f, 4.f, 0.f), left_red));
    world.add(std::make_shared<quad>(point3(-2.f,-2.f, 0.f), vec3(4.f, 0.f, 0.f), vec3(0.f, 4.f, 0.f), back_green));
    world.add(std::make_shared<quad>(point3( 3.f,-2.f, 1.f), vec3(0.f, 0.f, 4.f), vec3(0.f, 4.f, 0.f), right_blue));
    world.add(std::make_shared<quad>(point3(-2.f, 3.f, 1.f), vec3(4.f, 0.f, 0.f), vec3(0.f, 0.f, 4.f), upper_orange));
    world.add(std::make_shared<quad>(point3(-2.f,-3.f, 5.f), vec3(4.f, 0.f, 0.f), vec3(0.f, 0.f,-4.f), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0f;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70f, 0.80f, 1.00f);

    cam.vfov     = 80.f;
    cam.lookfrom = point3(0.f,0.f,9.f);
    cam.lookat   = point3(0.f,0.f,0.f);
    cam.vup      = vec3(0.f,1.f,0.f);

    cam.defocus_angle = 0.f;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}

auto simple_light() -> int {
    entity_list world;

    auto pertext = std::make_shared<noise_texture>(4.f);
    world.add(std::make_shared<sphere>(point3(0.f,-1000.f,0.f), 1000.f, std::make_shared<lambertian>(pertext)));
    world.add(std::make_shared<sphere>(point3(0.f,2.f,0.f), 2.f, std::make_shared<lambertian>(pertext)));

    auto difflight = std::make_shared<diffuse_light>(color(4.f,4.f,4.f));
    world.add(std::make_shared<sphere>(point3(0.f,7.f,0.f), 2.f, difflight));
    world.add(std::make_shared<quad>(point3(3.f,1.f,-2.f), vec3(2.f,0.f,0.f), vec3(0.f,2.f,0.f), difflight));

    camera cam;

    cam.aspect_ratio      = 16.0f / 9.0f;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.f,0.f,0.f);

    cam.vfov     = 20.f;
    cam.lookfrom = point3(26.f,3.f,6.f);
    cam.lookat   = point3(0.f,2.f,0.f);
    cam.vup      = vec3(0.f,1.f,0.f);

    cam.defocus_angle = 0.f;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}


auto cornell_box() -> int {
    entity_list world;

    auto red   = std::make_shared<lambertian>(color(.65f, .05f, .05f));
    auto white = std::make_shared<lambertian>(color(.73f, .73f, .73f));
    auto green = std::make_shared<lambertian>(color(.12f, .45f, .15f));
    auto light = std::make_shared<diffuse_light>(color(15.f, 15.f, 15.f));

    world.add(std::make_shared<quad>(point3(555.f,0.f,0.f), vec3(0.f,555.f,0.f), vec3(0.f,0.f,555.f), green));
    world.add(std::make_shared<quad>(point3(0.f,0.f,0.f), vec3(0.f,555.f,0.f), vec3(0.f,0.f,555.f), red));
    world.add(std::make_shared<quad>(point3(343.f, 554.f, 332.f), vec3(-130.f,0.f,0.f), vec3(0.f,0.f,-105.f), light));
    world.add(std::make_shared<quad>(point3(0.f,0.f,0.f), vec3(555.f,0.f,0.f), vec3(0.f,0.f,555.f), white));
    world.add(std::make_shared<quad>(point3(555.f,555.f,555.f), vec3(-555.f,0.f,0.f), vec3(0.f,0.f,-555), white));
    world.add(std::make_shared<quad>(point3(0.f,0.f,555.f), vec3(555.f,0,0), vec3(0.f,555.f,0.f), white));

    world.add(box(point3(130.f, 0.f, 65.f), point3(295.f, 165.f, 230.f), white));
    world.add(box(point3(265.f, 0.f, 295.f), point3(430.f, 330.f, 460.f), white));

    camera cam;

    cam.aspect_ratio      = 1.0f;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0.f,0.f,0.f);

    cam.vfov     = 40.f;
    cam.lookfrom = point3(278.f, 278.f, -800.f);
    cam.lookat   = point3(278.f, 278.f, 0.f);
    cam.vup      = vec3(0.f,1.f,0.f);

    cam.defocus_angle = 0.f;

    try {
        cam.render(world);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mERROR:\033[0m " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "\033[1;31mUnknown error occurred!\033[0m" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}