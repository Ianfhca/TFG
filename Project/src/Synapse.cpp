#include "../include/Synapse.hpp"

// Synapse::Synapse(std::shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_)
//     : preNeuron(preNeuron_), delay(delay_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_), weight(0.5), cycles((delay / dt) + 1), preSynapticTrace(0.0), sumCycles(0), numSpikes(0) {
//     std::cout << "Synapse created with delay: " << delay << " and cycles: " << cycles << std::endl;
// }

Synapse::Synapse(shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_)
: preNeuron(preNeuron_), delay(delay_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_), winit(0.5), weight(0.5), cycles(0) {
    if (dt_ <= 0) throw std::invalid_argument("dt must be greater than 0");
    if (delay_ < 0) throw std::invalid_argument("delay must be non-negative");
    cycles = (delay / dt) + 1;
    // spikesQ = Queue(cycles);
    // spikesQ.reserve(cycles);
    spikesQ.resize(cycles);
    preSynapticTrace = 0.0;
    sumCycles = 0;
    numSpikes = 0;
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
//     else throw std::invalid_argument("dt must be greater than 0");
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

double Synapse::getWinit() {
    return winit;
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
    // (value - min) / (max - min);
    // print presinaptic trace min and max
    
    // double maxTrace = (alpha * lambdaX) / (lambdaX + 1);
    // return preSynapticTrace / maxTrace;

    // cout << "PreSynapticTrace: " << preSynapticTrace << endl;
    // cout << "MinPreX: " << minPreX << endl;
    // cout << "MaxPreX: " << maxPreX << endl;
    if (maxPreX == minPreX) {
        std::cerr << "Error: maxPreX and minPreX are equal, cannot normalize." << std::endl;
        return 0.0;
    }
    return (preSynapticTrace - minPreX) / (maxPreX - minPreX);
}

double Synapse::getNormWeight(double minWeight, double maxWeight) {
    // value - min) / (max - min);
    // return weight / 1;
    // double maxWeight = 1.0; // Assuming weight is normalized between 0 and 1
    // return weight / maxWeight;
    return (weight - minWeight) / (maxWeight - minWeight);
}

void Synapse::setWeight(double deltaWeight) {
    weight += deltaWeight;
    if (weight < 0) weight = 0; // Check this es >= 0?
    // if (weight > 1) weight = 1;
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

void Synapse::updateSpikeAtributes() {
    auto preNeuronShared = preNeuron.lock();

    if (preNeuronShared) {
        if (preNeuronShared->getSpike() == 1) spikesQ.push_back(cycles);
    } else {
        std::cerr << "Error: preNeuron is null or destroyed" << std::endl;
    }
}

void Synapse::updatePresinapticTrace(int spike) {
    // preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].obtainSpike(d))) * (dt / lambdaX);
    // int spike = obtainSpike();
    preSynapticTrace = (-preSynapticTrace * (1/lambdaX)) + (alpha * spike);
    if (preSynapticTrace < 0) preSynapticTrace = 0; // Check this tiene que ser >= 0
    // cout << "Spike: " << spike << " PreSynapticTrace: " << preSynapticTrace << endl;
}

double Synapse::updateForcingFunction(int spike) {
    // forcingFunction += neurons[j].obtainSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
    // int spike = obtainSpike();
    // cout << "Spike: " << spike << " pS:" << preSynapticTrace << endl;
    return (spike * weight - preSynapticTrace);
    // return (obtainSpike() * weight - preSynapticTrace);
}

double Synapse::update() {
    int spike = 0;
    double forcingFunction = 0;

    updateSpikeAtributes();
    spike = obtainSpike();
    updatePresinapticTrace(spike);
    forcingFunction = updateForcingFunction(spike);
    
    return forcingFunction;
}

// int Synapse::obtainSpike() {
//     int spike = 0;

//     if (numSpikes > 0) {
//         if (numSpikes == 1) spike = (sumCycles == numSpikes) ? 1 : 0;
//         else spike = (sumCycles % numSpikes == 0) ? 1 : 0;
        
//         sumCycles -= 1;
//         numSpikes -= spike;
//     }
//     cout << "Spike: " << spike << endl;

//     return spike;
// }

// void Synapse::updateSpikeAtributes() {
//     // cout << "PreNeuron: " << preNeuron->getSpike() << endl;
//     if (preNeuron != nullptr) {
//         if (preNeuron->getSpike() == 1) {
//             sumCycles += cycles;
//             numSpikes += 1;
//             cout << "Cycles: " << cycles << " NumSpikes: " << numSpikes << endl;
//         }
//     } else {
//         std::cerr << "Error: preNeuron is null" << std::endl;
//     }
// }

// int Synapse::obtainSpike() {
//     int spike = 0;

//     if (numSpikes < 1) {
//         updateSpikeAtributes();
//         if (numSpikes == 1) spike = (sumCycles == 1) ? 1 : 0;
//         if (sumCycles > 0) sumCycles--;
//         cout << "SOY 1" << endl;
//     } else if (numSpikes == 1) {
//         if (sumCycles > 0) sumCycles--;
//         if (numSpikes == 1) spike = (sumCycles == 1) ? 1 : 0;
//         updateSpikeAtributes();
//         if (numSpikes > 1) if (sumCycles % numSpikes == 0) spike =1;
//         cout << "SOY 2" << endl;
//     } else {
//         if (sumCycles > 0) sumCycles--;
//         if (sumCycles % numSpikes == 0) spike =1;
//         updateSpikeAtributes();
//         cout << "SOY 3" << endl;
//     }

//     if (spike == 1 && numSpikes > 0) numSpikes--;

//     cout << "sumCycles: " << sumCycles << " NumSpikes: " << numSpikes << "\n" << endl;

//     cout << "Spike: " << spike << endl;

//     return spike;
// }

// bool Synapse::updateSpikeAtributes() {
//     bool spike = false;
//     auto preNeuronShared = preNeuron.lock();

//     if (preNeuronShared) {
//         if (preNeuronShared->getSpike() == 1) {
//             sumCycles += cycles;
//             numSpikes += 1;
//             std::cout << "sumCycles new: " << sumCycles << " NumSpikes new: " << numSpikes << std::endl;
//             spike = true;
//         }
//     } else {
//         std::cerr << "Error: preNeuron is null or destroyed" << std::endl;
//     }
//     return spike;
// }