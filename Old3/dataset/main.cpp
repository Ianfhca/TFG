#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;
using SpikeCube = vector<vector<vector<uint8_t>>>;

struct GestureAnnotation {
    int classLabel;               // Entre 1 y 11
    uint32_t startTime_usec;
    uint32_t endTime_usec;
};

struct DVSEvent {
    uint32_t data;
    uint32_t timestamp;

    uint16_t x() const { return (data >> 17) & 0x1FFF; }
    uint16_t y() const { return (data >> 2) & 0x1FFF; }
    bool polarity() const { return (data >> 1) & 0x1; }
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

void readAedat(const string& filename, vector<DVSEvent>& eventsOut) {
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return;
    }

    // Saltar cabecera de texto (líneas que empiezan con '#')
    while (true) {
        char c;
        file.get(c);
        if (!file) break;

        if (c != '#') {
            file.unget(); // devolver el carácter si no es parte de la cabecera
            break;
        }

        // saltar hasta el fin de línea
        while (file.get(c) && c != '\n') {}
    }

    while (file) {
        AedatBlockHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if ((unsigned long int)file.gcount() < sizeof(header)) break;

        // Solo nos interesa el tipo 1 (polarity events)
        if (header.eventType != 1 || header.eventValid == 0) {
            file.seekg(header.eventNumber * 8, ios::cur); // saltar eventos
            continue;
        }

        for (uint32_t i = 0; i < header.eventValid; ++i) {
            DVSEvent evt;
            file.read(reinterpret_cast<char*>(&evt.data), sizeof(evt.data));
            file.read(reinterpret_cast<char*>(&evt.timestamp), sizeof(evt.timestamp));

            if (!file) break;
            eventsOut.push_back(evt);
        }
    }

    cout << "Se leyeron " << eventsOut.size() << " eventos.\n";
}

void readAnnotationsCSV(const string& filename, vector<GestureAnnotation>& annotationsOut) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo CSV " << filename << "\n";
        return;
    }

    string line;

    // ✅ Saltar el encabezado
    if (!getline(file, line)) {
        cerr << "Error: El archivo CSV está vacío.\n";
        return;
    }

    // Procesar el resto de líneas
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string item;

        GestureAnnotation ga;

        getline(ss, item, ',');
        if (item.empty() || !isdigit(item[0])) continue;  // Evita líneas no válidas
        ga.classLabel = stoi(item);

        getline(ss, item, ',');
        ga.startTime_usec = stoul(item);

        getline(ss, item, ',');
        ga.endTime_usec = stoul(item);

        annotationsOut.push_back(ga);
    }

    cout << "Se leyeron " << annotationsOut.size() << " anotaciones.\n";
}

SpikeCube convertToSpikeCube(
    const vector<DVSEvent>& events,
    uint32_t startTime,
    uint32_t endTime,
    uint32_t dt,           // duración de timestep en µs
    int width = 128,
    int height = 128
) {
    int timeSteps = (endTime - startTime) / dt + 1;

    SpikeCube cube(timeSteps, vector<vector<uint8_t>>(height, vector<uint8_t>(width, 0)));

    for (const auto& e : events) {
        if (e.timestamp < startTime || e.timestamp > endTime)
            continue;

        int tIndex = (e.timestamp - startTime) / dt;
        if (tIndex >= timeSteps || e.x() >= width || e.y() >= height)
            continue;

        cube[tIndex][e.y()][e.x()] = 1;
    }


    return cube;
}

int main() {
    string aedatFile = "DvsGesture/user10_fluorescent_led.aedat";
    string csvFile = "DvsGesture/user10_fluorescent_led_labels.csv";

    vector<DVSEvent> events;
    vector<GestureAnnotation> annotations;

    readAedat(aedatFile, events);
    readAnnotationsCSV(csvFile, annotations);

    // Elegir el primer gesto
    GestureAnnotation gesture = annotations[0];
    uint32_t dt = 1000; // 1ms

    SpikeCube spikeData = convertToSpikeCube(events, gesture.startTime_usec, gesture.endTime_usec, dt);

    cout << "Mapa de spikes generado con "
              << spikeData.size() << " pasos temporales.\n";

    return 0;
}
