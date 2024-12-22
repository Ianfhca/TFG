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

using namespace std;

double roundTo(double value, double precision);
int convertTime(double value, string originUnit, string destinyUnit);
int randomNumber(int min, int max);
// int convertToInt(double valor);
// long long convertToLong(double valor);
// long long convertTime(double value, string originUnit, string destinyUnit);