/**
 * @file Synapse.cpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Implements the Synapse class logic, including trace updates, weight adjustments, and spike delay simulation.
 */

#include "../include/Synapse.hpp"
 
Synapse::Synapse(shared_ptr<LIFneuron> preNeuron_, double lambdaX_, double alpha_, double weight_, int delay_, int dt_)
: preNeuron(preNeuron_), lambdaX(lambdaX_), alpha(alpha_), weight(weight_), delay(delay_), dt(dt_) {
    if (delay_ < 0) throw invalid_argument("delay must be non-negative");
    if (dt_ <= 0) throw invalid_argument("dt must be greater than 0");
    cycles = floor(delay / dt);
    // spikesQ.resize(cycles);
    preSynapticTrace = 0.0;
}

Synapse::~Synapse() {
    // cout << "Destroying Synapse" << endl;
}

double Synapse::getWeight() {
    return weight;
}

int Synapse::getDelay() {
    return delay;
}

double Synapse::getPreSynapticTrace() {
    return preSynapticTrace;
}

double Synapse::getNormPreSynapticTrace(double minPreX, double maxPreX) {
    if (maxPreX == minPreX) return 0.0;
    double norm = (preSynapticTrace - minPreX) / (maxPreX - minPreX);

    return max(0.0, min(1.0, norm)); // Clipping [0, 1]
}

double Synapse::getNormWeight(double minWeight, double maxWeight) {
    if (maxWeight == minWeight) return 0.0;
    double norm = (weight - minWeight) / (maxWeight - minWeight);

    return max(0.0, min(1.0, norm)); // Clipping [0, 1]
}

shared_ptr<LIFneuron> Synapse::getPreNeuron() {
    return preNeuron;
}

void Synapse::setWeight(double weight_) {
    weight = weight_;
}

int Synapse::obtainPreviousSpike() {
    int spike = 0;

    if (preNeuron->getSpike() == 1) spikesQ.push_back(cycles);

    if (!spikesQ.empty() && spikesQ.front() <= 0) {
        spikesQ.erase(spikesQ.begin());
        spike = 1;
    }

    for (unsigned long i = 0; i < spikesQ.size(); i++) {
        spikesQ[i] -= 1;
    }

    return spike;
}

void Synapse::updatePresinapticTrace(int spike) {
    double decay  = (1/lambdaX);

    preSynapticTrace = decay * (-preSynapticTrace) + (alpha * spike);

    if (preSynapticTrace < 0) preSynapticTrace = 0;
}

double Synapse::updateForcingFunction(int spike) {
    return (spike * weight - preSynapticTrace);
}

void Synapse::updateWeight(double deltaWeight) {
    weight += deltaWeight;
    if (weight < 0) weight = 0;
    if (weight > 1) weight = 1;
}