#pragma once

#include "color.hpp"
#include "vec3.hpp"
#include "rtw_stb_image.hpp"

#pragma region Texture decl
class texture {
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};
#pragma endregion

#pragma region SolidColor decl
class solid_color : public texture {

    color albedo{127.f, 127.f, 127.f};

public:

    solid_color() = default;

    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(float r, float g, float b) : solid_color(color(r, g, b)) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }
};
#pragma endregion

#pragma region CheckerTexture decl
class checker_texture : public texture {
    
    float inv_scale;
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;

public:

    checker_texture(float scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
        : inv_scale(1.f / scale), even(even), odd(odd) {}

    checker_texture(float scale, const color& c1, const color& _c2)
        : checker_texture{ scale, std::make_shared<solid_color>(c1), std::make_shared<solid_color>(_c2) } {}

    color value(double u, double v, const point3& p) const override {
        auto x_integer{ static_cast<int>(std::floor(inv_scale * p.x())) };
        auto y_integer{ static_cast<int>(std::floor(inv_scale * p.y())) };
        auto z_integer{ static_cast<int>(std::floor(inv_scale * p.z())) };
        
        bool is_even{ (x_integer + y_integer + z_integer) % 2 == 0 };

        return is_even ? even->value(u, v, p) : odd->value(u, v, p);
    }
};
#pragma endregion

#pragma region ImageTexture decl
class image_texture : public texture {

    rtw_image image;

public:

    image_texture(std::string_view filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color{ 0, 1, 1 };

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v);

        auto i{ int(u * image.width()) };
        auto j{ int(v * image.height()) };
        auto pixel{ image.pixel_data(i,j) };
        auto color_scale{ 1.f / 255 };

        return color{ color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2] };
    }
};
#pragma endregion