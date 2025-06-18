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
// #include <opencv2/opencv.hpp>
// #include <opencv4/opencv2/core.hpp>

using namespace std;

static mt19937 generator;

void initializeGenerator();
double roundTo(double value, double precision);
unsigned long convertTime(double value, string originUnit, string destinyUnit);
// int randomNumber(int min, int max);
unsigned long randomInt(int min, int max);
double randomDouble(double min, double max);
// int getStartPoint(int number, int max);
void setColor(string color);
void setStyle(string style);
// int convertToInt(double valor);
// long long convertToLong(double valor);
// long long convertTime(double value, string originUnit, string destinyUnit);
// void visualizeSpikes(int t);