#include "scene_selection.hpp"

int main()
{
    std::clog << "\033[1;34mCPU Ray Tracer based on Ray Tracing book series\033[0m\n"
        << "\033[1;33m- \"Ray Tracing in one weekend\",\n"
        << "\033[1;33m- \"Ray Tracing the next week\"\033[0m\n"
        << "\033[1;33m- \"Ray Tracing the rest of your life\"\033[0m\n";

    auto res{scene_selection()};
    
    return res;
}
// If you take an intro to graphics course,
// there will be a lot of time spent on coordinate systems and 4×4 coordinate transformation matrices.
// Pay attention, it’s really important stuff!
// Chapter 11 tips on how to improve and expand the ray tracer,
// and also includes a scene file with a lot of objects in it.
// It’s a good test for the BVH implementation, and also a good example of how to use the ray tracer to create interesting scenes.
// We would also want a PDF for the block if we made it glass. But making a PDF for a block is quite a bit of work 
