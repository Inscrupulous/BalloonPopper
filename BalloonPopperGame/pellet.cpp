#include <pellet.h>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <typeinfo>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QList>
#include "balloon.h"

int pellet::popped = 0;
int pellet::missed = 0;
double pellet::accuracy = 0;

pellet::pellet(QString color): QGraphicsPixmapItem (/*parent*/){
    myColor = color;
    if (color == "Red")
        setPixmap(QPixmap(":/RedPellet.png"));
    else if (color == "Pink")
        setPixmap(QPixmap(":/PinkPellet.png"));
    else if (color == "Cyan")
        setPixmap(QPixmap(":/CyanPellet.png"));
    else if (color == "Yellow")
        setPixmap(QPixmap(":/YellowPellet.png"));
    else if (color == "Green")
        setPixmap(QPixmap(":/GreenPellet.png"));
    else if (color == "Blue")
        setPixmap(QPixmap(":/BluePellet.png"));
    else if (color == "Purple")
        setPixmap(QPixmap(":/PurplePellet.png"));
    else if (color == "Black")
        setPixmap(QPixmap(":/BlackPellet.png"));

    missed++; // increase missed when pellet is created

    // Make a timer to move() the bullet every 0.1 seconds
//    QTimer * timer = new QTimer;
//    connect(timer,SIGNAL(timeout()),this,SLOT(move()));
//    timer->setInterval(1000/30);
//    timer->start();
    // start the timer
    //timer->start(3);
}

void pellet::advance(int step){
    if(step == 1) return;
    // get a list of all the items currently colliding with the pellet
    QList<QGraphicsItem *> colliding_items = collidingItems();

    // if one of the colliding items is a balloon, destroy both the pellet and the balloon
    for (int i = 0, n = colliding_items.size(); i < n; ++i){
      if (typeid(*(colliding_items[i])) == typeid(Balloon)){
            // increase the score
          if(this->myColor == static_cast<Balloon*>(colliding_items[i])->myColor){
              emit poppedOneSameColor(myColor);
          }
          else emit poppedOne(myColor);

            //play popped animation
            //pellet::popped();
            //QTimer::singleShot()
            //popped = new QPropertyAnimation(this);
            //popped->setDuration(100);

            // remove them from the scene (still on the heap)
            scene()->removeItem(colliding_items[i]);
            scene()->removeItem(this);

            // delete them from the heap to save memory
            delete colliding_items[i];
            disconnect(this,nullptr,nullptr,nullptr); // disconnect any slots and receivers associated with this pellet
            delete this;
            return;
        }
    }

    // if there was no collision with a balloon, move the pellet forward
    this->setPos(x(),y() - 10.0);
    // if the pellet is off the screen, destroy it
    if (this->scenePos().y() <= -YMAX+20){
        scene()->removeItem(this);
        disconnect(this,nullptr,nullptr,nullptr);
        delete this;
    }
}
