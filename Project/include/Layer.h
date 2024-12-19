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
    int multisynapses;
    vector<pair<int, int>> sparseConnections;

    // Join this vectors in a single vector of pairs
    vector<LIFneuron> neurons;
    // vector< vector <vector<double>>> weights;
    // vector< vector <vector<double>>> tau; // Delays
    // vector< vector <vector<double>>> preSynapticTrace;

public:
    Layer(string type_ = "undefined", int numNeurons_ = -1, vector<pair<vector<double>, int>> neuronParams_ = {{{}, -1}}, string connections_ = "undefined", int multisynapses_ = -1, pair<int, int> delayRange_ = {-1, -1}, vector<pair<int, int>> sparseConnections_ = {{-1, -1}});
    string getType();
    int getNumNeurons();
    string getConnections();
    int getMultisynapses();
    vector<pair<int, int>> getSparseConnections();
    vector<LIFneuron> getNeurons();
    // void setPostsynapticLinks(Layer &postLayer);
    void setPresynapticLinks(Layer &preLayer);

    void feedForward(Layer &postLayer, double t);

    // void initWeights(int numPostNeurons);
    // void initDelays(int numPostNeurons, double minT, double maxT);
    // void initPresynapticTrace(int numPostNeurons);
    // void updatePreSynapticTrace(Layer &preLayer, double t, double alpha);
    // void propagateSpikes(Layer &postLayer, double t);
    // void feedForward(vector<double> input);
};