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
#include <vector>
#include <string>
#include "measurement.h"



class SimplePlot : public Fl_Widget {
    std::vector<double> data;
    double min_y, max_y;
    Fl_Color line_color;

public:
    SimplePlot(int X, int Y, int W, int H, const char* L = 0) 
        : Fl_Widget(X, Y, W, H, L), min_y(0), max_y(100), line_color(FL_BLUE) {}

    void add_point(double val) {
        data.push_back(val);
        if (data.size() > 100) data.erase(data.begin()); // Keep last 100 points
        redraw();
    }

    void set_bounds(double min, double max) { min_y = min; max_y = max; }
    void set_line_color(Fl_Color c) { line_color = c; }

    void draw() override {
        // 1. Draw White Background & Border
        fl_push_clip(x(), y(), w(), h());
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());
        fl_color(FL_BLACK);
        fl_rect(x(), y(), w(), h());

        // 2. Draw Grid (Light Grey)
        fl_color(FL_LIGHT2);
        fl_line_style(FL_DOT);
        for(int i = 1; i < 5; i++) {
            int yy = y() + (h() * i / 5);
            fl_line(x(), yy, x() + w(), yy);
            int xx = x() + (w() * i / 5);
            fl_line(xx, y(), xx, y() + h());
        }

        // 3. Draw Axis Ticks & Labels (Outside the clipping later)
        fl_line_style(FL_SOLID);
        fl_color(FL_BLACK);
        for(int i = 0; i <= 5; i++) {
            int yy = y() + h() - (h() * i / 5);
            int xx = x() + (w() * i / 5);
            fl_line(x()-5, yy, x(), yy); // Y-tick
            fl_line(xx, y()+h(), xx, y()+h()+5); // X-tick
        }

        // 4. Draw the Data Line
        if (data.size() > 1) {
            fl_color(line_color);
            double x_step = (double)w() / 100.0; 
            for (size_t i = 1; i < data.size(); i++) {
                int x1 = x() + (int)((i-1) * x_step);
                int y1 = y() + h() - (int)((data[i-1] - min_y) / (max_y - min_y) * h());
                int x2 = x() + (int)(i * x_step);
                int y2 = y() + h() - (int)((data[i] - min_y) / (max_y - min_y) * h());
                fl_line(x1, y1, x2, y2);
            }
        }
        fl_pop_clip();
        
        // Draw the title label above the widget
        draw_label();
    }
};


class LabInterface {
public:
    LabInterface(Measurement* sim);
    void show();
    
    // UI Elements made public for easy access by callbacks
    Fl_Window      *win;
    Fl_Input       *file_input;
    Fl_Value_Input *time_input;
    SimplePlot *res_time_chart;
    SimplePlot *res_temp_chart;
    Fl_Button      *start_btn;
    
    Measurement* engine;
};

// Callback helper
void timer_cb(void* data);
void start_stop_cb(Fl_Widget* w, void* data);

#endif