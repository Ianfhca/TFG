#pragma once

#include "LIFneuron.h"
#include "Globals.h"
#include "Utils.h"

#include <random>

using namespace std;

class Layer {
private:
    string type;
    int numNeurons;
    string connections;
    int multisynaptic;
    vector<pair<int, int>> sparseConnections;

    // Join this vectors in a single vector of pairs
    vector<LIFneuron> neurons;
    vector<vector<pair<int, double>>> spiked;
    vector< vector <vector<double>>> weights;
    vector< vector <vector<double>>> tau;
    vector< vector <vector<double>>> preSynapticTrace;

public:
    Layer(string type_ = "Undefined", int numNeurons_ = -1, string connections_ = "Undefined", int multisynaptic_ = -1, vector<pair<int, int>> sparseConnections_ = {{-1, -1}});
    string getType();
    int getNumNeurons();
    string getConnections();
    int getMultisynaptic();
    vector<pair<int, int>> getSparseConnections();
    vector<LIFneuron> getNeurons();
    void setPostSynapticLinks(Layer &postLayer);

    void initWeights(int numPostNeurons);
    void initSpikes();
    void initDelays(int numPostNeurons, double minT, double maxT);
    void initPreSynapticTrace(int numPostNeurons);
    void updatePreSynapticTrace(Layer &preLayer, double t, double alpha);
    void propagateSpikes(Layer &postLayer, double t);
    void feedForward(vector<double> input);
};