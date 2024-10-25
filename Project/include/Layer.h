#pragma once

#include "LIFneuron.h"

using namespace std;

class Layer {
private:
    string type;
    int numNeurons;
    string connections;
    int multisynaptic;
    vector<pair<int, int>> sparseConnections;

    vector<LIFneuron> neurons;
    vector<bool> spiked;
    vector< vector <vector<double>>> weights;
    vector< vector <vector<double>>> tau;
    vector< vector <vector<double>>> preSynapticTrace;

    // vector<double> membranePotentials;
    // vector<bool> spiked;
public:
    Layer();
    // Layer(string type_, int numNeurons_, string connections_);
    int initLayer(string type_, int numNeurons_, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_);
    string getType();
    int getNumNeurons();
    string getConnections();
    int getMultisynaptic();
    vector<pair<int, int>> getSparseConnections();
    vector<LIFneuron> getNeurons();
    void setPostSynapticLinks(Layer &postLayer);

    void initWeights(int numNeurons, int numPostNeurons, int m);
    void initDelays(int numNeurons, int numPostNeurons, int m, double minT, double maxT);
    void initPreSynapticTrace(int numNeurons, int numPostNeurons, int m);
    void updatePreSynapticTrace(Layer &preLayer, double t, double alpha);
    void propagateSpikes(Layer &postLayer, double t);
    void feedForward(vector<double> input);
};