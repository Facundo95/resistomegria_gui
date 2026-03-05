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
    bool autoscale; // New flag
    Fl_Color line_color;

public:
    SimplePlot(int X, int Y, int W, int H, const char* L = 0) 
        : Fl_Widget(X, Y, W, H, L), min_y(0), max_y(100), autoscale(true), line_color(FL_BLUE) {}

    void set_autoscale(bool s) { autoscale = s; redraw(); }

    void add_point(double val) {
        data.push_back(val);
        if (data.size() > 100) data.erase(data.begin());

        if (autoscale && !data.empty()) {
            double current_min = data[0];
            double current_max = data[0];
            
            for (double d : data) {
                if (d < current_min) current_min = d;
                if (d > current_max) current_max = d;
            }

            // Add a small 10% margin so the line isn't touching the very top/bottom
            double padding = (current_max - current_min) * 0.1;
            if (padding == 0) padding = 1.0; // Prevent flat line issues
            
            min_y = current_min - padding;
            max_y = current_max + padding;
        }
        redraw();
    }

    // Keep manual bounds as an option
    void set_bounds(double min, double max) { 
        autoscale = false; 
        min_y = min; 
        max_y = max; 
        redraw(); 
    }

    void clear_data() {
        data.clear();
        redraw();
    }

    void set_line_color(Fl_Color c) { line_color = c; }

    void draw() override {
        // 1. Draw the Background and Border
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());
        fl_color(FL_BLACK);
        fl_rect(x(), y(), w(), h());

        // 2. Draw the Grid (Dotted Grey)
        fl_color(FL_LIGHT2);
        fl_line_style(FL_DOT);
        for(int i = 1; i < 5; i++) {
            int yy = y() + (h() * i / 5);
            fl_line(x(), yy, x() + w(), yy);
            int xx = x() + (w() * i / 5);
            fl_line(xx, y(), xx, y() + h());
        }
        fl_line_style(FL_SOLID); // Reset to solid for everything else

        // 3. Draw Axis Ticks and Numbers
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA, 10);
        char buf[16];
        for(int i = 0; i <= 5; i++) {
            // Y-Axis (Vertical)
            int yy = y() + h() - (h() * i / 5);
            fl_line(x() - 5, yy, x(), yy); 
            std::snprintf(buf, 16, "%d", (int)(min_y + (max_y - min_y) * i / 5)); 
            fl_draw(buf, x() - 35, yy + 5); 

            // X-Axis (Horizontal)
            int xx = x() + (w() * i / 5);
            fl_line(xx, y() + h(), xx, y() + h() + 5);
            std::snprintf(buf, 16, "%d", i * 10);
            fl_draw(buf, xx - 10, y() + h() + 15);
        }

        // 4. Draw Data Lines (Clipped to prevent drawing over labels)
        if (data.size() > 1) {
            fl_push_clip(x(), y(), w(), h());
            fl_color(line_color);
            double x_step = (double)w() / 100.0; 
            for (size_t i = 1; i < data.size(); i++) {
                int x1 = x() + (int)((i-1) * x_step);
                int y1 = y() + h() - (int)((data[i-1] - min_y) / (max_y - min_y) * h());
                int x2 = x() + (int)(i * x_step);
                int y2 = y() + h() - (int)((data[i] - min_y) / (max_y - min_y) * h());
                fl_line(x1, y1, x2, y2);
            }
            fl_pop_clip();
        }

        // 5. Draw the "Resistencia vs..." label on top
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
    Fl_Button      *stop_btn;
    
    Measurement* engine;
};

// Callback helper
void timer_cb(void* data);
void start_continue_cb(Fl_Widget* w, void* data);
void stop_cb(Fl_Widget* w, void* data);

#endif