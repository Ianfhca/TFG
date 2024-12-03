#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <limits> 
#include <iomanip>
#include <unordered_map>

using namespace std;

double roundTo(double value, double precision);
int convertToInt(double valor);
long long convertToLong(double valor);
long long convertirTiempo(double valor, string unidadOrigen, string unidadDestino);