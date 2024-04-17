#include "GUI.h"

int main(int argc, char* argv[]){
    QApplication application(argc, argv);

    GUI gui;
    gui.show();

    return application.exec();
}

