#include <cfloat>
#include <fstream>
#include <iostream>
#include "entitylist.hpp"
#include "sphere.hpp"

vec3 color(const ray& r, entity* world) {
    hit_record rec;
    if (world->hit(r, 0.f, FLT_MAX, rec)) {
        return 0.5f * vec3{ rec.normal.x() + 1.f, rec.normal.y() + 1.f, rec.normal.z() + 1.f };
    }
    else {
        vec3 unit_direction{ unit_vector(r.direction()) };
        float t{ 0.5f * ( unit_direction.y() + 1.0f ) };
        return ( 1.0f - t ) * vec3{ 1.0f, 1.0f, 1.0f } + t * vec3{ 0.5f, 0.7f, 1.0f };
    }
}

int main()
{
    int nx{200};
    int ny{100};

    vec3 lower_left_corner{ -2.0f, -1.0f, -1.0f };
    vec3 horizontal{ 4.0f, 0.0f, 0.0f };
    vec3 vertical{ 0.0f, 2.0f, 0.0f };
    vec3 origin{ 0.0f, 0.0f, 0.0f };

    entity *list[2];
    list[0] = new sphere{ vec3{ 0.f, 0.f, -1.f }, 0.5f };
    list[1] = new sphere{ vec3{ 0.f, -100.5f, -1.f }, 100.f };
    entity *world{ new entity_list{ list, 2 } };
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
            for (int i{ 0 }; i < nx; i++)
            {
                float u{ float(i) / float(nx) };
                float v{ float(j) / float(ny) };
                ray r{ origin, lower_left_corner + u * horizontal + v * vertical };
                
                vec3 p{ r.point_at_parameter(2.f) };
                vec3 col{ color(r, world) };
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