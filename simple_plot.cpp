#include "simple_plot.h"
#include <iomanip>
#include <sstream>
#include <FL/fl_draw.H>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
double snap_tick_step(double range, int target_tick_count) {
    if (target_tick_count < 1) target_tick_count = 1;

    double raw_step = range / static_cast<double>(target_tick_count);
    if (raw_step <= 0.0) raw_step = 0.1;

    return std::max(0.1, std::ceil(raw_step / 0.1) * 0.1);
}

double snap_down(double value, double step) {
    return std::floor(value / step) * step;
}

double snap_up(double value, double step) {
    return std::ceil(value / step) * step;
}

void format_tick_value(double value, char* out) {
    if (std::abs(value) >= 1000) sprintf(out, "%.1fk", value / 1000.0);
    else if (std::abs(value) > 0 && std::abs(value) < 0.1) sprintf(out, "%.1fm", value * 1000.0);
    else sprintf(out, "%.2f", value);
}
}

SimplePlot::SimplePlot(int X, int Y, int W, int H, const char* L) 
    : Fl_Chart(X, Y, W, H, 0) {

    type(FL_LINE_CHART);  // Connects points with lines
    box(FL_FLAT_BOX);
    color(FL_WHITE);      // Background
    selection_color(FL_WHITE);

    reset();
}

void SimplePlot::set_x_axis_title(const char* title) {
    x_axis_label = title ? title : "";
    redraw();
}

void SimplePlot::set_y_axis_title(const char* title) {
    y_axis_label = title ? title : "";
    redraw();
}

void SimplePlot::set_axis_titles(const char* x_title, const char* y_title) {
    x_axis_label = x_title ? x_title : "";
    y_axis_label = y_title ? y_title : "";
    redraw();
}

