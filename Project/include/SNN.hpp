/**
 * @file SNN.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Defines the SNN class for simulating spiking neural networks.
 */

#pragma once

#include "Layer.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Dataset.hpp"

#include <fstream>
#include <sstream>

#include <functional>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <memory>

using namespace std;

/**
 * @class SNN
 * @brief Spiking Neural Network (SNN) class.
 *
 * This class handles the full lifecycle of a spiking neural network simulation,
 * including network initialization from a configuration file, training, testing,
 * and weight management. It supports various network topologies and biological neuron models.
 */
class SNN {
private:
    /** @brief Path to the network topology configuration file. */
    string topologyFile;
    /** @brief Base name used for saving output files. */
    string baseName;
    /** @brief Total duration of the simulation (in original unit). */
    long long time;
    /** @brief Time step used for simulation (in original unit). */
    unsigned long dt;
    /** @brief Unit for the total time (m, s, ms, us, ns). */
    string timeUnit;
    /** @brief Unit for the simulation time step (m, s, ms, us, ns). */
    string dtUnit;
    /** @brief Unit for the neuron refractory period (m, s, ms, us, ns). */
    string tRefrUnit;
    /** @brief Unit for synaptic delay (m, s, ms, us, ns). */
    string delayUnit;
    /** @brief Input layer height. */
    int height;
    /** @brief Input layer width. */
    int width;
    /** @brief Number of channels in the input layer. */
    int channels;
    /** @brief Maximum synaptic delay in the network. */
    int maxDelay;

    /** @brief Current operating mode: 'train' or 'test'. */
    string mode;

    /** @brief Parameters for each neuron type. */
    vector<NeuronParameters> neuronParams;

    /** @brief All layers in the network. */
    vector<shared_ptr<Layer>> layers;

    /** @brief Queue of input spikes (e.g., from input layer). */
    vector<deque<int>> inputSpikes;

    /** @brief Parses simulation-level parameters from config file. */
    void parseParameters(const string &line);
    /** @brief Parses neuron hyperparameters. */
    void parseHyperparameters(const string &line, NeuronParameters &neuron);
    /** @brief Parses network topology and builds layers. */
    void parseTopology(const string &line, TopologyParameters &topology);
    /** @brief Parses input configuration section. */
    void parseInput(const string &line);
    /** @brief Reads list of training or testing trials. */
    vector<string> readTrialsList(const string& filename);
    /** @brief Generates corresponding CSV annotation file name for a given AEDAT file. */
    string generateCSVFilename(const string& aedatFilename);
    /**
     * @brief Feeds input gesture data into the network.
     * @param spikeData Preprocessed spike maps.
     * @param gestureClass True gesture label.
     * @param symTime Current simulation time.
     * @param symCap Maximum simulation steps.
     * @return -1 if symCap is exceeded.
     */
    int processGestureData(const SpikeCubePolarity& spikeData, int gestureClass, unsigned long& symTime, unsigned long symCap);
public:
    /** @brief Constructs an empty SNN instance. */
    SNN();

    /** @brief Destructor. Frees layers. */
    ~SNN();

    /**
     * @brief Initializes the network from a configuration file.
     * @param filename Topology file name.
     * @param nMode Operation mode ("train" or "test").
     * @return 0 on success.
     */
    int initNetwork(char &filename, string &nMode);
    /** @brief Connects each layer to its previous layer. */
    void linkLayers();
    /** @brief Prints a summary of the network topology. */
    void viewTopology();

    /** @brief Trains the network on the defined training set. */
    void trainNetwork();

    /** @brief Saves learned synaptic weights into files depending on layer name. */
    void saveWeights();
    /**
     * @brief Loads weights from saved files corresponding to each layer name.
     * @return 0 on success.
     */
    int loadWeights();

    /** @brief Tests the network using a defined test set. */
    void testNetwork();
};