#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <fstream>

struct MeasurementData {
    int n;
    double time;
    double temp;
    double current;
    double voltage;
    double resistance;
};

class Measurement {
public:
    Measurement();
    bool start(const char* filename);
    void stop();
    MeasurementData nextStep(); // Generates/Reads the next data point
    bool isRunning() const { return active; }

private:
    std::ofstream salida;
    bool active;
    int step_count;
};

#endif