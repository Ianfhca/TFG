#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;
using SpikePair = pair<vector<vector<uint8_t>>, vector<vector<uint8_t>>>; // ON/OFF
using SpikeCubePolarity = vector<SpikePair>;


struct GestureAnnotation {
    int classLabel; // Between 1 & 11
    uint32_t startTime_usec;
    uint32_t endTime_usec;
};

struct DVSEvent {
    uint32_t data;
    uint32_t timestamp;

    uint16_t x() const { return (data >> 17) & 0x00001FFF; }
    uint16_t y() const { return (data >> 2) & 0x00001FFF; }
    bool polarity() const { return (data >> 1) & 0x00000001; }
};

struct AedatBlockHeader {
    uint16_t eventType;
    uint16_t eventSource;
    uint32_t eventSize;
    uint32_t eventTSOffset;
    uint32_t eventTSOverflow;
    uint32_t eventCapacity;
    uint32_t eventNumber;
    uint32_t eventValid;
};

void readAedat(const string& filename, vector<DVSEvent>& eventsOut);
void readAnnotationsCSV(const string& filename, vector<GestureAnnotation>& annotationsOut);

SpikeCubePolarity convertToSpikeCubePolarity(
    const vector<DVSEvent>& events,
    uint32_t startTime,
    uint32_t endTime,
    uint32_t dt,
    int width,
    int height
);