#include "CsvParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<GestureWindow> CsvParser::parse(const std::string& filename) {
    std::vector<GestureWindow> windows;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening CSV file: " << filename << std::endl;
        return windows;
    }

    std::getline(file, line); // Saltar cabecera

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        GestureWindow window;

        std::getline(ss, token, ',');
        window.gestureClass = std::stoi(token);

        std::getline(ss, token, ',');
        window.startTime = std::stoul(token);

        std::getline(ss, token, ',');
        window.endTime = std::stoul(token);

        windows.push_back(window);
    }

    return windows;
}
