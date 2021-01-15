#ifndef PLAYERDATA_H
#define PLAYERDATA_H

#include <QtNetwork/QTcpServer>
#include "messagetype.h"

struct Player{
    // Add qint8/qint16 variables for positions
    qreal posX = 0;     // Player x-coordinate
    qreal posY = 0;     // Player y-coordinate

    int color = static_cast<int>(Color::NullColor); // Players color
    int score = 0;      // Player's score

    //Client Name
    QString name = "unknown";   //should the name be used as an id? it wouldn't be unique, but it would simplify things
    qint16 id;                 //uid needs to be assigned at player initialization. Somehow keep track of the lowest unclaimed uid and assign it each time a Player is created

    // not used in client
    QTcpSocket *client = nullptr;   // Socket for connection
    Player* nextPlayer = nullptr;   //points to the next player in the sequence
};

struct SimplePlayer
{
    QString name = "unknown";
    int score = 0;
    float scoreTime = 0;
};

#endif // PLAYERDATA_H
