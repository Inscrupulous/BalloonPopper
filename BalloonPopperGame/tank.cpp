#include "tank.h"

#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>


Tank::Tank(QString color): QGraphicsPixmapItem (/*parent*/)
{
    if (color == "Red")
        setPixmap(QPixmap(":/RedTank.png"));
    else if (color == "Pink")
        setPixmap(QPixmap(":/PinkTank.png"));
    else if (color == "Cyan")
        setPixmap(QPixmap(":/CyanTank.png"));
    else if (color == "Yellow")
        setPixmap(QPixmap(":/YellowTank.png"));
    else if (color == "Green")
        setPixmap(QPixmap(":/GreenTank.png"));
    else if (color == "Blue")
        setPixmap(QPixmap(":/BlueTank.png"));
    else if (color == "Purple")
        setPixmap(QPixmap(":/PurpleTank.png"));
    else if (color == "Black")
        setPixmap(QPixmap(":/BlackTank.png"));
}

Tank::~Tank()
{

}


