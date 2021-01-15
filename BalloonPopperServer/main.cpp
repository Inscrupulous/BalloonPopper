#include "balloonserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BalloonServer w;
    w.show();
    return a.exec();
}
