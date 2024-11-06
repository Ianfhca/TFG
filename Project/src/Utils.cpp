#include "../include/Utils.h"

#include <cmath>

double roundTo(double value, double precision) {
    return round(value / precision) * precision;
}