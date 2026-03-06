#include "simple_plot.h"
#include <iomanip>
#include <sstream>
#include <FL/fl_draw.H>

SimplePlot::SimplePlot(int X, int Y, int W, int H, const char* L) 
    : Fl_Gl_Window(X, Y, W, H, L) {

    gridDivs = 5;
    clear_data(); // Use this to set the initial min/max values safely
}

void SimplePlot::clear_data() {
    data.clear();
    // Initialize with "inverted" extremes so the first point 
    // sets the actual boundaries of the graph.
    minX = minY = 1e30; 
    maxX = maxY = -1e30;
    redraw();
}

void SimplePlot::add_point(double x, double y) {
    data.push_back({x, y});
    
    // Update boundaries for the autoscale
    if (x < minX) minX = x;
    if (x > maxX) maxX = x;
    if (y < minY) minY = y;
    if (y > maxY) maxY = y;

    redraw(); // Important: this triggers the draw() method
}

void SimplePlot::draw_labels(double minV, double maxV, bool isXAxis, double range) {
    gl_font(FL_HELVETICA, 12);
    glColor3f(1.0f, 1.0f, 1.0f); // Pure white for testing

    for (int i = 0; i <= gridDivs; i++) {
        double val = minV + (range * i / (double)gridDivs);
        float pos = (float)i / (float)gridDivs;

        char buf[16];
        sprintf(buf, "%.1f", val);

        if (isXAxis) {
            glRasterPos2f(pos - 0.02f, -0.1f);
        } else {
            glRasterPos2f(-0.18f, pos - 0.02f);
        }
        gl_draw(buf);
    }
}

void SimplePlot::draw() {
    if (!valid()) {
        glViewport(0, 0, w(), h());
    }

    // Bright Red Background
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a single white point in the middle
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(50.0f); // Huge point
    glBegin(GL_POINTS);
        glVertex2f(0.0f, 0.0f);
    glEnd();

    glFlush(); 
    if (mode() & FL_DOUBLE) swap_buffers();
    valid(1);
}