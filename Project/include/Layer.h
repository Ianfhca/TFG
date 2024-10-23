#pragma once

#include "LIFneuron.h"

using namespace std;

class Layer {
private:
    string type;
    int numNeurons;
    string connections;
    // LIFparams lifParams;
    vector<pair<int, int>> sparseConnections;

    vector<LIFneuron> neurons;
    // vector<double> membranePotentials;
    // vector<bool> spiked;
public:
    Layer();
    Layer(string type_, int numNeurons_, string connections_);
    int initLayer(string type_, int numNeurons_, string connections_, vector<pair<int, int>> sparseConnections_);
    string getType();
    int getNumNeurons();
    string getConnections();
    vector<pair<int, int>> getSparseConnections();
    vector<LIFneuron> getNeurons();
};