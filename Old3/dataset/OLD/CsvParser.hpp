#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct GestureWindow {
    int gestureClass;
    uint32_t startTime;
    uint32_t endTime;
};

class CsvParser {
public:
    std::vector<GestureWindow> parse(const std::string& filename);
};
