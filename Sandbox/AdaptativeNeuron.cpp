#include <iostream>
#include <vector>
#include <cmath>

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
        std::cout << "Neuron fired at time " << time << " ms" << std::endl;
    }

    // Actualizar la traza presináptica
    void updateTrace(double s_pre) {
        for (size_t d = 0; d < X.size(); d++) {
            X[d] += dt * (-X[d] / lambda_X + alpha * s_pre);  // Trazas presinápticas
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
