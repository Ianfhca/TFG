#pragma once

#include "Layer.h"

#include <fstream>
#include <sstream>

using namespace std;

class SNN {
private:
    double time;
    double dt;
    vector<Layer> layers;
public:
    SNN();
    SNN(char &file);
    int initNetwork(char &file);
    void linkLayers();
    void viewTopology();
    void trainNetwork(double t, double dt);
    void testNetwork();
};