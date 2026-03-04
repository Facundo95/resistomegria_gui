#include "ui.h"
#include <iostream>
#include <cstdio>
#include <FL/fl_draw.H>

// Constructor: Setup the layout and widgets
LabInterface::LabInterface(Measurement* meas) : engine(meas) {
    win = new Fl_Window(800, 500, "Resistometry Lab - XP Edition");

    // File Input
    file_input = new Fl_Input(120, 20, 150, 30, "Filename:");
    file_input->value("data_log.csv");

    // Interval Input
    time_input = new Fl_Value_Input(400, 20, 50, 30, "Interval (s):");
    time_input->value(1.0);

    // Start/Stop Button
    start_btn = new Fl_Button(600, 20, 100, 30, "START");
    start_btn->color(FL_GREEN);
    
    // Connect the button to the callback, passing 'this' (the UI) as data
    start_btn->callback(start_stop_cb, this);

    // Resistance vs Time Chart
    res_time_chart = new Fl_Grid_Chart(50, 80, 700, 180, "Resistance vs Time");
    res_time_chart->type(FL_LINE_CHART);
    res_time_chart->color(FL_WHITE);

    // Resistance vs Temperature Chart
    res_temp_chart = new Fl_Grid_Chart(50, 300, 700, 180, "Resistance vs Temperature");
    res_temp_chart->type(FL_LINE_CHART);
    res_temp_chart->color(FL_WHITE);

    win->end();
}

void LabInterface::show() {
    win->show();
}

// --- CALLBACKS ---

void start_stop_cb(Fl_Widget* w, void* data) {
    LabInterface* ui = (LabInterface*)data;
    Measurement* meas = ui->engine;

    if (!meas->isRunning()) {
        // Start the simulation
        if (meas->start(ui->file_input->value())) {
            ui->start_btn->label("STOP");
            ui->start_btn->color(FL_RED);
            ui->start_btn->redraw();
            
            // Start the timer loop
            Fl::add_timeout(ui->time_input->value(), timer_cb, ui);
        }
    } else {
        // Stop the simulation
        meas->stop();
        ui->start_btn->label("START");
        ui->start_btn->color(FL_GREEN);
        ui->start_btn->redraw();
        
        Fl::remove_timeout(timer_cb, ui);
    }
}

void timer_cb(void* data) {
    LabInterface* ui = (LabInterface*)data;
    Measurement* meas = ui->engine;

    if (!meas->isRunning()) return;

    // Get the next data point from the logic layer
    MeasurementData d = meas->nextStep();

    // Update the visual charts
    ui->res_time_chart->add(d.resistance, NULL, FL_BLUE);
    ui->res_temp_chart->add(d.resistance, NULL, FL_RED);

    // Force redraw to show new points
    ui->res_time_chart->redraw();
    ui->res_temp_chart->redraw();

    // Schedule next run based on user input
    Fl::repeat_timeout(ui->time_input->value(), timer_cb, ui);
}