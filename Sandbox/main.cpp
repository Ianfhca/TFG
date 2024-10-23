#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>  // para manejar el formato de impresión

class AdaptiveNeuron {
public:
    double v;  // potencial de membrana
    double v_rest, v_reset, v_th, lambda_v, i_syn;  // parámetros de la neurona
    double t_ref;  // tiempo refractario
    bool in_refractory;  // estado refractario de la neurona
    double time_last_spike;  // último tiempo de disparo
    double dt;  // paso de tiempo

    // Parámetros para la adaptación homeostática
    std::vector<double> X;  // trazas presinápticas
    double lambda_X;  // constante de tiempo de la traza
    double alpha;  // factor de escalado para las trazas

    AdaptiveNeuron(double v_rest_, double v_reset_, double v_th_, double lambda_v_, double t_ref_, double dt_, double lambda_X_, double alpha_)
        : v(v_rest_), v_rest(v_rest_), v_reset(v_reset_), v_th(v_th_), lambda_v(lambda_v_), t_ref(t_ref_), in_refractory(false), dt(dt_), lambda_X(lambda_X_), alpha(alpha_) {}

    // Actualizar la dinámica de la neurona
    void update(double input_current, double time) {
        if (in_refractory) {
            // Si la neurona está en periodo refractario, no actualiza el potencial
            if (time - time_last_spike >= t_ref) {
                in_refractory = false;  // Sale del periodo refractario
            } else {
                return;
            }
        }

        // Dinámica del potencial de membrana con integración de Euler
        v += dt * (- (v - v_rest) / lambda_v + input_current);
        // v += (- (v - v_rest) +input_current) * (dt / lambda_v);

        // Comprobar si la neurona alcanza el umbral de disparo
        if (v >= v_th) {
            fire(time);
        }
    }

    // Función que se llama cuando la neurona dispara
    void fire(double time) {
        v = v_reset;  // Reiniciar el potencial de membrana
        in_refractory = true;  // Activar el periodo refractario
        time_last_spike = time;  // Guardar el tiempo de disparo
        std::cout << "Neuron fired at time " << std::fixed << std::setprecision(1) << time << " ms" << std::endl;
    }

    // Actualizar la traza presináptica
    void updateTrace(double s_pre) {
        for (size_t d = 0; d < X.size(); d++) {
            X[d] += dt * (-X[d] / lambda_X + alpha * s_pre);  // Trazas presinápticas
            // X[d] += dt * (-X[d] + alpha * s_pre) * (dt / lambda_X);
        }
    }

    // Calcular el input sináptico basado en las trazas presinápticas y los pesos sinápticos
    double synapticInput(const std::vector<double>& W, const std::vector<double>& s_pre) {
        double input_current = 0.0;
        for (size_t j = 0; j < W.size(); j++) {
            input_current += W[j] * s_pre[j];  // Integrar el input sináptico
        }
        return input_current;
    }
};

// Clase para manejar una sinapsis con STDP
class Synapse {
public:
    double W;  // peso sináptico
    double tau;  // retraso sináptico
    double eta;  // tasa de aprendizaje para STDP
    double A_plus;  // factor de potenciación a largo plazo (LTP)
    double A_minus;  // factor de depresión a largo plazo (LTD)

    Synapse(double W_, double tau_, double eta_, double A_plus_, double A_minus_)
        : W(W_), tau(tau_), eta(eta_), A_plus(A_plus_), A_minus(A_minus_) {}

    // Regla de actualización STDP
    void updateWeight(double delta_t) {
        if (delta_t > 0) {
            // Potenciación si el presináptico dispara antes
            W += eta * A_plus * exp(-delta_t / tau);
        } else {
            // Depresión si el presináptico dispara después
            W -= eta * A_minus * exp(delta_t / tau);
        }

        // Limitar el peso sináptico entre un rango
        if (W < 0) W = 0;
        if (W > 1) W = 1;
    }

    void printWeight() const {
        std::cout << "Current synaptic weight: " << std::fixed << std::setprecision(4) << W << std::endl;
    }
};

// Simulación de red neuronal simple con salida visual
int main() {
    double dt = 0.1;  // paso de tiempo (ms)
    double sim_time = 100.0;  // tiempo total de simulación (ms)

    // Parámetros de la neurona
    double v_rest = -65.0;
    double v_reset = -70.0;
    double v_th = -50.0;
    double lambda_v = 10.0;
    double t_ref = 2.0;  // tiempo refractario
    double lambda_X = 20.0;  // constante de tiempo de la traza presináptica
    double alpha = 0.1;

    // Crear dos neuronas
    AdaptiveNeuron pre_neuron(v_rest, v_reset, v_th, lambda_v, t_ref, dt, lambda_X, alpha);
    AdaptiveNeuron post_neuron(v_rest, v_reset, v_th, lambda_v, t_ref, dt, lambda_X, alpha);

    // Crear una sinapsis entre ellas
    Synapse synapse(0.5, 20.0, 0.005, 0.1, 0.12);  // W, tau, eta, A+, A-

    // Vector de entrada espigada (pre_neuron firing history)
    std::vector<double> s_pre(1, 0);

    // Simulación de la red neuronal
    std::cout << "Starting simulation..." << std::endl;
    for (double t = 0; t < sim_time; t += dt) {
        // Actualizar la actividad presináptica
        s_pre[0] = (t > 10 && t < 12) ? 1 : 0;  // Simulación de un spike en pre_neuron en t=10 ms

        // Actualizar la traza de la neurona presináptica
        pre_neuron.updateTrace(s_pre[0]);

        // Calcular el input sináptico para la neurona postsináptica
        double i_syn = post_neuron.synapticInput({synapse.W}, s_pre);
        post_neuron.update(i_syn, t);

        // Mostrar información del potencial de membrana y estado de disparo
        std::cout << "Time: " << std::fixed << std::setprecision(1) << t << " ms, Post-neuron membrane potential: " << post_neuron.v << " mV " << i_syn << std::endl;

        // Si la neurona postsináptica dispara, aplicar STDP
        // if (post_neuron.in_refractory) {
        //     double delta_t = t - pre_neuron.time_last_spike;
        //     synapse.updateWeight(delta_t);  // Actualizar los pesos con STDP
        //     synapse.printWeight();  // Mostrar el peso sináptico actualizado
        // }
    }

    std::cout << "Simulation finished!" << std::endl;

    return 0;
}
