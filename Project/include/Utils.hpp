/**
 * @file Utils.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Contains utility functions and definitions for the simulator.
 */

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <limits> 
#include <iomanip>
#include <unordered_map>
#include <chrono>
#include <random>
#include <vector>
#include <stdexcept>
#include <random>

using namespace std;

static mt19937 generator;

/** @brief Initializes the random number generator with a seed based on the current time. */
void initializeGenerator();
/**
 * @brief Converts a time value from one unit to another.
 * @param value The time value to convert.
 * @param originUnit The original unit of the time value (m, s, ms, us, ns).
 * @param destinyUnit The unit to convert the time value to (m, s, ms, us, ns).
 * @return The converted time value in the desired unit.
 */
unsigned long convertTime(double value, string originUnit, string destinyUnit);
/**
 * @brief Converts a time value from one unit to another.
 * @param value The time value to convert.
 * @param originUnit The original unit of the time value (m, s, ms, us, ns).
 * @param destinyUnit The unit to convert the time value to (m, s, ms, us, ns).
 * @return The converted time value in the desired unit.
 */
unsigned long randomInt(int min, int max);
/**
 * @brief Generates a random double value within a specified range.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return A random double value between min and max.
 */
double randomDouble(double min, double max);
/**
 * @brief Sets the color for console output.
 * @param color The color to set (e.g., "red", "green", "blue").
 */
void setColor(string color);
/**
 * @brief Sets the style for console output.
 * @param style The style to set (e.g., "bold", "italic").
 */
void setStyle(string style);