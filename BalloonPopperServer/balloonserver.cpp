#include "balloonserver.h"
#include "ui_server.h"
#include <QString>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

//constructor, inherits from QDialog
BalloonServer::BalloonServer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BalloonServer)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this); //initialize tcp server

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer())); //when the user clicks the button, the server is started.
    connect(tcpServer, &QTcpServer::newConnection, this, &BalloonServer::sendHelloMessage); //when a new connection is made to a client, say hi!
}

//destructor
BalloonServer::~BalloonServer()
{
    delete ui;
}

//slot function to start server once user clicks start button
void BalloonServer::startServer()
{
    ui->startButton->setEnabled(false); //once the server has started(or tried to start), it cannot try again

    qDebug() << "we are trying to start a server.";

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use  the first non-localhost IPv4 address
    //I THINK that this code just finds the address that the host is using
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())    //if we never put anything into it
    {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    // By moving this past the for loop we set a single IP address dynamically
    if(!tcpServer->listen(QHostAddress(ipAddress)))
    {
        qDebug() <<"it ain't workin";
        close(); //if it fails, we exit
        return;
    }

    ui->addressLabel->setText(ipAddress); //display which ip address client's should access
    ui->portLabel->setText(tr("%1").arg(tcpServer->serverPort()));  //display the port in use

    qDebug()<< tcpServer->serverAddress();
    qDebug() << tcpServer->serverPort();

    qDebug() <<"it should have worked ?";

    return;
}

// Send updated lobby/name data to all clients
void BalloonServer::SendNameData(){
    QByteArray out;
    QDataStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData.setVersion(QDataStream::Qt_4_8);

    // Set up client names
    for(auto i = 0; i < clientIndex; i++)
    {
        outgoingData << clientConnections[i].playerName.toUtf8();
    }

    //Send names out
    for(auto i = 0; i < clientIndex; i++)
    {
        if(!clientConnections[i].currentClient->isValid()) continue; // If someone disconnected don't write to them
        clientConnections[i].currentClient->write(out);
        clientConnections[i].currentClient->flush();
    }
}

// Set up client connection. Get clients name. Update lobby
void BalloonServer::sendHelloMessage()
{
    qDebug() << "ladies and gentlemen... we got em.";

    if(clientIndex < 5)
    {
        // Grab connection and wait for name to be sent
        clientConnections[clientIndex].currentClient = tcpServer->nextPendingConnection();
        clientConnections[clientIndex].currentClient->waitForReadyRead();

        QByteArray in;
        QDataStream incomingData(clientConnections[clientIndex].currentClient);
        incomingData.setVersion(QDataStream::Qt_4_8);

        incomingData >> in;
        clientConnections[clientIndex].playerName = in;

        //qDebug() << playerNames[clientIndex];
        // Set the current clients index **might be useful for disconnects
        clientConnections[clientIndex].m_index = clientIndex;
        clientIndex++;
        SendNameData();
        // Connect current socket to function that will update game states
        // Function passes current socket
        connect(clientConnections[clientIndex-1].currentClient, &QTcpSocket::readyRead, [=] {
            updatePositions(clientConnections[clientIndex-1]);
        });
    }
    else {
        QTcpSocket *clientToDeny = tcpServer->nextPendingConnection();
        QByteArray deny;
        QDataStream denialStream(&deny,  QIODevice::WriteOnly);
        denialStream << "Server has already reached player limit!";

        clientToDeny->write(deny);
        clientToDeny->flush();
        clientToDeny->disconnectFromHost();
        delete clientToDeny;
    }
}


void BalloonServer::updatePositions(clientInfo &currentClientInfo)
{
    // Game position stuff goes here
    //int x = currentClientInfo.posX;  // X coordinate data
    //int y = currentClientInfo.posY;  // Y coordinate data

    qDebug() << "We are in updatePositions now";

    // Relay positional data back to client

    //QByteArray dataToSend;
    //QDataStream out(&dataToSend, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_4_8);
    // out << positional data goes here
    // clientConnections[i].currentClient->write(dataToSend);
}
