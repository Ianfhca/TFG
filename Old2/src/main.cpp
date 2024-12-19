#include "../include/Globals.h"
#include "../include/SNN.h"
#include "../include/Utils.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return 1;
	}

	SNN snn;
	snn.initNetwork(*argv[1]);
	// snn.linkLayers();
	// snn.viewTopology();

	// snn.trainNetwork(TIME, DT);



	// double valor;
    // string unidadOrigen, unidadDestino;

    // cout << "Ingrese el valor de tiempo: ";
    // cin >> valor;

    // cout << "Ingrese la unidad de origen (minutos, segundos, milisegundos, microsegundos, nanosegundos): ";
    // cin >> unidadOrigen;

    // cout << "Ingrese la unidad de destino (minutos, segundos, milisegundos, microsegundos, nanosegundos): ";
    // cin >> unidadDestino;

    // long long resultado = convertirTiempo(valor, unidadOrigen, unidadDestino);

    // if (resultado != -1.0) {
    //     cout << valor << " " << unidadOrigen << " son " << resultado << " " << unidadDestino << "." << endl;
    // }

	return 0;
}