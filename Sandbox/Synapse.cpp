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
};
