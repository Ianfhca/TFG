// #include <opencv2/opencv.hpp>
#include "../include/Layer.hpp"


Layer::Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt)
    : type(topology.type), height(topology.height), width(topology.width), channels(topology.channels),
      numNeurons(topology.numNeurons), neuronType(topology.neuronType), connections(topology.connections),
      rDim(topology.r), multisynapses(topology.multisynapses), sparseConnections(topology.sparseConnections) {

    // spikeHistory.reserve(11);
    if (type == "Output") spikeHistory.resize(numNeurons, 0);

    int neuronIdx = 0;

    for (int ch = 0; ch < channels; ch++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                neuronIdx = ((ch * height * width) + (i * width) + j);
                vector<int> neuronId = {i, j, ch, neuronIdx}; // Store neuron ID with additional info
                neurons.emplace_back(make_shared<LIFneuron>(neuronId, topology, neuronParams, dt));
                // neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology, neuronParams, dt));
            }
        }
    }
}

// Layer::Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt) {
//     type = topology.type;
//     height = topology.height;
//     width = topology.width;
//     channels = topology.channels;
//     // numNeurons = topology.numNeurons;
//     numNeurons = topology.numNeurons;
//     neuronType = topology.neuronType;
//     connections = topology.connections;
//     multisynapses = topology.multisynapses;
//     sparseConnections = topology.sparseConnections;
//     rDim = topology.r;
//     // kDim = topology.k;

//     spikeHistory.reserve(11);
//     spikeHistory.resize(11, 0);

//     int neuronIdx = 0;

//     for (int ch = 0; ch < channels; ch++) {
//         for (int i = 0; i < height; i++) {
//             for (int j = 0; j < width; j++) {
//                 neuronIdx = ((ch * height * width) + (i * width) + j);
//                 neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology, neuronParams, dt));
//                 // neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology.neuronType, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams.vTh, neuronParams.vRest, neuronParams.vReset, neuronParams.lambdaV, neuronParams.tRefr, neuronParams.lambdaX, neuronParams.alpha, dt));
//             }
//         }
//     }
// }

Layer::~Layer() {
    // cout << "-----------------------" << endl;
    // cout << "Destroying Layer " << type << endl;
    neurons.clear();
}

string Layer::getType() {
    return type;
};

int Layer::getHeight() {
    return height;
};

int Layer::getWidth() {
    return width;
};

int Layer::getChannels() {
    return channels;
};

int Layer::getNumNeurons() {
    return numNeurons;
};

int Layer::getNeuronsType() {
    return neuronType;
};

string Layer::getConnections() {
    return connections;
};

int Layer::getRDim() {
    return rDim;
};

int Layer::getMultisynapses() {
    return multisynapses;
};

shared_ptr<LIFneuron> Layer::getNeuron(int i) {
    return neurons[i];
};

vector<pair<int, int>>& Layer::getSparseConnections() {
    return sparseConnections;
};

