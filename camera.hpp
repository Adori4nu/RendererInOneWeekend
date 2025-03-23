#pragma once
#include "rtweekend.hpp"

#include "color.hpp"
#include "entity.hpp"
#include "interval.hpp"
#include "material.hpp"

#include "threading/thread_pool.hpp"

#include <iostream>

#pragma region camera class declaration
class camera {
public:
    float aspect_ratio{ 1.0f };
    int image_width{ 100 };
    int samples_per_pixel{ 10 };
    int max_depth{ 10 };

    float vfov{ 90.f };
    point3 lookfrom{ point3{ 0, 0, -1 } };
    point3 lookat{ point3{ 0, 0, 0 } };
    vec3 vup{ vec3{ 0, 1, 0 } };

    float defocus_angle{ 0.f };
    float focus_dist{ 10.f };

    vec3 lower_left_corner{};
    vec3 horizontal{};
    vec3 vertical{};
    vec3 origin{};

    void render(const entity& world) {
        initialize();

        std::vector<color> frame_buffer(image_width * image_height);

        thread_pool_ws thread_pool;

        const int tile_size{ 16 };

        std::clog << "Rendering..." << std::endl;

        std::vector<std::future<void>> futures;

        for (int j = 0; j < image_height; j += tile_size) {
            for (int i = 0; i < image_width; i += tile_size) {
                auto future = thread_pool.submit([this, &world, &frame_buffer, i, j, tile_size]() {
                    try
                    {
                        int x_end{ std::min(i + tile_size, image_width) };
                        int y_end{ std::min(i + tile_size, image_width) };

                        // Render pixels in tile
                        for (int y{j}; y < y_end; ++y) {
                            for (int x{i}; x < x_end; ++x) {
                                color pixel_color{0, 0, 0};

                                for (int sample{ 0 }; sample < samples_per_pixel; ++sample)
                                {
                                    ray r{ get_ray(x, y) };
                                    pixel_color += ray_color(r, max_depth, world);
                                }

                                frame_buffer[y * image_width + x] = pixel_color;
                            }
                        }
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << "\nException in tile [" << i << "," << j << "]" << " " << e.what() << std::endl;
                    }
                    catch(...)
                    {
                        std::cerr << "\nUnknown exception in tile [" << i << "," << j << "]" << std::endl;
                    }
                    
                });

                futures.push_back(std::move(future));
            }
        }

        int completed_futures{};
        size_t total_futures{ futures.size() };

        for (auto& future: futures) {
            future.wait();
            completed_futures++;
            
            std::clog << "\rCompleted: " << completed_futures << "/" << total_futures
                      << " tiles (" << (completed_futures * 100 / total_futures) << "%)" << std::flush;
        }

        std::ofstream file("renderer_output.ppm");
        if (!file.is_open())
        {
            file.clear();
            file.open("renderer_output.ppm", std::ios::out);
        }
        if (file.is_open())
        {
            file.clear();
            file << "P3\n" << image_width << " " << image_height << "\n255\n";
            for (int j{ 0 }; j < image_height; ++j) {
                for (int i{ 0 }; i < image_width; ++i) {
                    write_color(file, frame_buffer[j * image_width + i], samples_per_pixel);
                }
            }
            file.close();

            std::clog << "\rDone.           \n";
        }
        else { std::cerr << "Unable to open file for writing." << std::endl; }
    }

private:
    int image_height{};
    point3 center{};
    point3 pixel00_loc{};
    vec3 pixel_delta_u{};
    vec3 pixel_delta_v{};
    vec3 u{}, v{}, w{};
    vec3 defocus_disk_u{};
    vec3 defocus_disk_v{};

    void initialize();
    ray get_ray(int i, int j) const;
    vec3 pixel_sample_square() const;
    vec3 pixel_sample_disk(double radius) const;
    point3 defocus_disk_sample() const;
    color ray_color(const ray& r, int depth, const entity& world) const;
};
#pragma endregion

inline void camera::initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = ( image_height < 1 ) ? 1 : image_height;

    center = lookfrom;

    auto theta{ degrees_to_radians(vfov) };
    auto h{ tan( theta / 2 ) };
    auto viewport_height{ 2 * h * focus_dist };
    auto viewport_width{ viewport_height * (static_cast<float>(image_width) / image_height ) };

    w = unit_vector( lookfrom - lookat );
    u = unit_vector( cross( vup, w ) );
    v = cross( w, u );

    vec3 viewport_u{ viewport_width * u };
    vec3 viewport_v{ viewport_height * -v };

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left{ center - ( focus_dist * w ) - viewport_u / 2 - viewport_v / 2 };
    pixel00_loc = viewport_upper_left + 0.5f * ( pixel_delta_u + pixel_delta_v );

    auto defocus_radius = focus_dist * tan( degrees_to_radians( defocus_angle / 2 ) );
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;

}

inline ray camera::get_ray(int i, int j) const
{
    auto pixel_center{ pixel00_loc + ( i * pixel_delta_u ) + ( j * pixel_delta_v ) };
    auto pixel_sample{ pixel_center + pixel_sample_square() };

    auto ray_origin{ ( defocus_angle <= 0 ) ? center : defocus_disk_sample() };
    auto ray_direction{ pixel_sample - ray_origin };

    return ray(ray_origin, ray_direction);
}

inline vec3 camera::pixel_sample_square() const
{
    auto px{ -0.5f + random_float() };
    auto py{ -0.5f + random_float() };
    return ( px * pixel_delta_u ) + ( py * pixel_delta_v );
}

inline vec3 camera::pixel_sample_disk(double radius) const
{
    auto p{ radius * random_in_unit_disk() };
    return ( p[0] * pixel_delta_u ) + ( p[1] * pixel_delta_v );
}

inline point3 camera::defocus_disk_sample() const
{
    auto p{ random_in_unit_disk() };
    return center + ( p[0] * defocus_disk_u ) + ( p[1] * defocus_disk_v );
}

inline color camera::ray_color(const ray &r, int depth, const entity &world) const
{
    if (depth <= 0)
        return color{ 0.f, 0.f, 0.f };
    
    hit_record rec{};

    if (world.hit(r, interval(0.001f, infinity), rec))
    {
        ray scattered{};
        color attenuation{};
        if (rec.mat->scatter( r, rec, attenuation, scattered ))
            return attenuation * ray_color(scattered, depth - 1, world);
        return color{ 0.f, 0.f, 0.f };
    }

    vec3 unit_direction{ unit_vector( r.direction() ) };
    float a{ 0.5f * ( unit_direction.y() + 1.0f ) };
    return ( 1.0f - a ) * color{ 1.0f, 1.0f, 1.0f } + a * color{ 0.5f, 0.7f, 1.0f };
}
