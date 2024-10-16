#include "../include/LIFneuron.h"

LIFneuron::LIFneuron(double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_)
    : v(vRest_), vTh(vTh_), vRest(vRest_), vReset(vReset_), lambdaV(lambdaV_), tRefr(tRefr_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_) {}

double LIFneuron::getMembranePotential() {
    return v;
}

void LIFneuron::updateMembranePotential(double inputCurrent, double time) {
    if (inRefraction) {
        if (time - timeLastSpike >= tRefr) {
            inRefraction = false;
        } else {
            return;
        }
    }

    v += dt * (- (v - vRest) / lambdaV + inputCurrent); // Check this
    // v += (- (v - vRest) / lambdaV + inputCurrent) * (dt / lambda_v);

    if (v >= vTh) {
        fire(time);
        sHist.push_back(time); // Check this
    }
}

void LIFneuron::fire(double time) {
    v = vReset;
    inRefraction = true;
    timeLastSpike = time;
    std::cout << "Neuron fired at time " << time << " ms" << std::endl;
}

void LIFneuron::updateTrace(double sPre) {
    for (size_t i = 0; i < X.size(); i++) {
        X[i] += dt * (-X[i] / lambdaX + alpha * sPre); // Check this
        // X[i] += dt * (-X[i] + alpha * sPre) * (dt / lambdaX);
    }
}