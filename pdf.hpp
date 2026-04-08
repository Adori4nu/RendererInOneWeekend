#pragma once

#include "entitylist.hpp"
#include "onb.hpp"
#include "vec3.hpp"
#include <memory>

#pragma region PDF ABSTRACT class declaration
class pdf
{
public:

    virtual ~pdf() = default;

    virtual float value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};
#pragma endregion // Probability Density Function (PDF) for importance sampling

#pragma region SPHERE PDF declaration
class sphere_pdf : public pdf
{
public:
    
    sphere_pdf() {}

    float value(const vec3& direction) const override { return 1 / (4 * pi); }
    vec3 generate() const override { return random_unit_vector(); }
};
#pragma endregion

#pragma region COSINE PDF declaration
class cosine_pdf : public pdf
{
    onb uvw;
public:

    cosine_pdf(const vec3& w) : uvw(w) {}

    float value(const vec3& direction) const override {
        auto cosine_theta{ dot(unit_vector(direction), uvw.w()) };
        return std::fmax(0.0f, cosine_theta / pi);
    }

    vec3 generate() const override {
        return uvw.transform(random_cosine_direction());
    }
};
#pragma endregion

#pragma region ENTITY PDF declaration
class entity_pdf : public pdf
{
    const entity& objects;
    point3 origin;

public:

    entity_pdf(const entity& objects_, const point3& origin) : objects(objects_), origin(origin) {}

    float value(const vec3& direction) const override { return objects.pdf_value(origin, direction); }
    vec3 generate() const override { return objects.random(origin); }
};
#pragma endregion

#pragma region MIXTURE PDF declaration
class mixture_pdf : public pdf
{
    std::array<std::shared_ptr<pdf>, 2> pdfs;
public:

    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {
        pdfs[0] = p0;
        pdfs[1] = p1;
    }

    float value(const vec3& direction) const override {
        return 0.5f * pdfs[0]->value(direction) + 0.5f * pdfs[1]->value(direction);
    }

    vec3 generate() const override {
        if (random_float() < 0.5f)
            return pdfs[0]->generate();
        else
            return pdfs[1]->generate();
    }
};
#pragma endregion