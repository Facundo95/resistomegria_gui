#ifndef SIMPLE_PLOT_H
#define SIMPLE_PLOT_H

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/Fl_Gl_Window.H>
#include <vector>

struct PlotPoint {
    double x, y;
};

class SimplePlot : public Fl_Gl_Window {
private:
    std::vector<PlotPoint> data;
    double minX, maxX, minY, maxY;
    int gridDivs;

public:
    // This is the declaration of the constructor
    SimplePlot(int X, int Y, int W, int H, const char* L = 0);

    // Methods
    void draw() override;
    void add_point(double x, double y);
    void clear_data();
    void draw_labels(double minV, double maxV, bool isXAxis, double range);
};

#endif