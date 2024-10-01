#ifndef NEURON_H
#define NEURON_H

#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

class LIF_neuron {
private: 
    // Neuron parameters
    double V_reset; // Reset potential
    double V_th; // Threshold potential
    double Tau_m; // Membrane time constant
    double g_L; // Leak conductance
    double E_L; // Leak reversal potential
    double V_init; // Initial potential
    double T_refr; // Refractory period

    //Symulation parameters
    double T = 400; // Total time (ms)
    double dt = 0.1; // Time step (ms)

    // (0, 400, 0.1) -> (0, 4000, 1)
    double* time_points = (double*) malloc(T / dt * sizeof(double)); // Discretized time points
    int Lt = T / dt; // Number of time points

    double* V = (double*) malloc(Lt * sizeof(double)); // Membrane potential
    double* Icrnt = (double*) malloc(Lt * sizeof(double)); // Input current

public:
    // Constructor
    LIF_neuron(double V_reset, double V_th, double Tau_m, double g_L, double E_L, double V_init, double T_refr) {
        this->V_reset = V_reset;
        this->V_th = V_th;
        this->Tau_m = Tau_m;
        this->g_L = g_L;
        this->E_L = E_L;
        this->V_init = V_init;
        this->T_refr = T_refr;

        // Discretize time points
        for (int i = 0; i < T / dt; i++) {
            time_points[i] = i * dt;
            V[i] = 0;
            Icrnt[i] = 100;
        }
        V[0] = V_init;

    }

    // Method to update membrane potential
    void update_membrane_potential(bool stop = false) {
        double dv;
        vector<double> rec_spike;
        double tr = 0;

        if (stop) {
            for (int i = 0; i < (Lt / 2 - 1000); i++) {
                Icrnt[i] = 0;
            }
            for (int i = (Lt / 2 + 1000); i < Lt; i++) {
                Icrnt[i] = 0;
            }
        }

        // Loop through time points
        for (int i = 0; i < Lt; i++) {

            // Refractory period
            if (tr > 0) {
                tr -= 1; //dt;
                V[i] = V_reset;
            } else if (V[i] >= V_th) {
                rec_spike.push_back(i * dt);
                V[i] = V_reset;
                tr = T_refr / dt;
            }

            // Calculate change in membrane potential
            dv = (-(V[i] - E_L) + Icrnt[i] / g_L) * (dt / Tau_m);

            // Update membrane potential
            V[i + 1] = V[i] + dv;

            // Print membrane potential
            cout << V[i] << endl;
        }
    }

};

#endif // NEURON_H