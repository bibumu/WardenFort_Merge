#include "wardenfort.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wardenfort w;
    w.show();
    return a.exec();
}
