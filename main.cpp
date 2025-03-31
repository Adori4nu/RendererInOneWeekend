#include "scene_selection.hpp"

int main()
{
    std::clog << "\033[1;34mCPU Ray Tracer based on Ray Tracing book series\033[0m\n"
        << "\033[1;33m- \"Ray Tracing in one weekend\",\n"
        << "- \"Ray Tracing the next week\"\033[0m\n";

    auto res{scene_selection()};
    
    return res;
}