#ifndef BALLOONSERVER_H
#define BALLOONSERVER_H

#include <QDialog>
#include <QtNetwork/QTcpServer>
#include <QLabel>
#include <messagetype.h>
#include <QList>
#include "lobby.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class BalloonServer : public QDialog
{
    Q_OBJECT

public:
    explicit BalloonServer(QWidget *parent = nullptr); //I'm not sure why these constructors are usually explicit
    ~BalloonServer();
    QHostAddress getIP() const{return tcpServer->serverAddress();}
    quint16 getPort() const{return tcpServer->serverPort();}
signals:
    void closeClicked();

public slots:
    void closeServer();
    void playerDataReset(); // Resets positional and score data

private:
    // Private Data
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //db uses sqlite structurew
    QTcpServer *tcpServer = nullptr; //declare our server object
    Lobby *lobby = nullptr;         //lobby holds all the connected players
    quint8 lobbySize = 0;
    int dataOption = 0; // Used to read in type of message to be sent
    int colorRequested = static_cast<int>(Color::NullColor);
    QList<int> availableColors; // Used to store unchosen colors
    QList<int> unavaialableColors;
    QList<qint16> activeIDs;
    int gameTimeLimit = 30;
    bool scheduleCreated = false;
    QByteArray Schedule;


    // Other Functions
    void UpdateColorList();
    void CreateAndSendSchedule(QTextStream &incomingData);

    // Send Functions
    void SendPlayerData();
    void SendHighScore();
    void SendPlayerPosition();
    void SendProjectileCreateRequest();
    void sendPlayerPosData();
    void SendPlayerScoreData();
    void SendFinalScoreData(QTextStream&);

    // RECEIVAL FUNCTIONS
    void StartGame(QTextStream &incomingData);
    void ColorChangeRequest(QTextStream &incomingData);
    void pelletAdded(QTextStream&);
    void playerMoved(QTextStream&);
    void IncScore(QTextStream&);

private slots:
    void StartServer();
    void SendHelloMessage();
    void DataReceival(QTcpSocket*);
    void DataReceival(QTextStream&);


};
#endif // BALLOONSERVER_H
