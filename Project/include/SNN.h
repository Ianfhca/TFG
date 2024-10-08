#ifndef SNN_H
#define SNN_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct LIFparams {
    double vTh; // Threshold potential
    double vReset; // Reset potential
    double tauM; // Membrane time constant
    double gL; // Leak conductance
    double eL; // Leak reversal potential
    double vInit; // Initial potential
    double tRefr; // Refractory period
};

struct Layer {
    string type;
    int neurons;
    string connections;
    LIFparams lif_params;
    vector<pair<int, int>> custom_connections;
};

struct SNN {
    vector<Layer> layers;
};

int readTopology(char *file, SNN *snn);
void viewTopology(SNN *snn);

#endif // SNN_H