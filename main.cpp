#include "ui.h"

int main() {
    Measurement meas;
    LabInterface ui(&meas);
    ui.show();
    return Fl::run();
}