void Layer::setPresynapticLinks(Layer &preLayer) {
    // if (connections == "local") {
    //     int preHeight = preLayer.getHeight();
    //     int preWidth = preLayer.getWidth();
    //     int preChannels = preLayer.getChannels();

    //     int preY, preX, preCh = 0;
    //     int neuronIdx, preNeuronIdx = 0;

    //     for (int ch = 0; ch < channels; ch++) {
    //         for (int i = 0; i < height; i++) {
    //             for (int j = 0; j < width; j++) {
    //                 neuronIdx = ((ch * height * width) + (i * width) + j);
    //                 for (int y = 0; y < rDim; y++) {
    //                     for (int x = 0; x < rDim; x++) {
    //                         preY = i + y;
    //                         preX = j + x;
    //                         preCh = ch % preChannels;
    //                         if (preY >= 0 && preY < preHeight && preX >= 0 && preX < preWidth) {
    //                             preNeuronIdx = ((preCh * preHeight * preWidth) + (preY * preWidth) + preX);
    //                             // preNeuronIdx = preChannels * preHeight * preWidth + preY * preWidth + preX;
    //                             if (preNeuronIdx < preLayer.getNumNeurons()) {
    //                                 neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx]);
    //                             }
    //                             // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx  << " (H: " << i << ", W: " << j << ", ch: " << ch << ")" << " (H: " << preY << ", W: " << preX << ", Ch: " << preCh << ")" << endl;  
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //         // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx  << " (ch: " << ch << ")" << " (H: " << preY << ", W: " << preX << ", Ch: " << preCh << ")" << endl;
    //     }
    if (connections == "local") {
        int preHeight = preLayer.getHeight();
        int preWidth = preLayer.getWidth();
        int preChannels = preLayer.getChannels();

        for (int ch = 0; ch < channels; ch++) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int neuronIdx = ((ch * height * width) + (i * width) + j);
                    for (int preCh = 0; preCh < preChannels; preCh++) {
                        for (int y = 0; y < rDim; y++) {
                            for (int x = 0; x < rDim; x++) {
                                int preY = i + y - rDim / 2;
                                int preX = j + x - rDim / 2;
                                if (preY >= 0 && preY < preHeight && preX >= 0 && preX < preWidth) {
                                    int preNeuronIdx = ((preCh * preHeight * preWidth) + (preY * preWidth) + preX);
                                    if (preNeuronIdx < preLayer.getNumNeurons()) {
                                        neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (connections == "sparse") {
        int preNeuronIdx, neuronIdx = 0;

        for (unsigned long i = 0; i < sparseConnections.size(); i++) {
            preNeuronIdx = sparseConnections[i].first;
            neuronIdx = sparseConnections[i].second;
            neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx]);
            // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx << endl;
        }
    } else {
        int preNumNeurons = preLayer.getNumNeurons();
        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preNumNeurons; j++) {
                neurons[i]->setPresynapticLink(preLayer.neurons[j]); // Check this getNeurons
            }
        }
    }
}

void Layer::feedForward(const string& mode, int classLabel, int t) {
    int spike = 0;
    // cout << "Layer " << layerId << " number of neurons: " << numNeurons << endl;
    if (type == "Output") {
        string fileName;
        if (mode == "train") fileName = "output/spikes/training_output.txt";
        else if (mode == "test") fileName = "output/spikes/testing_output.txt";
        else fileName = "output/spikes/training_output.txt";
        ofstream file(fileName, ios::app);

        for (int i = 0; i < numNeurons; i++) {
            spike = neurons[i]->updateNeuronState(t);
            spikeHistory[i] += spike;
            if (spike == 1) file << "N" << i << " = " << t << " (" << classLabel << ")" << endl;
        }
        file.close();
    } else {
        for (int i = 0; i < numNeurons; i++) spike = neurons[i]->updateNeuronState(t);
    }
}

void Layer::saveWeights(const string& baseName, const int layerId) {
    int i = 0;
    string fileName = baseName + "0" + to_string(i) + "_" + to_string(layerId) + "_" + type + ".txt";

    while (filesystem::exists(fileName) && i < 10) {
        i += 1;
        if (i < 10) {
            fileName = baseName + "0" + to_string(i) + "_" + to_string(layerId) + "_" + type + ".txt";
        } else {
            fileName = baseName + to_string(i) + "_" + to_string(layerId) + "_" + type + ".txt";
        }
    }

    cout << "Saving weights to " << fileName << endl;

    for (int i = 0; i < numNeurons; i++) {
        neurons[i]->saveWeights(fileName);
    }
}

void Layer::loadWeights(const std::string& baseName, const int layerId) {
    string fileName = baseName + to_string(layerId) + "_" + type + ".txt";
    for (int i = 0; i < numNeurons; i++) {
        neurons[i]->loadWeights(fileName);
        // std::cout << "Loaded weights for neuron " << i << " from " << fileName << std::endl;
    }
}


void Layer::showSpikeHistory() {
    for (unsigned long i = 0; i < neurons.size(); i++) {
        cout << "Neuron " << i << " Spike History: " << spikeHistory[i] << endl;
    }
}

// void Layer::visualizeSpikes(int t) {
//     int maxDisplay = std::min(numNeurons, 1024);

//     // Guarda la fila de spikes actual
//     std::vector<uchar> spikesRow(maxDisplay, 0);
//     for (int i = 0; i < maxDisplay; i++) {
//         spikesRow[i] = neurons[i]->getSpike() ? 255 : 0;
//     }
//     spikeHistory.push_back(spikesRow);

//     // Limita el historial a las últimas 200 filas (por ejemplo)
//     int maxRows = 200;
//     if (spikeHistory.size() > maxRows)
//         spikeHistory.erase(spikeHistory.begin());

//     // Crea la imagen del raster plot
//     cv::Mat spikeImg(spikeHistory.size(), maxDisplay, CV_8UC1);
//     for (size_t r = 0; r < spikeHistory.size(); ++r)
//         for (int c = 0; c < maxDisplay; ++c) 
//             spikeImg.at<uchar>(r, c) = spikeHistory[r][c];

//     // Escala para que sea visible
//     cv::resize(spikeImg, spikeImg, cv::Size(maxDisplay, 400), 0, 0, cv::INTER_NEAREST);
//     cv::imshow("Spikes Layer", spikeImg);
//     cv::waitKey(1);
// }

// void Layer::visualizeSpikes(int t) {
//     int displayWidth = 1024;
//     cv::Mat spikeImg = cv::Mat::zeros(1, numNeurons, CV_8UC1);

//     for (int i = 0; i < numNeurons; i++) {
//         int spike = neurons[i]->getSpike();
//         spikeImg.at<uchar>(0, i) = spike ? 255 : 0;
//     }

//     cv::resize(spikeImg, spikeImg, cv::Size(displayWidth, 20), 0, 0, cv::INTER_NEAREST);
//     cv::imshow("Spikes Layer", spikeImg);
//     cv::waitKey(1);
// }

// void Layer::visualizeSpikes(int t) {
//     int displayWidth = 1024;
//     int maxDisplay = std::min(numNeurons, displayWidth);
//     int maxRows = 200; // Número de pasos temporales a mostrar

//     // Guarda la fila de spikes actual
//     std::vector<uchar> spikesRow(maxDisplay, 0);
//     for (int i = 0; i < maxDisplay; i++) {
//         spikesRow[i] = neurons[i]->getSpike() ? 255 : 0;
//     }
//     spikeHistory.push_back(spikesRow);

//     // Limita el historial a las últimas maxRows filas
//     if (spikeHistory.size() > maxRows)
//         spikeHistory.erase(spikeHistory.begin());

//     // Crea la imagen del raster plot
//     cv::Mat spikeImg(spikeHistory.size(), maxDisplay, CV_8UC1);
//     for (size_t r = 0; r < spikeHistory.size(); ++r)
//         for (int c = 0; c < maxDisplay; ++c)
//             spikeImg.at<uchar>(r, c) = spikeHistory[r][c];

//     // Escala para que sea visible
//     cv::resize(spikeImg, spikeImg, cv::Size(displayWidth, 400), 0, 0, cv::INTER_NEAREST);
//     cv::imshow("Spikes Layer", spikeImg);
//     cv::waitKey(1);
// }