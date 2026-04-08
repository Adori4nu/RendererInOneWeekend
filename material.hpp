#pragma once
#include "color.hpp"
#include "entitylist.hpp"
#include "pdf.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include <memory>

#pragma region SCATER RECORD
struct scatter_record {
    color attenuation{};
    std::shared_ptr<pdf> pdf_ptr;
    bool skip_pdf{};
    ray skip_pdf_ray{};
};
#pragma endregion
#pragma region ABSTRACT material declaration
class material {
public:
    virtual ~material() = default;

    virtual color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const {
        return color{ 0.f, 0.f, 0.f };
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const { return false; }

    virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const { return 0.f; }
};

#pragma endregion

#pragma region LAMBERTIAN declaration

class lambertian : public material {

    std::shared_ptr<texture> albedo_texture{};

public:
    
    lambertian(const color& albedo)
        : albedo_texture(std::make_shared<solid_color>(albedo)) {}
    
    lambertian(std::shared_ptr<texture> tex) : albedo_texture{ tex } {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override;

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
};

#pragma endregion

bool lambertian::scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = albedo_texture->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;
    return true;
}

float lambertian::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
    auto cos_theta{ dot(rec.normal, unit_vector(scattered.direction())) };
    return cos_theta < 0 ? 0 : cos_theta / pi;
    // return 1 / (2 * pi);
}

#pragma region METALIC declaration
class metalic : public material {
    
    color albedo{};
    float fuzz{};

public:
    metalic(const color& a, float f)
        : albedo(a)
        , fuzz(f < 1.f ? f : 1.f) // aka reflection sharpness
         {};
    
    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override;

};

#pragma endregion

bool metalic::scatter(const ray &r_in, const hit_record &rec, scatter_record& srec) const
{
    auto reflected{ unit_vector(reflect(r_in.direction(), rec.normal)) + fuzz * random_in_unit_sphere() };
    srec.attenuation = albedo;
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    srec.skip_pdf_ray = ray(rec.p, reflected, r_in.time());

    return true;
}

#pragma region DIELECTRIC declaration
class dielectric : public material {
public:
    dielectric(float index_of_refraction = 1.f)
        : refraction_index(index_of_refraction)
         {};

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override;

private:
    float refraction_index{};

    static float reflectance(float cosine, float ref_idx) {
        auto r0{ ( 1 - ref_idx ) / ( 1 + ref_idx ) };
        r0 = r0 * r0;
        return r0 + ( 1 - r0 ) * pow(( 1 - cosine ), 5);
    }
};

#pragma endregion

bool dielectric::scatter(const ray &r_in, const hit_record &rec, scatter_record& srec) const
{
    srec.attenuation = color(1.0, 1.0, 1.0);
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    float refraction_ratio{ rec.front_face ? ( 1.0f / refraction_index ) : refraction_index };
    
    vec3 unit_direction{ unit_vector(r_in.direction()) };
    float cos_theta{ fmin(dot(-unit_direction, rec.normal), 1.0f) };
    float sin_theta{ sqrt(1.0f - cos_theta * cos_theta) };

    bool cannot_refract{ refraction_ratio * sin_theta > 1.0f };
    vec3 direction{};

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, refraction_ratio);

    srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());
    return true;
}

#pragma region EMISSION declaration
class diffuse_light : public material {

    std::shared_ptr<texture> tex{};

public:

    diffuse_light(std::shared_ptr<texture> tex) : tex{tex} {}
    diffuse_light(const color& emit) : tex{std::make_shared<solid_color>(emit)} {}

    color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const override {
        if (!rec.front_face)
            return color{ 0.f, 0.f, 0.f };
        return tex->value(u, v, p);
    }

};
#pragma endregion

#pragma region ISOTROPIC declaration
class isotropic : public material {
    
    std::shared_ptr<texture> tex;

public:

    isotropic(const color& albedo) : tex{std::make_shared<solid_color>(albedo) }
    {}

    isotropic(std::shared_ptr<texture> tex) : tex{tex} {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = std::make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        return 1 / (4 * pi);
    }
};
#pragma endregion