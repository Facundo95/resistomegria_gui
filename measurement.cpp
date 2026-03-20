#include "measurement.h"
#include <cmath>
#include <iostream>

#ifdef ENABLE_IEEE_HARDWARE
#include "IEEE-C.H"
#endif

Measurement::Measurement()
    : active(false), step_count(1), hardware_connected(false), last_connection_success(false) {}

bool Measurement::connect_hardware() {
#ifdef ENABLE_IEEE_HARDWARE
    long int status = 0;

    int tipo_de_interface = gpib_board_present();
    std::cout << "Tipo de KM-488 = " << tipo_de_interface << "\n";
    if (tipo_de_interface == 0) {
        last_status_message = "No se detecto la interfaz GPIB KM-488. Prueba conectar los equipos y reiniciar la computadora.";
        std::cerr << last_status_message << "\n";
        return false;
    }

    initialize(21, 0);

    if (!listener_present(7)) {
        last_status_message = "Nanovoltimetro (addr 7) no conectado.";
        std::cerr << last_status_message << "\n";
        return false;
    }
    if (!listener_present(12)) {
        last_status_message = "Fuente de corriente (addr 12) no conectada.";
        std::cerr << last_status_message << "\n";
        return false;
    }
    if (!listener_present(16)) {
        last_status_message = "Multimetro (addr 16) no conectado.";
        std::cerr << last_status_message << "\n";
        return false;
    }

    send(7, "*RST", &status);
    send(12, "*RST", &status);
    send(16, "*RST", &status);

    send(7, "SENS:FUNC 'VOLT'", &status);
    send(16, ":SENS:FUNC 'TEMP'", &status);
    send(16, ":SENS:TEMP:TC:TYPE k", &status);
    send(16, "temp:tc:rjun:rsel real", &status);
    send(12, "outp on", &status);

    last_status_message = "Dispositivos conectados correctamente.";

    return true;
#else
    last_status_message = "Modo simulado activo (ENABLE_IEEE_HARDWARE no definido).";
    std::cout << last_status_message << "\n";
    return true;
#endif
}

bool Measurement::start(const char* filename) {
    last_connection_success = false;

    if (!hardware_connected) {
        hardware_connected = connect_hardware();
        last_connection_success = hardware_connected;
        if (!hardware_connected) return false;
    } else {
        last_connection_success = true;
        if (last_status_message.empty()) {
            last_status_message = "Dispositivos conectados correctamente.";
        }
    }

    salida.open(filename);
    if (salida.is_open()) {
        salida << "N\tt(s)\tT(C)\ti(A)\tV(V)\tR(Ohms)\n";
        active = true;
        step_count = 1;
        return true;
    }

    last_status_message = "No se pudo abrir el archivo de salida.";
    return false;
}

void Measurement::stop() {
    active = false;

#ifdef ENABLE_IEEE_HARDWARE
    long int status = 0;
    send(12, "sour:clear:imm", &status);
#endif

    if (salida.is_open()) salida.close();
}

MeasurementData Measurement::nextStep() {
    MeasurementData d;
    d.n = step_count++;
    d.time = d.n * 2.0;
    d.temp = 25.0 + (d.n * 0.1);
    d.current = 0.1;
    d.resistance = 0.012 + (d.n * 0.0001);
    d.voltage = d.resistance * d.current;

    if (salida.is_open()) {
        salida << d.n << "\t" << d.time << "\t" << d.temp << "\t" 
               << d.current << "\t" << d.voltage << "\t" << d.resistance << "\n";
        salida.flush();
    }
    return d;
}