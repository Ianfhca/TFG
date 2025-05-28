#include "AedatParser.hpp"
#include "CsvParser.hpp"
#include <iostream>

int main() {
    AedatParser aedat;
    CsvParser csv;

    auto events = aedat.parse("DvsGesture/user10_fluorescent_led.aedat");
    auto gestures = csv.parse("DvsGesture/user10_fluorescent_led_labels.csv");

    for (const auto& gesture : gestures) {
        std::cout << "Gesture: " << gesture.gestureClass
                  << " [" << gesture.startTime << ", " << gesture.endTime << "]\n";

        for (const auto& evt : events) {
            if (evt.timestamp >= gesture.startTime && evt.timestamp <= gesture.endTime) {
                std::cout << "  Event x=" << evt.x << " y=" << evt.y
                          << " p=" << evt.polarity << " t=" << evt.timestamp << "\n";
            }
        }
    }

    return 0;
}
