#include "../include/Utils.hpp"
#include "../include/Globals.hpp"

void initializeGenerator() {
    static random_device rd;
    static mt19937 gen(rd());
    generator = gen;
}

double roundTo(double value, double precision) {
    return round(value / precision) * precision;
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

    // cout << factos[originUnit] << " -> " << factos[destinyUnit] << endl;

    return static_cast<unsigned long>(value * factos[originUnit] / factos[destinyUnit]);
}

// int randomNumber(int min, int max) {
//     // A motor to generate random numbers
//     mt19937 motor(chrono::steady_clock::now().time_since_epoch().count());

//     // Uniform distribution for integers in the range [min, max]
//     uniform_int_distribution<int> distribution(min, max);

//     return distribution(motor);
// }

unsigned long randomInt(int min, int max) {
    uniform_int_distribution<unsigned long> distribution(min, max);
    // cout << min << max << distribution(generator) << endl;
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

// int getStartPoint(int number, int max) {
//     // if (number > max) {
//     //     cerr << "Error: El número es mayor que el valor máximo permitido." << endl;
//     //     return -1; // Código de error
//     // }
//     int result = 0;
//     if (number < max) {
//         result = max - number;
//     } else {
//         max += 1;

//         int power = 1;
//         while (max / power > 0 && number % (power * 10) < max) {
//             power *= 10;
//         }
        
//         max -= 1;

//         result = max - (number % power);
//     }
//     return result;
// }


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