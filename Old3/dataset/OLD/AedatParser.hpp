#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct DVSEvent {
    uint32_t x, y, timestamp;
    bool polarity;
};

class AedatParser {
public:
    std::vector<DVSEvent> parse(const std::string& filename);
};
