#ifndef PELLET_H
#define PELLET_H
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QObject>
#include <QPropertyAnimation>
#include "defs.h"

class pellet: public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
public:
    pellet(QString color);
    QString myColor;
    static int howManyPopped() {return popped;}
    static int howManyMissed() {return missed;}
    static void resetStats() {popped = 0; missed = 0;}
    void setAccuracy() {accuracy = popped/missed * 100;}
    static double accurancyInPercentage() {return accuracy;}

protected:
    void advance(int step);

signals:
    void poppedOne(QString);
    void poppedOneSameColor(QString);
public slots:
    //void move();
    //void addToGame();
private:
    static int popped;
    static int missed;
    static double accuracy;
    pellet* Pellet;
};
#endif // PELLET_H
