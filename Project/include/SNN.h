#pragma once

#include "Layer.h"

#include <fstream>
#include <sstream>

using namespace std;

class SNN {
private:
    vector<Layer> layers;
public:
    SNN();
    SNN(char &file);
    int initNetwork(char &file);
    void linkLayers();
    void viewTopology();
    void trainNetwork();
    void testNetwork();
};