void SimplePlot::reset() {
    clear();
    x_data.clear();
    y_data.clear();
    min_x = 1e30;
    max_x = -1e30;
    min_y = 1e30;
    max_y = -1e30;
    display_min_x = 0.0;
    display_max_x = 1.0;
    scale_factor = 1.0;
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

    // 4. Keep the displayed X-axis starting from zero whenever possible
    this->display_min_x = 0.0;
    this->display_max_x = std::max(required_range_x, max_x);

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

    // 2. Calculate Y-Ticks
    int num_y = h() / 40;
    if (num_y < 2) num_y = 2;

    double y_step = snap_tick_step(d_max_y - d_min_y, num_y);
    double y_start = snap_down(d_min_y, y_step);
    double y_end = snap_up(d_max_y, y_step);
    double active_range_y = y_end - y_start;
    if (active_range_y <= 0.0) active_range_y = y_step;

    int y_tick_count = static_cast<int>(std::round(active_range_y / y_step));
    if (y_tick_count < 1) y_tick_count = 1;

    for (int i = 0; i <= y_tick_count; ++i) {
        double value = y_start + (y_step * i);
        Tick t;
        t.value = value;
        double normalized = (value - y_start) / active_range_y;
        t.pixel_pos = y() + h() - static_cast<int>(h() * normalized);
        y_ticks.push_back(t);
    }

    // 3. Calculate 1:1 X-Range based on Y-Range and Widget Aspect Ratio
    double aspect_ratio = (double)w() / (double)h();
    double required_range_x = (active_range_y * aspect_ratio) * scale_factor;

    this->display_min_x = 0.0;
    this->display_max_x = std::max(required_range_x, max_x);
    double active_range_x = display_max_x - display_min_x;

    // 4. Calculate X-Ticks (matching the density of Y)
    int num_x = (int)(num_y * aspect_ratio);
    if (num_x < 2) num_x = 2;

    double x_step = snap_tick_step(active_range_x, num_x);
    double x_start = snap_down(display_min_x, x_step);
    double x_end = snap_up(display_max_x, x_step);
    active_range_x = x_end - x_start;
    if (active_range_x <= 0.0) active_range_x = x_step;

    int x_tick_count = static_cast<int>(std::round(active_range_x / x_step));
    if (x_tick_count < 1) x_tick_count = 1;

    display_min_x = x_start;
    display_max_x = x_end;

    for (int i = 0; i <= x_tick_count; ++i) {
        double value = x_start + (x_step * i);
        Tick t;
        t.value = value;
        double normalized = (value - x_start) / active_range_x;
        t.pixel_pos = x() + static_cast<int>(w() * normalized);
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

    int y_label_right_x = x() - 8;

    // Y-Axis Labels
    for (const auto& t : y_ticks) {
        format_tick_value(t.value, buf);

        int tw = 0;
        int th = 0;
        fl_measure(buf, tw, th, 0);
        int label_x = std::max(2, y_label_right_x - tw);

        fl_draw(buf, label_x, t.pixel_pos + 4);
        fl_line(x() - 5, t.pixel_pos, x(), t.pixel_pos);
    }

    // X-Axis Labels
    for (const auto& t : x_ticks) {
        if (t.pixel_pos < x() || t.pixel_pos > x() + w()) continue;

        format_tick_value(t.value, buf);

        int tw, th;
        fl_measure(buf, tw, th);
        fl_draw(buf, t.pixel_pos - (tw / 2), y() + h() + 15);
        fl_line(t.pixel_pos, y() + h(), t.pixel_pos, y() + h() + 5);
    }
}

void SimplePlot::draw_axis_titles() {
    fl_font(FL_HELVETICA_BOLD, 12);
    fl_color(FL_BLACK);

    if (!x_axis_label.empty()) {
        int tw = 0;
        int th = 0;
        fl_measure(x_axis_label.c_str(), tw, th, 0);
        fl_draw(x_axis_label.c_str(), x() + (w() - tw) / 2, y() + h() + 40);
    }

    if (!y_axis_label.empty()) {
        int max_y_tick_width = 0;
        char tick_buf[64];
        fl_font(FL_HELVETICA, 10);
        for (const auto& t : y_ticks) {
            format_tick_value(t.value, tick_buf);

            int tick_w = 0;
            int tick_h = 0;
            fl_measure(tick_buf, tick_w, tick_h, 0);
            if (tick_w > max_y_tick_width) max_y_tick_width = tick_w;
        }

        fl_font(FL_HELVETICA_BOLD, 12);
        int title_width = 0;
        int title_height = 0;
        fl_measure(y_axis_label.c_str(), title_width, title_height, 0);

        int y_label_right_x = x() - 8;
        int title_x = std::max(8, y_label_right_x - max_y_tick_width - 18);
        int title_y = y() + (h() / 2) + (title_width / 2);

        fl_draw(90, y_axis_label.c_str(), title_x, title_y);
    }
}

void SimplePlot::draw() {
    Fl_Chart::draw(); // Base plot
    
    update_tick_calculations(); // Refresh math
    
    fl_push_no_clip();

    int max_y_tick_width = 0;
    char tick_buf[64];
    fl_font(FL_HELVETICA, 10);
    for (const auto& t : y_ticks) {
        format_tick_value(t.value, tick_buf);
        int tick_w = 0;
        int tick_h = 0;
        fl_measure(tick_buf, tick_w, tick_h, 0);
        if (tick_w > max_y_tick_width) max_y_tick_width = tick_w;
    }

    int y_title_height = 0;
    if (!y_axis_label.empty()) {
        fl_font(FL_HELVETICA_BOLD, 12);
        int title_w = 0;
        int title_h = 0;
        fl_measure(y_axis_label.c_str(), title_w, title_h, 0);
        y_title_height = title_h;
    }

    int left_margin_width = max_y_tick_width + y_title_height + 46;
    if (left_margin_width < 80) left_margin_width = 80;
    int left_margin_x = x() - left_margin_width;
    
    // Clear the outside area before drawing new labels
    fl_color(FL_GRAY);
    fl_rectf(left_margin_x, y() - 40, left_margin_width - 2, h() + 80); // Y area
    fl_rectf(x() - 20, y() + h() + 1, w() + 50, 80); // X area

    draw_grid_lines();
    draw_tick_labels();
    draw_axis_titles();

    fl_pop_clip();
}