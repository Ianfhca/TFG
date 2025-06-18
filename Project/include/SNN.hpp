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
    string topologyFile;
    string baseName;
    unsigned long time;
    unsigned long dt;
    string timeUnit;
    string dtUnit;
    string tRefrUnit;
    string delayUnit;
    int height;
    int width;
    int channels;
    int maxDelay;
    vector<NeuronParameters> neuronParams;
    // vector<Layer> layers;
    vector<shared_ptr<Layer>> layers;
    // unordered_map<string, deque<int>> inputSpikes;
    vector<deque<int>> inputSpikes;

    string mode;

    void parseParameters(const string &line);
    void parseHyperparameters(const string &line, NeuronParameters &neuron);
    void parseTopology(const string &line, TopologyParameters &topology);
    void parseInput(const string &line);
    vector<string> readTrialsList(const string& filename);
    string generateCSVFilename(const string& aedatFilename);
    int processGestureData(const SpikeCubePolarity& spikeData, int gestureClass, unsigned long& symTime, unsigned long symCap);
public:
    SNN();
    ~SNN();
    
    // int getTime();
    // int getDt();
    
    int initNetwork(char &filename, string &nMode);
    void linkLayers();
    void viewTopology();
    void viewInputSpikes();
    void trainNetwork();
    void saveWeights();
    int loadWeights(const int numFile);
    void testNetwork();
};