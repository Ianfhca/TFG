#pragma once
#include <string>
#include <vector>

using namespace std;

const int MIN_DELAY = 50;
const int MAX_DELAY = 150;

const int MAX_INTEGER = 9;
const int MAX_LONG = 16;

const string UNDEFINED = "Undefined";
const int NONE = -1;

//Hiperparameters

const int TIME = 10;
const string TIME_UNIT = "s";
const int DT = 100;
const string DT_UNIT = "ms";

const double V_RESET = -65.0;
const double V_REST = V_RESET;
const double V = V_REST;
const double V_TH = -50.0;
const double LAMBDA_V = 20.0;
const double T_REFR = 2.0;
const double LAMBDA_X = 20.0; // 10
const double ALPHA = 0.1;

struct NeuronParameters {
    double vReset, vRest, v, vTh, tauM, lambdaX, alpha, learningRate, aValue, convergenceTh;
    int tRefr;
    // double lambdaV;
};

struct TopologyParameters {
    string type;
    int height = 0;
    int width = 0;
    int channels = 0;
    int numNeurons = 0;
    int neuronType = 0;
    string connections;
    int r = 0;
    int k = 0;
    int multisynapses = 0;
    int delayMin = 0;
    int delayMax = 0;
    bool sparse = false;
    int stdpRule;
    int wtaRule;
    vector<pair<int, int>> sparseConnections;
    string mode;
};