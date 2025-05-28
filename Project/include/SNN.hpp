#pragma once

#include "Layer.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Dataset.hpp"

#include <fstream>
#include <sstream>

#include <functional>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <memory>

using namespace std;

class SNN {
private:
    int time;
    int dt;
    string timeUnit;
    string dtUnit;
    int height;
    int width;
    int channels;
    int maxDelay;
    vector<NeuronParameters> neuronParams;
    // vector<Layer> layers;
    vector<shared_ptr<Layer>> layers;
    // unordered_map<string, deque<int>> inputSpikes;
    vector<deque<int>> inputSpikes;


    void parseParameters(const string &line);
    void parseHyperparameters(const string &line, NeuronParameters &neuron);
    void parseTopology(const string &line, TopologyParameters &topology);
    void parseInput(const string &line);
public:
    SNN();
    ~SNN();
    
    // int getTime();
    // int getDt();
    
    int initNetwork(char &file);
    void linkLayers();
    void viewTopology();
    void viewInputSpikes();
    void trainNetwork();
    void saveWeights();
    int loadWeights(const int numFile);
    void testNetwork();
};