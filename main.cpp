#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Box.H>
#include <fstream>
#include <vector>
#include "ieee-c.h" // GPIB header

// Global UI Elements
Fl_Input       *file_input;
Fl_Value_Input *time_input;
Fl_Chart       *res_time_chart;
Fl_Chart       *res_temp_chart;
Fl_Button      *start_btn;
Fl_Box         *status_label;

std::ofstream salida;
bool is_running = false;
double current_time_elapsed = 0;

void measure_step(void* data) {
    if (!is_running) return;

    int status, l;
    char voltajeCh[20], tempCh[20];
    
    // --- INSERT YOUR GPIB LOGIC HERE ---
    // Example: send(7, "read?", &status); 
    // enter(voltajeCh, 20, &l, 7, &status);
    
    // Mock data for demonstration (Replace with your atof logic)
    double R = 100.0 + (rand() % 10); 
    double T = 25.0 + (current_time_elapsed * 0.1);

    // Save to File
    if (salida.is_open()) {
        salida << current_time_elapsed << "\t" << T << "\t" << R << std::endl;
    }

    // Update Charts
    res_time_chart->add(R, NULL, FL_BLUE);
    res_temp_chart->add(R, NULL, FL_RED);
    
    // Update Status
    static char buf[64];
    snprintf(buf, 64, "R: %.4f Ohm | T: %.2f C", R, T);
    status_label->label(buf);

    // Schedule next measurement based on user input
    current_time_elapsed += time_input->value();
    Fl::repeat_timeout(time_input->value(), measure_step);
}

// Button Callbacks
void start_stop_cb(Fl_Widget* w, void* data) {
    if (!is_running) {
        // Start Logic
        std::string filename = std::string(file_input->value()) + ".txt";
        salida.open(filename.c_str(), std::ios::out);
        
        is_running = true;
        start_btn->label("STOP");
        start_btn->color(FL_RED);
        Fl::add_timeout(0.1, measure_step);
    } else {
        // Stop Logic
        is_running = false;
        salida.close();
        start_btn->label("START");
        start_btn->color(FL_GREEN);
        Fl::remove_timeout(measure_step);
    }
}

int main() {
    Fl_Window *win = new Fl_Window(800, 500, "Resistometry Lab - XP Edition");

    // Controls
    file_input = new Fl_Input(120, 20, 150, 30, "Filename:");
    file_input->value("data_log");

    time_input = new Fl_Value_Input(400, 20, 50, 30, "Interval (s):");
    time_input->value(1.0);

    start_btn = new Fl_Button(600, 20, 100, 30, "START");
    start_btn->callback(start_stop_cb);
    start_btn->color(FL_GREEN);

    // Charts
    res_time_chart = new Fl_Chart(50, 80, 700, 180, "Resistance vs Time");
    res_time_chart->type(FL_LINE_CHART);
    
    res_temp_chart = new Fl_Chart(50, 300, 700, 180, "Resistance vs Temperature");
    res_temp_chart->type(FL_LINE_CHART);

    status_label = new Fl_Box(50, 480, 700, 20, "Ready");

    win->end();
    win->show();
    return Fl::run();
}