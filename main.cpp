#include "ui.h"

int main() {
    Measurement* engine = new Measurement(); // 1. Create Logic
    LabInterface* ui = new LabInterface(engine); // 2. Pass to UI
    ui->show();
    return Fl::run();
}