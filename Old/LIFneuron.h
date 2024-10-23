#ifndef LIF_NEURON_H
#define LIF_NEURON_H

#include <iostream>
#include <vector>
#include <cstdlib>

// Constructor for LIF neuron
class LIFneuron {
private: 
    // Neuron parameters
    double vTh; // Threshold potential
    double vReset; // Reset potential
    double tauM; // Membrane time constant
    double gL; // Leak conductance
    double eL; // Leak reversal potential
    double vInit; // Initial potential
    double tRefr; // Refractory period

public:
    // LIFneuron(double threshold, double resting_potential, double reset_potential, double membrane_resistance, double membrane_capacitance, double membrane_potential, double time_step);
    LIFneuron(double vTh, double vReset, double tauM, double gL, double eL, double vInit, double tRefr);
    void updateMembranePotential(bool stop);
    //void plot_membrane_potential();
};

#endif // LIF_NEURON_H