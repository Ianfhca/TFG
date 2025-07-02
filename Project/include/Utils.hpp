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

void initializeGenerator();
double roundTo(double value, double precision);
unsigned long convertTime(double value, string originUnit, string destinyUnit);
unsigned long randomInt(int min, int max);
double randomDouble(double min, double max);
void setColor(string color);
void setStyle(string style);