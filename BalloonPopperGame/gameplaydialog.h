#ifndef GAMEPLAYDIALOG_H
#define GAMEPLAYDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QObject>
#include <QPropertyAnimation>
#include "balloon.h"
#include "mainwindow.h"
#include "timer.h"
#include "client.h"
#include "tank.h"
#include "pellet.h"
#include "gameenddialog.h"

namespace Ui {
class GamePlayDialog;
}

class GamePlayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GamePlayDialog(Client* c = nullptr, MainWindow *lobby = nullptr, int secs = 10, QWidget *parent = nullptr);
    ~GamePlayDialog();
    QTimer *timer;
    QTime *time;
    QTimer *movementTimer;
    int SecondsToEnd;
    int initialTime;
    int dxTank = 10;

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    bool eventFilter(QObject *watched, QEvent *event);  // event filter to catch arrow keys

public slots:
    void updateCountdown();
    void playerPosUpdate(QTextStream &incomingData);
    void pelletAdd(QTextStream &incomingData);
    void pelletAddThis(QString, qreal);
    void InitializeSchedule(QTextStream &incomingData);
    void BalloonHit(QString);
    void BalloonSameColorHit(QString);
    void BalloonDelete(int K);
    void UpdateScores(QTextStream &incomingData);       // Uncomment input parameter when we set up score broadcasting

private:
    Ui::GamePlayDialog *ui;
    MainWindow* lobbyScreen = nullptr;
    Client* client = nullptr;
    QString clientColor;
    QGraphicsScene* scene;
    QList<Tank*> tanks; // stores scene tanks
    Tank* myTank;   // stores my tank
    Tank* tank = nullptr; // used to create scene tanks
    pellet* Pellet;
    bool pelletBuffered = false;    //when a player presses space, a pellet is buffered. Once a pellet has been shot from the buffer, the buffer is cleared until the player releases the key and presses it again.
    int score = 0;
    QTimer *gameUpdateTimer = nullptr;
    QTimer *shootingTimer = nullptr;
    bool shotProcessed = false;
    bool spaceWasReleased = false;
    QList<Balloon*> balloons;
    QList<int> Times;
    QList<int> BalloonID;
    QList<int> BalloonColors;
    QList<int> XCoordinates;
    QList<int> YCoordinates;
    QList<double> Trajectories;
    int prevTime = 0;

    int myID = 0xffffff; // ID should never be this high

    QSet<int> keysPressed; //replace with dir and firing variables in the Player object.

    void sendPelletRequest(qreal xpos);
    void sendPlayerPos(qreal xpos);
private slots:
    void DisconnectHandler();
    void gameUpdate();
    void spawn();
    void shootingCheck();
    void dataRec(QTextStream&);
};

#endif // GAMEPLAYDIALOG_H
