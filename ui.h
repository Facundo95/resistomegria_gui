#ifndef UI_H
#define UI_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>
#include <vector>
#include <string>
#include "measurement.h"
#include "simple_plot.h"


class LabInterface {
public:
    LabInterface(Measurement* sim);
    void show();
    
    // UI Elements made public for easy access by callbacks
    Fl_Window      *win;
    Fl_Input       *file_input;
    Fl_Value_Input *time_input;
    Fl_Button      *start_btn;
    Fl_Button      *stop_btn;

    SimplePlot* res_time_chart;
    SimplePlot* res_temp_chart;
    
    Measurement* engine;
};

// Callback helper
void timer_cb(void* data);
void start_continue_cb(Fl_Widget* w, void* data);
void stop_cb(Fl_Widget* w, void* data);

#endif