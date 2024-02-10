#pragma once
#include "color.hpp"
#include "entitylist.hpp"
#include "rtweekend.hpp"

#pragma region declaration of abstract material
class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

#pragma endregion

#pragma region declaration of lambertian

class lambertian : public material {
public:
    lambertian(const color& a)
        : albedo(a)
         {};

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;

private:
    color albedo{};
};

#pragma endregion

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
{
    auto scatter_direction{ rec.normal + random_unit_vector() };

    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;

    return true;
}

#pragma region declaration of metalic
class metalic : public material {
public:
    metalic(const color& a, float f)
        : albedo(a)
        , fuzz(f < 1.f ? f : 1.f) // aka reflection sharpness
         {};
    
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
private:
    color albedo{};
    float fuzz{};
};


#pragma endregion

bool metalic::scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
{
    auto reflected{ reflect(unit_vector(r_in.direction()), rec.normal) };
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
}

#pragma region declaration of dielectric
class dielectric : public material {
public:
    dielectric(float index_of_refraction = 1.f)
        : ir(index_of_refraction)
         {};

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
private:
    float ir{};

    static float reflectance(float cosine, float ref_idx) {
        auto r0{ ( 1 - ref_idx ) / ( 1 + ref_idx ) };
        r0 = r0 * r0;
        return r0 + ( 1 - r0 ) * pow(( 1 - cosine ), 5);
    }
};

#pragma endregion

bool dielectric::scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
{
    attenuation = color{ 1.f, 1.f, 1.f };
    float refraction_ratio{ rec.front_face ? (1.0f/ir) : ir };
    vec3 unit_direction{ unit_vector(r_in.direction()) };
    float cos_theta{ fmin(dot(-unit_direction, rec.normal), 1.0f) };
    float sin_theta{ sqrt(1.0f - cos_theta * cos_theta) };

    bool cannot_refract{ refraction_ratio * sin_theta > 1.0f };
    vec3 direction{};

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, refraction_ratio);

    scattered = ray(rec.p, direction);
    return true;
}