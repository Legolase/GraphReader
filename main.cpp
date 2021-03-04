//#include "mainwindow.h"

#include "Panel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Panel window;
    window.setWindowIcon(QIcon(":resorse\\Image\\icon.png"));

    window.show();

    return a.exec();
}
