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
    vector< vector <vector<double>>> weights;

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
    void feedForward(vector<double> input);
};