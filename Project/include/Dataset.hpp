/**
 * @file Dataset.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Contains definitions and functions for handling DVS dataset events and gesture annotations.
 */

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

/**
 * @struct GestureAnnotation
 * @brief Represents an annotation for a gesture in the dataset.
 * 
 * Contains the class label and the start and end times of the gesture in microseconds.
 */
struct GestureAnnotation {
    int classLabel; // Between 1 & 11
    uint32_t startTime_usec;
    uint32_t endTime_usec;
};

/**
 * @struct DVSEvent
 * @brief Represents an event in the DVS dataset.
 * 
 * Contains the data and timestamp of the event, along with methods to extract x, y coordinates and polarity.
 */
struct DVSEvent {
    uint32_t data;
    uint32_t timestamp;

    uint16_t x() const { return (data >> 17) & 0x00001FFF; }
    uint16_t y() const { return (data >> 2) & 0x00001FFF; }
    bool polarity() const { return (data >> 1) & 0x00000001; }
};

/**
 * @struct AedatBlockHeader
 * @brief Represents the header of a block in an Aedat file.
 * 
 * Contains metadata about the events in the block, such as type, source, size, timestamps, capacity, number of events, and validity.
 */
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

/**
 * @brief Reads DVS events from an Aedat file and stores them in a vector.
 * 
 * @param filename The name of the Aedat file to read.
 * @param eventsOut The vector to store the read DVS events.
 */
void readAedat(const string& filename, vector<DVSEvent>& eventsOut);
/**
 * @brief Reads gesture annotations from a CSV file and stores them in a vector.
 * 
 * The CSV file should contain three columns: class label, start time in microseconds, and end time in microseconds.
 * 
 * @param filename The name of the CSV file to read.
 * @param annotationsOut The vector to store the read gesture annotations.
 */
void readAnnotationsCSV(const string& filename, vector<GestureAnnotation>& annotationsOut);

/**
 * @brief Converts a vector of DVS events into a SpikeCubePolarity representation.
 * 
 * @param events The input vector of DVS events.
 * @param startTime The start time for the conversion.
 * @param endTime The end time for the conversion.
 * @param dt The time step for the conversion.
 * @param width The width of the output spike cube.
 * @param height The height of the output spike cube.
 * @return A SpikeCubePolarity object containing the converted data.
 */
SpikeCubePolarity convertToSpikeCubePolarity(
    const vector<DVSEvent>& events,
    uint32_t startTime,
    uint32_t endTime,
    uint32_t dt,
    int width,
    int height
);