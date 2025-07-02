/**
 * @file Utils.cpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Contains helper functions used throughout the simulator, such as random number generation and time conversion.
 */

#include "../include/Utils.hpp"
#include "../include/Globals.hpp"

void initializeGenerator() {
    static random_device rd;
    static mt19937 gen(rd());
    generator = gen;
}

unsigned long convertTime(double value, string originUnit, string destinyUnit) {
    unordered_map<string, double> factos = {
        {"m", 60.0},
        {"s", 1.0},
        {"ms", 0.001},
        {"us", 0.000001},
        {"ns", 0.000000001}
    };

    if (factos.find(originUnit) == factos.end() || factos.find(destinyUnit) == factos.end()) {
        cerr << "Error: Unknown unit." << endl;
        return -1;
    }
    if (factos[originUnit] < factos[destinyUnit]) {
        cerr << "Error: This conversion cannot be performed. From " << originUnit << " to " << destinyUnit << "." << endl;
        return -1;
    }

    return static_cast<unsigned long>(value * factos[originUnit] / factos[destinyUnit]);
}

unsigned long randomInt(int min, int max) {
    uniform_int_distribution<unsigned long> distribution(min, max);
    return distribution(generator);
}

double randomDouble(double min, double max) {
    uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

void setColor(string color) {
    if (color == "red") cout << "\033[31m";
    else if (color == "green") cout << "\033[32m";
    else if (color == "yellow") cout << "\033[33m";
    else if (color == "blue") cout << "\033[34m";
    else if (color == "magenta") cout << "\033[35m";
    else if (color == "cyan") cout << "\033[36m";
    else if (color == "white") cout << "\033[37m";
    else cout << "\033[0m";
}

void setStyle(string style) {
    if (style == "bold") cout << "\033[1m";
    else if (style == "underline") cout << "\033[4m";
    else if (style == "blink") cout << "\033[5m";
    else if (style == "reverse") cout << "\033[7m";
    else cout << "\033[0m";
}