#ifndef TANK_H
#define TANK_H
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

class Tank: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Tank(QString color);
    //void keyPressEvent(QKeyEvent * event);
    ~Tank();

    int getID() const{return ID;}           // grab tank ID
    void setID(int i){ID = i;}              // set tank ID
    void setDir(int newDir){dir = newDir;}  // set tank's direction
    void setFiring(bool b){firing = b;}     // set whether tank is shooting
    qreal getDir(){return dir;}             // grab tank direction
    bool isFiring(){return firing;}         // grab wehther or not tank is shooting

private:
    int ID = 0;
    qreal dir = 0;      // Player direction (left = -11, stopped = 0, right = 1)
    bool firing = 0;    // contains whether the player is shooting (holding space) or not
};

#endif // Tank_H
