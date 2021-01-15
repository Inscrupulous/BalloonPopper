#ifndef BALLOONSERVER_H
#define BALLOONSERVER_H

#include <QDialog>
#include <QtNetwork/QTcpServer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class BalloonServer; }
QT_END_NAMESPACE

struct clientInfo{

    // Socket for connection
    QTcpSocket *currentClient;

    // Add qint8/qint16 variables for positions
    qint16 posX = 0;
    qint16 posY = 0;

    //Client Name
    QString playerName = "Safety dance";

    //Index data **might be useful for handling disconnects
    quint8 m_index = 0;
};

class BalloonServer : public QDialog
{
    Q_OBJECT

public:
    explicit BalloonServer(QWidget *parent = nullptr); //I'm not sure why these constructors are usually explicit
    ~BalloonServer();

private:
    Ui::BalloonServer *ui;

    QTcpServer *tcpServer = nullptr; //declare our server object
    clientInfo clientConnections[5];
    quint8 clientIndex = 0;
    void SendNameData();

private slots:
    void startServer();
    void sendHelloMessage();
    void updatePositions(clientInfo &currentClient);

};
#endif // BALLOONSERVER_H
