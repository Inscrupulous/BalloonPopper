#ifndef BALLOON_H
#define BALLOON_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

class Balloon : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Balloon(int color, double trajectory);
    QString myColor = "Null";
    double orientation;

protected:
    void advance(int);

signals:
    void deleteBalloon(int J);

public slots:
    //void move(double trajectory);
};

#endif // BALLOON_H
