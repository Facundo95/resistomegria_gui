#include "measurement.h"
#include <cmath>

Measurement::Measurement() : active(false), step_count(1) {}

bool Measurement::start(const char* filename) {
    salida.open(filename);
    if (salida.is_open()) {
        salida << "N\tt(s)\tT(C)\ti(A)\tV(V)\tR(Ohms)\n";
        active = true;
        step_count = 1;
        return true;
    }
    return false;
}

void Measurement::stop() {
    active = false;
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