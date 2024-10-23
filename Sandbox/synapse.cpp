#include <vector>
#include <iostream>
#include <cmath>

// Definición de la capa de neuronas
class Layer {
public:
    std::vector<float> membranePotentials;
    std::vector<bool> spiked;
    
    Layer(int numNeurons) {
        membranePotentials.resize(numNeurons, 0.0f);
        spiked.resize(numNeurons, false);
    }
};

// Definición de la matriz tridimensional de sinapsis
class SynapseLayer {
public:
    std::vector<std::vector<std::vector<float>>> weights;
    std::vector<std::vector<std::vector<float>>> delays;
    std::vector<std::vector<std::vector<float>>> traces;
    
    SynapseLayer(int numPostNeurons, int numPreNeurons, int numConnections) {
        weights.resize(numPostNeurons, std::vector<std::vector<float>>(numPreNeurons, std::vector<float>(numConnections, 0.5f)));
        delays.resize(numPostNeurons, std::vector<std::vector<float>>(numPreNeurons, std::vector<float>(numConnections, 1.0f)));
        traces.resize(numPostNeurons, std::vector<std::vector<float>>(numPreNeurons, std::vector<float>(numConnections, 0.0f)));
    }
    
    void updateTraces(const Layer& preLayer, float tauX, float alpha) {
        for (int i = 0; i < weights.size(); i++) {
            for (int j = 0; j < weights[i].size(); j++) {
                for (int d = 0; d < weights[i][j].size(); d++) {
                    if (preLayer.spiked[j]) {
                        traces[i][j][d] = traces[i][j][d] * std::exp(-tauX) + alpha;
                    } else {
                        traces[i][j][d] = traces[i][j][d] * std::exp(-tauX);
                    }
                }
            }
        }
    }

    void propagate(Layer& postLayer, const Layer& preLayer, float threshold) {
        for (int i = 0; i < weights.size(); i++) {
            for (int j = 0; j < weights[i].size(); j++) {
                for (int d = 0; d < weights[i][j].size(); d++) {
                    postLayer.membranePotentials[i] += weights[i][j][d] * (preLayer.spiked[j] ? 1.0f : 0.0f);
                }
            }
            if (postLayer.membranePotentials[i] >= threshold) {
                postLayer.spiked[i] = true;
                postLayer.membranePotentials[i] = 0.0f; // resetear después del disparo
            } else {
                postLayer.spiked[i] = false;
            }
        }
    }
};

// Ejemplo de uso
int main() {
    int numPreNeurons = 5;
    int numPostNeurons = 3;
    int numConnections = 2;

    Layer preLayer(numPreNeurons);
    Layer postLayer(numPostNeurons);

    SynapseLayer synapseLayer(numPostNeurons, numPreNeurons, numConnections);

    // Simulación de una traza
    float tauX = 0.5f;
    float alpha = 1.0f;
    float threshold = 1.0f;

    // Estimulación de neuronas en la capa presináptica
    preLayer.spiked[2] = true; // Simula que la neurona 2 dispara
    
    // Actualización de trazas y propagación
    synapseLayer.updateTraces(preLayer, tauX, alpha);
    synapseLayer.propagate(postLayer, preLayer, threshold);

    // Resultados
    for (int i = 0; i < postLayer.spiked.size(); i++) {
        std::cout << "Post neuron " << i << (postLayer.spiked[i] ? " fired!" : " did not fire.") << std::endl;
    }

    return 0;
}
