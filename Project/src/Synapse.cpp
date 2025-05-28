#include "../include/Synapse.hpp"

// Synapse::Synapse(shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_)
//     : preNeuron(preNeuron_), delay(delay_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_), weight(0.5), cycles((delay / dt) + 1), preSynapticTrace(0.0), sumCycles(0), numSpikes(0) {
//     cout << "Synapse created with delay: " << delay << " and cycles: " << cycles << endl;
// }
 
Synapse::Synapse(shared_ptr<LIFneuron> preNeuron_, double lambdaX_, double alpha_, double weight_, int delay_, int dt_)
: preNeuron(preNeuron_), lambdaX(lambdaX_), alpha(alpha_), weight(weight_), delay(delay_), dt(dt_) {
    if (delay_ < 0) throw invalid_argument("delay must be non-negative");
    if (dt_ <= 0) throw invalid_argument("dt must be greater than 0");
    cycles = floor(delay / dt) + 1;
    spikesQ.resize(cycles);
    preSynapticTrace = 0.0;
    // sumCycles = 0;
    // numSpikes = 0;
}

// Synapse::Synapse(shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_) {
//     preNeuron = preNeuron_;
//     dt = dt_;
//     lambdaX = lambdaX_;
//     alpha = alpha_;
//     weight = 0.5;
//     delay = delay_; 
//     // cycles = (dt == delay) ? 1 : (dt / delay) + 1;
//     if (dt > 0) cycles = (delay / dt) + 1;
//     else throw invalid_argument("dt must be greater than 0");
//     cout << cycles << endl;
//     preSynapticTrace = 0.0;
//     sumCycles = 0;
//     numSpikes = 0;
//     spikes = Queue(cycles);
//     // Queue spikes(cycles);
//     cout << delay << endl;
// }

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
    return (preSynapticTrace - minPreX) / (maxPreX - minPreX);
    // double norm = (preSynapticTrace - minPreX) / (maxPreX - minPreX);
    // return max(0.0, min(1.0, norm)); 
}

double Synapse::getNormWeight(double minWeight, double maxWeight) {
    if (maxWeight == minWeight) return 0.0;
    // return (weight - minWeight) / (maxWeight - minWeight);
    double norm = (weight - minWeight) / (maxWeight - minWeight);
    return max(0.0, min(1.0, norm)); // Check this clipping [0, 1]
}

void Synapse::setWeight(double weight_) {
    // if (weight_ < 0 || weight_ > 1) {
    //     cerr << "Error: Weight must be in the range [0, 1]. Setting to 0.5." << endl;
    //     weight = 0.5; // Default value if out of range
    // } else {
        weight = weight_;
    // }
}

shared_ptr<LIFneuron> Synapse::getPreNeuron() {
    auto preNeuronShared = preNeuron.lock();
    if (preNeuronShared) {
        return preNeuronShared;
    } else {
        cerr << "Error: preNeuron is null or destroyed" << endl;
        return nullptr; // Return a null pointer
    }
}

void Synapse::updateWeight(double deltaWeight) {
    // cout << "Weight before: " << weight << "Weight after: " << weight+deltaWeight << endl;
    weight += deltaWeight;
    // if (weight > 1 || weight < -1) cout << "Weight exceed: " << weight << endl;
    // if (weight < 0) weight = 0; // Check this es >= 0?
    // if (weight > 1) weight = 1;
}

void Synapse::updateSpikeAtributes() {
    auto preNeuronShared = preNeuron.lock();

    if (preNeuronShared) {
        if (preNeuronShared->getSpike() == 1) {
            spikesQ.push_back(cycles);
            // preNeuronShared->setSpike(0); // Check this
        }
    } else {
        cerr << "Error: preNeuron is null or destroyed" << endl;
    }
}

int Synapse::obtainSpike() {
    int spike = 0;

    for (auto& value : spikesQ) {
        value -= 1;
    }

    if (!spikesQ.empty() && spikesQ.front() <= 0) {
        spikesQ.erase(spikesQ.begin());
        spike = 1;
        // cout << "Ha habido spike" << endl;
    }

    return spike;
}

void Synapse::updatePresinapticTrace(int spike) {
    // preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].obtainSpike(d))) * (dt / lambdaX);
    // int spike = obtainSpike();
    preSynapticTrace = (-preSynapticTrace * (1/lambdaX)) + (alpha * spike);
    if (preSynapticTrace < 0) preSynapticTrace = 0;
    // preSynapticTrace += (alpha * spike - preSynapticTrace) * (dt / lambdaX);
    // cout << "Spike: " << spike << " PreSynapticTrace: " << preSynapticTrace << endl;
}

double Synapse::updateForcingFunction(int spike) {
    // forcingFunction += neurons[j].obtainSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
    // int spike = obtainSpike();
    // cout << "Spike: " << spike << " pS:" << preSynapticTrace << endl;
    return (spike * weight - preSynapticTrace);
}