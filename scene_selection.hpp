#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>

#include "scenes.hpp"

struct scene_option {
    int id;
    std::string name;
    std::string partial;
};

const std::vector<scene_option> scenes {
    {1, "Bouncing Spheres", "bouncing"}
    , {2, "Two Spheres", "two"}
    , {3, "Planet Earth", "earth"}
    , {4, "Perlin Noise spheres", "noise"}
    , {5, "Quads", "squares"}
    , {6, "Simple lights", "lights"}
    , {7, "Cornell box", "box"}
};

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool is_number(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

std::vector<scene_option> find_scenes_by_partial_name(const std::string& partial_name) {
    std::vector<scene_option> matches;
    std::string lower_query{ to_lower(partial_name) };

    for (const auto& scene : scenes) {
        if (to_lower(scene.name) == lower_query || to_lower(scene.partial) == lower_query) {
            matches.push_back(scene);
            return matches;
        }
    }

    try {
        std::regex pattern(lower_query, std::regex_constants::icase);
        for (const auto& scene : scenes) {
            if (std::regex_search(scene.name, pattern) || std::regex_search(scene.partial, pattern)) {
                matches.push_back(scene);
            }
        }
    } catch (const std::regex_error& e) {
        std::cerr << "\033[1;31mRegex error: " << e.what() << "\033[0m" << std::endl;
        
        for (const auto& scene : scenes) {
            if (scene.name.find(lower_query) != std::string::npos || scene.partial.find(lower_query) != std::string::npos) {
                matches.push_back(scene);
            }
        }
    }

    return matches;
}

int get_scene_choice() {
    while (true) {

        std::clog << "\033[1;36mScene selection\033[0m\n";
        for (const auto& scene : scenes) {
            std::cout << scene.id << ". " << scene.name << "\n";
        }
        std::clog << "0. Exit\n";
        std::clog << "Enter scene number: ";

        std::string input;
        std::getline(std::cin, input);

        if (to_lower(input) == "exit" || input == "0") {
            return 0;
        }

        if (is_number(input)) {
            int choice = std::stoi(input);
            if (choice >= 0 && choice <= static_cast<int>(scenes.size())) {
                return choice;
            } else {
                std::cout << "\033[1;31mInvalid scene number. Please enter a number between 0 and " 
                          << scenes.size() << ".\033[0m\n";
            }
        } 
        else 
        {
            auto matches = find_scenes_by_partial_name(input);
            
            if (matches.empty()) {
                std::cout << "\033[1;31mNo matching scenes found. Please try again.\033[0m\n";
            } 
            else if (matches.size() == 1) {
                
                std::cout << "\033[1;32mSelected: " << matches[0].partial << "\033[0m\n";
                return matches[0].id;
            } 
            else {
                std::cout << "Multiple matches found. Please select one:\n";
                for (const auto& match : matches) {
                    std::cout << match.id << ". " << match.partial << "\n";
                }
                std::cout << "Enter number: ";
                
                std::string choice_input;
                std::getline(std::cin, choice_input);
                
                if (is_number(choice_input)) {
                    int choice = std::stoi(choice_input);
                    for (const auto& match : matches) {
                        if (match.id == choice) {
                            return choice;
                        }
                    }
                }
                std::cout << "\033[1;31mInvalid selection. Please try again.\033[0m\n";
            }
        }
    }
}

auto render_scene(int scene_id) -> int {

    if (scene_id == 0) {
        return 0;
    }
    
    
    for (const auto& scene : scenes) {
        if (scene.id == scene_id) {
            std::cout << "Rendering scene: " << scene.name << "\n";
            
            // Call the appropriate scene function based on ID
            switch (scene_id) {
                case 1:
                    return bouncing_spheres();
                case 2:
                    return two_spheres_scene();
                case 3:
                    return earth();
                case 4:
                    return perlin_spheres();
                case 5:
                    return quads();
                case 6:
                    return simple_light();
                case 7: 
                    return cornell_box();
                    
            }
            
        }
    }
    
    return -1;
}

auto scene_selection() -> int {
    bool running{ true };

    std::cin.ignore(0, '\n');

    int res{};
    while (running) {
        int scene_choice = get_scene_choice();
        
        if (scene_choice == 0) {
            std::cout << "Exiting program. Goodbye!\n";
            running = false;
        } else {
            res = render_scene(scene_choice);
        }
    }

    return res;
}