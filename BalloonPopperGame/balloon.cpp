#include "balloon.h"
#include "messagetype.h"
#include <QPixmap>
#include <qmath.h>
#include <QTimer>
#include "defs.h"
#include <QGraphicsScene>
#include <QDebug>

/******************************************************
 * Balloon class should be created by passing in an int
 *
 * Balloon(static_cast<int>(Color::Red);
 *
 * For creating with message from server
 * this can be done with a simple passing of the int
 * that was read
 * i.e.
 *
 * Balloon(int)
 *
 * myColor variable should be used to compare color with
 * pellet that collides with it
 * ****************************************************/

Balloon::Balloon(int color, double trajectory) : QGraphicsPixmapItem ()
{
    switch(color){
    case static_cast<int>(Color::Red):
        setPixmap(QPixmap(":/RedBalloon.png"));
        myColor = "Red";
        break;
    case static_cast<int>(Color::Blue):
        setPixmap(QPixmap(":/BlueBalloon.png"));
        myColor = "Blue";
        break;
    case static_cast<int>(Color::Cyan):
        setPixmap(QPixmap(":/CyanBalloon.png"));
        myColor = "Cyan";
        break;
    case static_cast<int>(Color::Pink):
        setPixmap(QPixmap(":/PinkBalloon.png"));
        myColor = "Pink";
        break;
    case static_cast<int>(Color::Black):
        setPixmap(QPixmap(":/BlackBalloon.png"));
        myColor = "Black";
        break;
    case static_cast<int>(Color::Green):
        setPixmap(QPixmap(":/GreenBalloon.png"));
        myColor = "Green";
        break;
    case static_cast<int>(Color::Purple):
        setPixmap(QPixmap(":/PurpleBalloon.png"));
        myColor = "Purple";
        break;
    case static_cast<int>(Color::Yellow):
        setPixmap(QPixmap(":/YellowBalloon.png"));
        myColor = "Yellow";
        break;
    }

    orientation = trajectory;
}

void Balloon::advance(int S) {
    if(S == 0){ return;}
    int step = 1;
    double dx, dy;

    // Create angled step distance by converting trajectory from degrees to radians
    dx = step * qCos(qDegreesToRadians(orientation));
    dy = step * qSin(qDegreesToRadians(orientation));

    // Set new position
    setPos(x()+dx, y()+dy);

    // If the Balloon reaches the bottom of the screen, remove it
    if (this->scenePos().y() >= 0){
        emit deleteBalloon(0);
        scene()->removeItem(this);
        disconnect(this,nullptr,nullptr,nullptr);
        delete this;
    }
}

