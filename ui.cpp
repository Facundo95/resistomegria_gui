#include "ui.h"
#include <iostream>
#include <cstdio>
#include <FL/fl_ask.H> // Required for fl_message
#include "simple_plot.h"

// Constructor: Setup the layout and widgets
LabInterface::LabInterface(Measurement* meas) : engine(meas) {
    win = new Fl_Window(800, 580, "Resistometry Lab - XP Edition");

    win->set_modal();

    win->begin();

    // File Input
    file_input = new Fl_Input(80, 20, 150, 30, "Filename:");
    file_input->value("data_log.csv");

    // Interval Input
    time_input = new Fl_Value_Input(310, 20, 50, 30, "Interval (s):");
    time_input->value(1.0);

    // Start/Continue Button
    start_btn = new Fl_Button(440, 20, 100, 30, "START");
    start_btn->color(FL_GREEN);
    
    // Connect the button to the callback, passing 'this' (the UI) as data
    start_btn->callback(start_continue_cb, this);

    stop_btn = new Fl_Button(620, 20, 100, 30, "STOP");
    stop_btn->color(FL_RED);
    stop_btn->deactivate();
    stop_btn->callback(stop_cb, this);

    // 2. Initialize Resistance vs Time Chart
    res_time_chart = new SimplePlot(100, 100, 650, 162.5, "Resistance vs Time");
    res_time_chart->set_scale_factor(100.0); // Default scale
    res_time_chart->set_axis_titles("Time (s)", "Resistance");

    win->add(res_time_chart);

    res_temp_chart = new SimplePlot(100, 340, 650, 162.5, "Resistance vs Temperature");
    res_temp_chart->set_scale_factor(100.0); // Default scale
    res_temp_chart->set_axis_titles("Temperature (°C)", "Resistance");

    win->add(res_temp_chart);

    win->end();
}

void LabInterface::show() {
    win->show();
}

// --- CALLBACKS ---

void start_continue_cb(Fl_Widget* w, void* data) {
    LabInterface* ui = (LabInterface*)data;
    Fl_Button* btn = (Fl_Button*)w;
    Measurement* meas = ui->engine;

    // Get the current label to decide the next state
    std::string currentState = btn->label();

    if (currentState == "START") {
        ui->stop_btn->activate(); // <--- Enable the STOP button
        ui->stop_btn->color(FL_RED);
        ui->stop_btn->redraw();

        ui->file_input->deactivate();
        ui->time_input->deactivate();
    }

    if (currentState == "START" || currentState == "CONTINUE") {
        // ACTION: Start or Resume the simulation
        if (meas->start(ui->file_input->value())) {
            btn->label("PAUSE");
            btn->color(FL_YELLOW); 
            
            // Start the timer loop (1.0s or user defined interval)
            double interval = ui->time_input->value();
            if (interval <= 0) interval = 1.0; 
            Fl::add_timeout(interval, timer_cb, ui);
        }
    } 
    else if (currentState == "PAUSE") {
        // ACTION: Pause the simulation
        meas->stop(); // Assuming your logic handles pausing via stop()
        btn->label("CONTINUE");
        btn->color(FL_GREEN);
        
        // Stop the background updates
        Fl::remove_timeout(timer_cb, ui);
    }
    
    btn->redraw();
}

void stop_cb(Fl_Widget* w, void* data) {
    LabInterface* ui = (LabInterface*)data;
    Measurement* meas = ui->engine;

    // 1. Stop the measurement and timer immediately
    meas->stop();
    Fl::remove_timeout(timer_cb, ui);

    // 2. Show the Success Pop-up
    // This function blocks execution until the user clicks "OK"
    fl_message_title("Lab System Notification");
    fl_message("Measurement completed successfully and data has been logged.");

    // 3. Reset the Charts (Empty the graphs)
    ui->res_time_chart->reset();
    ui->res_temp_chart->reset();

    // 4. Reset the START button
    ui->start_btn->label("START");
    ui->start_btn->color(FL_GREEN);
    ui->start_btn->redraw();

    ui->stop_btn->deactivate();
    ui->file_input->activate();
    ui->time_input->activate();
}

void timer_cb(void* data) {
    LabInterface* ui = (LabInterface*)data;
    Measurement* meas = ui->engine;

    if (!meas->isRunning()) return;

    // Get the next data point from the logic layer
    MeasurementData d = meas->nextStep();

    static double elapsed_time = 0;
    elapsed_time += ui->time_input->value();

    // Update the visual charts
    ui->res_time_chart->add_data(elapsed_time, d.resistance);
    ui->res_temp_chart->add_data(d.temp, d.resistance);


    // Force redraw to show new points
    ui->res_time_chart->redraw();
    ui->res_temp_chart->redraw();


    // Schedule next run based on user input
    double interval = ui->time_input->value();
    if (interval <= 0.05) interval = 0.5; // Safety floor
    
    Fl::repeat_timeout(interval, timer_cb, ui);
}