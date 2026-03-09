#include "simple_plot.h"
#include <iomanip>
#include <sstream>
#include <FL/fl_draw.H>
#include <algorithm>
#include <iostream>

SimplePlot::SimplePlot(int X, int Y, int W, int H, const char* L) 
    : Fl_Chart(X, Y, W, H, 0) {

    type(FL_LINE_CHART);  // Connects points with lines
    box(FL_FLAT_BOX);
    color(FL_WHITE);      // Background
    selection_color(FL_WHITE);

    reset();
}

void SimplePlot::reset() {
    clear();
    x_data.clear();
    y_data.clear();
    min_y = 1e30;
    max_y = -1e30;
    bounds(0, 1); // Default initial view
}

void SimplePlot::add_data(double x, double y) {
    x_data.push_back(x);
    y_data.push_back(y);

    // 1. Update the actual data limits
    if (x < min_x) min_x = x;
    if (x > max_x) max_x = x;
    if (y < min_y) min_y = y;
    if (y > max_y) max_y = y;

    // 2. Calculate the "Real" Y-range used by the chart (including your 10% padding)
    double data_range_y = max_y - min_y;
    if (data_range_y == 0) data_range_y = 1.0; 
    double chart_range_y = data_range_y * 1.2; // Matching your padding logic

    // 3. Calculate the required X-range to maintain 1:1
    // Width / Height gives us the aspect ratio of the widget box
    double aspect_ratio = (double)w() / (double)h();
    double required_range_x = chart_range_y * aspect_ratio;

    // 4. Center the X-data within this new calculated range
    double center_x = (min_x + max_x) / 2.0;
    // Overwrite the display limits for the label drawing logic
    this->display_min_x = center_x - (required_range_x / 2.0);
    this->display_max_x = center_x + (required_range_x / 2.0);

    // 5. Apply Y-bounds to Fl_Chart
    double chart_min_y = min_y - (data_range_y * 0.1);
    double chart_max_y = max_y + (data_range_y * 0.1);
    bounds(chart_min_y, chart_max_y);
    
    add(y, "", FL_GREEN); 
    redraw();
    std::cout << "Added point: (" << x << ", " << y << ")\n";
}
    
void SimplePlot::update_tick_calculations() {
    x_ticks.clear();
    y_ticks.clear();

    if (min_y >= 1e20) return;

    // 1. Calculate Y-Range (with 10% padding)
    double data_range_y = max_y - min_y;
    if (data_range_y <= 0) data_range_y = 1.0; 
    double d_min_y = min_y - (data_range_y * 0.1);
    double d_max_y = max_y + (data_range_y * 0.1);
    double active_range_y = d_max_y - d_min_y;

    // 2. Calculate Y-Ticks
    int num_y = h() / 40;
    if (num_y < 2) num_y = 2;
    for (int i = 0; i <= num_y; i++) {
        Tick t;
        t.value = d_min_y + (active_range_y * i / (double)num_y);
        t.pixel_pos = y() + h() - (int)(h() * i / (double)num_y);
        y_ticks.push_back(t);
    }

    // 3. Calculate 1:1 X-Range based on Y-Range and Widget Aspect Ratio
    double aspect_ratio = (double)w() / (double)h();
    double required_range_x = (active_range_y * aspect_ratio) * scale_factor;
    
    double center_x = (min_x + max_x) / 2.0;
    this->display_min_x = center_x - (required_range_x / 2.0);
    this->display_max_x = center_x + (required_range_x / 2.0);

    // 4. Calculate X-Ticks (matching the density of Y)
    int num_x = (int)(num_y * aspect_ratio);
    if (num_x < 2) num_x = 2;
    for (int i = 0; i <= num_x; i++) {
        Tick t;
        t.value = display_min_x + (required_range_x * i / (double)num_x);
        t.pixel_pos = x() + (int)(w() * i / (double)num_x);
        x_ticks.push_back(t);
    }
}

void SimplePlot::draw_grid_lines() {
    fl_color(FL_BLACK); // Light gray for grid
    fl_line_style(FL_DOT);

    for (const auto& t : x_ticks) {
        if (t.pixel_pos >= x() && t.pixel_pos <= x() + w())
            fl_line(t.pixel_pos, y(), t.pixel_pos, y() + h());
    }
    for (const auto& t : y_ticks) {
        fl_line(x(), t.pixel_pos, x() + w(), t.pixel_pos);
    }
    fl_line_style(0);
}

void SimplePlot::draw_tick_labels() {
    fl_font(FL_HELVETICA, 10);
    fl_color(FL_BLACK);
    char buf[64];

    // Y-Axis Labels
    for (const auto& t : y_ticks) {
        if (std::abs(t.value) >= 1000) sprintf(buf, "%.1fk", t.value / 1000.0);
        else if (std::abs(t.value) > 0 && std::abs(t.value) < 0.1) sprintf(buf, "%.1fm", t.value * 1000.0);
        else sprintf(buf, "%.2f", t.value);

        fl_draw(buf, x() - 50, t.pixel_pos + 4);
        fl_line(x() - 5, t.pixel_pos, x(), t.pixel_pos);
    }

    // X-Axis Labels
    for (const auto& t : x_ticks) {
        if (t.pixel_pos < x() || t.pixel_pos > x() + w()) continue;
        
        if (std::abs(t.value) >= 1000) sprintf(buf, "%.1fk", t.value / 1000.0);
        else if (std::abs(t.value) > 0 && std::abs(t.value) < 0.1) sprintf(buf, "%.1fm", t.value * 1000.0);
        else sprintf(buf, "%.2f", t.value);

        int tw, th;
        fl_measure(buf, tw, th);
        fl_draw(buf, t.pixel_pos - (tw / 2), y() + h() + 15);
        fl_line(t.pixel_pos, y() + h(), t.pixel_pos, y() + h() + 5);
    }
}

void SimplePlot::draw() {
    Fl_Chart::draw(); // Base plot
    
    update_tick_calculations(); // Refresh math
    
    fl_push_no_clip();
    
    // Clear the outside area before drawing new labels
    fl_color(FL_GRAY);
    fl_rectf(x() - 80, y() - 40, 78, h() + 80); // Y area
    fl_rectf(x() - 20, y() + h() + 1, w() + 50, 80); // X area

    draw_grid_lines();
    draw_tick_labels();

    fl_pop_clip();
}