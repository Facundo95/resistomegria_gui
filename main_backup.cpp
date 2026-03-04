#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Box.H>
#include <iostream>
#include <fstream>
#include <vector>
#include "IEEE-C.H" // GPIB header

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

std::ofstream test_salida;
bool is_testing = false;
int test_N = 1;

double min_temp = 999.0, max_temp = -999.0;
double min_res  = 999.0, max_res  = -999.0;

void test_write_loop(void* v);
void measure_step(void* data);
void start_stop_cb(Fl_Widget* w, void* data);
void update_plots(double current_temp, double current_res);

// --- THE TEST METHOD (Timer Callback) ---
void test_write_loop(void* v) {
    if (!is_testing) return;

    // Simulate physics: Temp rises, Resistance rises slightly
    double sim_t = 25.0 + (test_N * 0.1);           // T(C)
    double sim_i = 0.1;                             // i(A)
    double sim_r = 0.012 + (test_N * 0.0001);       // R(Ohms)
    double sim_v = sim_r * sim_i;                   // V(V)
    double sim_time = test_N * 2.0;                 // t(s)

    if (test_salida.is_open()) {
        // Writing the specific header format: N \t t(s) \t T(C) \t i(A) \t V(V) \t R(Ohms)
        test_salida << test_N << "\t" 
                    << sim_time << "\t" 
                    << sim_t << "\t" 
                    << sim_i << "\t" 
                    << sim_v << "\t" 
                    << sim_r << "\n";
        
        // Ensure data is written to disk immediately
        test_salida.flush(); 
    }

    // Update the UI plots while we are at it
    res_time_chart->add(sim_r, NULL, FL_BLUE);
    res_temp_chart->add(sim_r, NULL, FL_RED);

    test_N++;
    
    // Repeat every 1 second for the test
    Fl::repeat_timeout(1.0, test_write_loop);
}

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

void start_stop_cb(Fl_Widget* w, void* data) {
    if (!is_testing) {
        // 1. Open the stream
        test_salida.open("test.csv", std::ios::out);
        
        // 2. Write the Header
        test_salida << "N \t t(s) \t T(C) \t i(A) \t V(V) \t R(Ohms)\n";
        
        // 3. Set flags and start loop
        is_testing = true;
        test_N = 1;
        Fl::add_timeout(1.0, test_write_loop);
        
        std::cout << "Test Stream Started: writing to test.csv" << std::endl;
    } else {
        is_testing = false;
        test_salida.close();
        Fl::remove_timeout(test_write_loop);
        std::cout << "Test Stream Stopped." << std::endl;
    }
}



void update_plots(double current_temp, double current_res) {
    // 1. Update Temperature Limits for X-Axis scaling
    bool needs_rescale = false;
    if (current_temp < min_temp) { min_temp = current_temp; needs_rescale = true; }
    if (current_temp > max_temp) { max_temp = current_temp; needs_rescale = true; }
    
    // 2. Update Resistance Limits for Y-Axis scaling
    if (current_res < min_res) { min_res = current_res; needs_rescale = true; }
    if (current_res > max_res) { max_res = current_res; needs_rescale = true; }

    // 3. Apply Rescaling
    if (needs_rescale) {
        // bounds(min_y, max_y) - FLTK Charts auto-scale X based on number of points,
        // but we set the Y bounds to keep the "wiggle" visible.
        res_temp_chart->bounds(min_res * 0.95, max_res * 1.05); 
    }

    // 4. Add the data point
    // Note: Fl_Chart adds points sequentially. 
    // If you need a true X-Y scatter plot (R vs T), you might need Fl_Chart::type(FL_LINE_CHART)
    res_temp_chart->add(current_res, NULL, FL_RED);
    res_time_chart->add(current_res, NULL, FL_BLUE);
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

    win->end();
    win->show();
    return Fl::run();
}