#include "ui.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <FL/fl_ask.H> // Required for fl_message
#include <FL/Fl_Native_File_Chooser.H>
#include "simple_plot.h"

namespace {
bool file_exists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

std::string make_unique_path(const std::string& full_path) {
    if (!file_exists(full_path)) return full_path;

    size_t slash_pos = full_path.find_last_of("/\\");
    std::string directory = (slash_pos == std::string::npos) ? "" : full_path.substr(0, slash_pos + 1);
    std::string name_with_ext = (slash_pos == std::string::npos) ? full_path : full_path.substr(slash_pos + 1);

    size_t dot_pos = name_with_ext.find_last_of('.');
    std::string stem = (dot_pos == std::string::npos) ? name_with_ext : name_with_ext.substr(0, dot_pos);
    std::string ext = (dot_pos == std::string::npos) ? "" : name_with_ext.substr(dot_pos);

    int index = 1;
    std::string candidate;
    do {
        candidate = directory + stem + "(" + std::to_string(index) + ")" + ext;
        ++index;
    } while (file_exists(candidate));

    return candidate;
}
}

// Constructor: Setup the layout and widgets
LabInterface::LabInterface(Measurement* meas) : engine(meas) {
    int screen_w = Fl::w();
    int screen_h = Fl::h();

    win = new Fl_Window(screen_w, screen_h, "Resistometry Lab - XP Edition");
    //win->callback(close_window_cb, this);

    win->begin();

    // File Input
    file_input = new Fl_Input(80, 20, 120, 30, "Filename:");
    file_input->value("data_log.csv");

    // Output folder selector
    folder_btn = new Fl_Button(220, 20, 60, 30, "Folder");
    folder_btn->callback(folder_select_cb, this);
    save_folder = ".";
    folder_btn->copy_tooltip(save_folder.c_str());

    // Current Input
    current_input = new Fl_Value_Input(380, 20, 50, 30, "Current (mA):");
    current_input->value(1.0);

    // Interval Input
    time_input = new Fl_Value_Input(510, 20, 50, 30, "Interval (s):");
    time_input->value(1.0);

    // Start/Continue Button
    start_btn = new Fl_Button(580, 20, 80, 30, "START");
    start_btn->color(FL_GREEN);
    
    // Connect the button to the callback, passing 'this' (the UI) as data
    start_btn->callback(start_continue_cb, this);

    stop_btn = new Fl_Button(680, 20, 80, 30, "STOP");
    stop_btn->color(FL_RED);
    stop_btn->deactivate();
    stop_btn->callback(stop_cb, this);

    // 2. Initialize Resistance vs Time Chart
    res_time_chart = new SimplePlot(100, 100, 650, 162.5, "Resistance vs Time");
    res_time_chart->set_axis_titles("Time (s)", "Resistance");
    res_time_chart->set_line_color(FL_BLUE);

    win->add(res_time_chart);

    res_temp_chart = new SimplePlot(100, 340, 650, 162.5, "Resistance vs Temperature");
    res_temp_chart->set_axis_titles("Temperature (°C)", "Resistance");
    res_temp_chart->set_line_color(FL_MAGENTA);

    win->add(res_temp_chart);

    win->end();
}

void LabInterface::show() {
    win->show();
}

std::string LabInterface::build_output_path() const {
    std::string filename = file_input->value() ? file_input->value() : "";
    if (filename.empty()) filename = "data_log.csv";

    std::string full_path;
    if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        full_path = filename;
    } else {
        std::string base = save_folder.empty() ? "." : save_folder;
        if (!base.empty() && (base.back() == '/' || base.back() == '\\')) {
            full_path = base + filename;
        } else {
            full_path = base + "/" + filename;
        }
    }

    return make_unique_path(full_path);
}

// --- CALLBACKS ---

void folder_select_cb(Fl_Widget* w, void* data) {
    LabInterface* ui = (LabInterface*)data;
    Fl_Button* btn = (Fl_Button*)w;

    Fl_Native_File_Chooser chooser;
    chooser.title("Select output folder");
    chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    if (!ui->save_folder.empty()) {
        chooser.directory(ui->save_folder.c_str());
    }

    int result = chooser.show();
    if (result == 0 && chooser.filename() && chooser.filename()[0] != '\0') {
        ui->save_folder = chooser.filename();
        btn->copy_tooltip(ui->save_folder.c_str());
        btn->redraw();
    }
}

void start_continue_cb(Fl_Widget* w, void* data) {
    LabInterface* ui = (LabInterface*)data;
    Fl_Button* btn = (Fl_Button*)w;
    Measurement* meas = ui->engine;

    // Get the current label to decide the next state
    std::string currentState = btn->label();
    bool is_start_press = (currentState == "START");

    if (currentState == "START") {
        ui->stop_btn->activate(); // <--- Enable the STOP button
        ui->stop_btn->color(FL_RED);
        ui->stop_btn->redraw();

        ui->file_input->deactivate();
        ui->current_input->deactivate();
        ui->time_input->deactivate();
        ui->folder_btn->deactivate();
    }

    if (currentState == "START" || currentState == "CONTINUE") {
        // ACTION: Start or Resume the simulation
        std::string entered_name = ui->file_input->value() ? ui->file_input->value() : "";
        std::string output_path = ui->build_output_path();

        // Reflect the final (possibly renamed) target in the input box
        if (entered_name.find('/') == std::string::npos && entered_name.find('\\') == std::string::npos) {
            size_t slash_pos = output_path.find_last_of("/\\");
            std::string final_name = (slash_pos == std::string::npos) ? output_path : output_path.substr(slash_pos + 1);
            ui->file_input->value(final_name.c_str());
        } else {
            ui->file_input->value(output_path.c_str());
        }

        if (meas->start(output_path.c_str())) {
            if (is_start_press) {
                fl_message_title("Hardware Status");
                fl_message("%s", meas->get_last_status_message().c_str());
            }

            btn->label("PAUSE");
            btn->color(FL_YELLOW); 
            
            // Start the timer loop (1.0s or user defined interval)
            double interval = ui->time_input->value();
            if (interval <= 0) interval = 1.0; 
            Fl::add_timeout(interval, timer_cb, ui);
        } else {
            if (is_start_press) {
                fl_alert("No fue posible iniciar la medicion.\n%s", meas->get_last_status_message().c_str());
            }

            ui->stop_btn->deactivate();
            ui->file_input->activate();
            ui->time_input->activate();
            ui->folder_btn->activate();
            ui->current_input->activate();
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
    ui->folder_btn->activate();
    ui->current_input->activate();
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