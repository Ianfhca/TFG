#include "../include/LIFneuron.h"
using namespace std;

// #include "./Pbplots/pbPlots.hpp"
// #include "./Pbplots/supportLib.hpp"

//Symulation parameters
double tt = 400; // Total time (ms)
double dt = 0.1; // Time step (ms)

// (0, 400, 0.1) -> (0, 4000, 1)
double* timePoints = (double*) malloc(tt / dt * sizeof(double)); // Discretized time points
int lt = tt / dt; // Number of time points

double* v = (double*) malloc(lt * sizeof(double)); // Membrane potential
double* inputC = (double*) malloc(lt * sizeof(double)); // Input current

// Constructor
LIFneuron::LIFneuron(double vTh, double vReset, double tauM, double gL, double eL, double vInit, double tRefr) {
    this->vTh = vTh;
    this->vReset = vReset;
    this->tauM = tauM;
    this->gL = gL;
    this->eL = eL;
    this->vInit = vInit;
    this->tRefr = tRefr;

    // Discretize time points
    for (int i = 0; i < tt / dt; i++) {
        timePoints[i] = i * dt;
        v[i] = 0;
        inputC[i] = 100;
    }
    v[0] = vInit;
}

// Method to update membrane potential
void LIFneuron::updateMembranePotential(bool stop = false) {
    double dv;
    vector<double> recSpike;
    double tr = 0;

    if (stop) {
        for (int i = 0; i < (lt / 2 - 1000); i++) {
            inputC[i] = 0;
        }
        for (int i = (lt / 2 + 1000); i < lt; i++) {
            inputC[i] = 0;
        }
    }

    // Loop through time points
    for (int i = 0; i < lt; i++) {
        // Refractory period
        if (tr > 0) {
            tr -= 1;
            v[i] = vReset;  
        } else if (v[i] >= vTh) {
            recSpike.push_back(i * dt);
            v[i] = vReset;
            tr = tRefr / dt;
        }

        // Calculate change in membrane potential
        dv = (-(v[i] - eL) + inputC[i] / gL) * (dt / tauM);

        // Update membrane potential
        v[i + 1] = v[i] + dv; 

        // Print membrane potential
        cout << v[i] << endl;
    }

    // Print spike times
    // for (int i = 0; i < recSpike.size(); i++) {
    //     cout << recSpike[i] << endl;
    // }
    
    free(timePoints);
    free(v);
    free(inputC);
}

// void plot_membrane_potential() {
//     bool success;
//     StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
//     RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

//     vector<double> xs{-2, -1, 0, 1, 2};
//     vector<double> ys{2, -1, -2, -1, 2};

//     success = DrawScatterPlot(imageReference, 600, 400, &xs, &ys, errorMessage);

//     if(success){
//         vector<double> *pngdata = ConvertToPNG(imageReference->image);
//         WriteToFile(pngdata, "./plots/plot.png");
//         DeleteImage(imageReference->image);
//     }else{
//         cerr << "Error: ";
//         for(wchar_t c : *errorMessage->string){
//             wcerr << c;
//         }
//         cerr << endl;
//     }

//     FreeAllocations();
// }