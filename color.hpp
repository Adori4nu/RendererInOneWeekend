#pragma once
#include <iostream>

#include "interval.hpp"
#include "vec3.hpp"

using color = vec3;

__forceinline float linear_to_gamma(float linear_component) {
    return sqrtf(linear_component);
}

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0f / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0,255] value of each color component.
    // std::nexttowardf(1.f, 0.f)
    static const interval intensity(0.f, 0.999f);
    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

void save_ppm_binary(const std::string& filename, const std::vector<color>& frame_buffer,
    int image_width, int image_height, int samples_per_pixel) {
    // Open file in binary mode
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
    return;
    }

    // Write PPM header as a single string
    std::string header = "P6\n" + std::to_string(image_width) + " " + 
            std::to_string(image_height) + "\n255\n";
    file.write(header.c_str(), header.length());

    // Allocate buffer for entire image at once
    std::vector<unsigned char> buffer(image_width * image_height * 3);

    // Fill the buffer with pixel data
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            const color& pixel_color = frame_buffer[j * image_width + i];

            // Apply scaling and gamma correction
            float scale = 1.0f / samples_per_pixel;
            float r{ linear_to_gamma(pixel_color.x() * scale) };
            float g{ linear_to_gamma(pixel_color.y() * scale) };
            float b{ linear_to_gamma(pixel_color.z() * scale) };

            // Convert to 0-255 range bytes
            static const interval intensity(0.f, 0.999f);
            unsigned char r_byte = static_cast<unsigned char>(256 * intensity.clamp(r));
            unsigned char g_byte = static_cast<unsigned char>(256 * intensity.clamp(g));
            unsigned char b_byte = static_cast<unsigned char>(256 * intensity.clamp(b));

            // Store in buffer (note: row-major order, RGB sequence)
            size_t pixel_index = (j * image_width + i) * 3;
            buffer[pixel_index + 0] = r_byte;
            buffer[pixel_index + 1] = g_byte;
            buffer[pixel_index + 2] = b_byte;
        }
    }

    // Write the entire buffer at once
    file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

    file.close();

    std::cout << "Binary PPM file saved: " << filename << std::endl;
}