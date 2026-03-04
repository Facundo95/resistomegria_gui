#ifndef UI_H
#define UI_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Device.H>
#include <FL/fl_draw.H>
#include "measurement.h"

class LabInterface {
public:
    LabInterface(Measurement* sim);
    void show();
    
    // UI Elements made public for easy access by callbacks
    Fl_Window      *win;
    Fl_Input       *file_input;
    Fl_Value_Input *time_input;
    Fl_Chart       *res_time_chart;
    Fl_Chart       *res_temp_chart;
    Fl_Button      *start_btn;
    
    Measurement* engine;
};

class Fl_Grid_Chart : public Fl_Chart {
    void draw() {
        Fl_Chart::draw(); // Draw the background and title
        
        // Setup grid color and style
        fl_color(FL_LIGHT2); // Light grey color
        fl_line_style(FL_DOT); // Dotted lines
        
        // Draw 5 horizontal lines
        for(int i = 1; i < 5; i++) {
            int yy = y() + (h() * i / 5);
            fl_line(x(), yy, x() + w(), yy);
        }
        
        // Draw 5 vertical lines
        for(int i = 1; i < 5; i++) {
            int xx = x() + (w() * i / 5);
            fl_line(xx, y(), xx, y() + h());
        }
        
        fl_line_style(0); // Reset line style to solid
    }
public:
    Fl_Grid_Chart(int X, int Y, int W, int H, const char* L=0) 
        : Fl_Chart(X, Y, W, H, L) {}
};

// Callback helper
void timer_cb(void* data);
void start_stop_cb(Fl_Widget* w, void* data);

#endif