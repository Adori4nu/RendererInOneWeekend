#include "scene_selection.hpp"

int main()
{
    std::clog << "\033[1;34mCPU Ray Tracer based on Ray Tracing book series\033[0m\n"
        << "\033[1;33m- \"Ray Tracing in one weekend\",\n"
        << "- \"Ray Tracing the next week\"\033[0m\n"
        << "- \"Ray Tracing the rest of your life\"\033[0m\n";

    auto res{scene_selection()};
    
    return res;
} // 10.3
// If you take an intro to graphics course,
// there will be a lot of time spent on coordinate systems and 4×4 coordinate transformation matrices.
// Pay attention, it’s really important stuff!