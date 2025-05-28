#include "AedatParser.hpp"
#include <fstream>
#include <iostream>

std::vector<DVSEvent> AedatParser::parse(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<DVSEvent> events;

    if (!file.is_open()) {
        std::cerr << "Error opening AEDAT file: " << filename << std::endl;
        return events;
    }

    // Saltar encabezado textual (hasta "#!END-HEADER")
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("#!END-HEADER") != std::string::npos) break;
    }

    // Leer eventos binarios
    while (!file.eof()) {
        uint16_t eventType, eventSource;
        uint32_t eventSize, eventTSOffset, eventTSOverflow;
        uint32_t eventCapacity, eventNumber, eventValid;

        file.read(reinterpret_cast<char*>(&eventType), sizeof(eventType));
        file.read(reinterpret_cast<char*>(&eventSource), sizeof(eventSource));
        file.read(reinterpret_cast<char*>(&eventSize), sizeof(eventSize));
        file.read(reinterpret_cast<char*>(&eventTSOffset), sizeof(eventTSOffset));
        file.read(reinterpret_cast<char*>(&eventTSOverflow), sizeof(eventTSOverflow));
        file.read(reinterpret_cast<char*>(&eventCapacity), sizeof(eventCapacity));
        file.read(reinterpret_cast<char*>(&eventNumber), sizeof(eventNumber));
        file.read(reinterpret_cast<char*>(&eventValid), sizeof(eventValid));

        if (eventNumber == 0) break;

        for (uint32_t i = 0; i < eventNumber; ++i) {
            uint32_t data, timestamp;
            file.read(reinterpret_cast<char*>(&data), sizeof(data));
            file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));

            DVSEvent evt;
            evt.x = (data >> 17) & 0x1FFF;
            evt.y = (data >> 2) & 0x1FFF;
            evt.polarity = (data >> 1) & 0x1;
            evt.timestamp = timestamp;

            events.push_back(evt);
        }
    }

    return events;
}
