#include "../include/Utils.h"
#include "../include/Globals.h"

double roundTo(double value, double precision) {
    return round(value / precision) * precision;
}

int convertTime(double value, string originUnit, string destinyUnit) {
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

    // cout << factos[originUnit] << " -> " << factos[destinyUnit] << endl;

    return static_cast<int>(value * factos[originUnit] / factos[destinyUnit]);
}

int randomNumber(int min, int max) {
    // A motor to generate random numbers
    std::mt19937 motor(std::chrono::steady_clock::now().time_since_epoch().count());

    // Uniform distribution for integers in the range [min, max]
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(motor);
}


// int convertToInt(double value) {
//     // Convert the number to a string with a fixed format
//     ostringstream oss;
//     oss << setprecision(MAX_INTEGER) << value;
//     string strValue = oss.str();
//     int decimals = 0;
//     int rest = 0;
//     int i = 0;
//     bool firstPart = true;

//     cout << strValue.size() << endl;

//     // Search for the position of the decimal point
//     size_t point = strValue.find('.');
//     if (point == string::npos) {
//         cerr << "Error: not a floating point number" << endl;
//         return -1;
//     }

//     i = point + 1;
//     while (i < strValue.size()) {
//         if (firstPart && strValue[i] == '0') decimals += 1;
//         else firstPart = false;

//         if (!firstPart && strValue[i] == '0') rest += 1;
//         else rest = 0;

//         i++;
//     }
//     cout << "Decimals: " << decimals << endl;

//     // decimals += strValue.size() - point - 1 - rest;
//     decimals += strValue.size() - point - 1;

//     cout << "Decimals: " << decimals << endl;

//     if (decimals > MAX_INTEGER) {
//         cerr << "Warning: too many decimals, rounded to " << MAX_INTEGER << " digits number." << endl;
//         decimals = MAX_INTEGER;
//     }

//     return static_cast<int>(value * pow(10, decimals));
// }

// long long convertToLong(double value) {
//     // Convert the number to a string with a fixed format
//     ostringstream oss;
//     oss << setprecision(MAX_LONG) << value;
//     string strValue = oss.str();
//     int decimals = 0;
//     int rest = 0;
//     int i = 0;
//     bool firstPart = true;

//     cout << strValue.size() << endl;

//     // Search for the position of the decimal point
//     size_t point = strValue.find('.');
//     if (point == string::npos) {
//         cerr << "Error: not a floating point number" << endl;
//         return -1;
//     }

//     i = point + 1;
//     while (i < strValue.size()) {
//         if (firstPart && strValue[i] == '0') decimals += 1;
//         else firstPart = false;

//         if (!firstPart && strValue[i] == '0') rest += 1;
//         else rest = 0;

//         i++;
//     }

//     decimals += strValue.size() - point - 1 - rest;

//     cout << "Decimals: " << decimals << endl;

//     if (decimals > MAX_LONG) {
//         cerr << "Warning: too many decimals, rounded to " << MAX_LONG << " digits number." << endl;
//         decimals = MAX_LONG;
//     }

//     return static_cast<long long>(value * pow(10, decimals));
// }

// long long convertTime(double value, string originUnit, string destinyUnit) {
//     double result = -1.0;
//     unordered_map<string, double> factos = {
//         {"m", 60.0},
//         {"s", 1.0},
//         {"ms", 0.001},
//         {"us", 0.000001},
//         {"ns", 0.000000001}
//     };

//     if (factos.find(originUnit) == factos.end() || factos.find(destinyUnit) == factos.end()) {
//         cerr << "Error: Unknown unit." << endl;
//         return result;
//     }
//     if (factos[originUnit] < factos[destinyUnit]) {
//         cerr << "Error: This conversion cannot be performed. From " << originUnit << " to " << destinyUnit << "." << endl;
//         return result;
//     }

//     cout << factos[originUnit] << " " << factos[destinyUnit] << endl;

//     result = static_cast<long long>(value * factos[originUnit] / factos[destinyUnit]);

//     return result;
// }