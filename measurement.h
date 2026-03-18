#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <fstream>
#include <string>

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
    bool was_last_connection_successful() const { return last_connection_success; }
    const std::string& get_last_status_message() const { return last_status_message; }

private:
    std::ofstream salida;
    bool active;
    int step_count;
    bool hardware_connected;
    bool last_connection_success;
    std::string last_status_message;
    
    bool connect_hardware();
};

#endif