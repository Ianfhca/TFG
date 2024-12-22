#pragma once

#include "Layer.h"

#include <fstream>
#include <sstream>

using namespace std;

class SNN {
private:
    int time;
    int dt;
    int maxDelay;
    vector<Layer> layers;
public:
    SNN();
    int getTime();
    int getDt();
    
    int initNetwork(char &file);
    void linkLayers();
    void viewTopology();
    void trainNetwork();
    void testNetwork();
};