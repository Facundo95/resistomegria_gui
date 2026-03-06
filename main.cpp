#include "ui.h"

int main(int argc, char **argv) {

    Fl::gl_visual(FL_RGB | FL_SINGLE | FL_DEPTH);

    Measurement* engine = new Measurement(); // 1. Create Logic
    LabInterface* ui = new LabInterface(engine); // 2. Pass to UI
    
    ui->win->show(argc, argv);
    
    return Fl::run();
}