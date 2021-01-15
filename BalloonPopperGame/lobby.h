#ifndef LOBBY_H
#define LOBBY_H

#include <QtNetwork/QTcpServer>
#include <QString>
#include "playerdata.h"

//Lobby is a class to hold ptrs to each socket on the server side.
//When one player disconnects, the lobby connects adjacent players together
//SHOULD IT HANDLE DISCONNECTIONS AS WELL?
class Lobby : public QObject // This allows it to use connect statements
{
    Q_OBJECT
public:
    Lobby();    //default constructor
    ~Lobby();
    qint16 getLobbySize() const{return numPlayers;}
    qint16 getNextID() const{return nextID;}
    bool AddPlayer(QTcpSocket* socket, QString name, int color);   //send a bool to indicate success
    bool insertPlayer(QTcpSocket* socket, QString name, int color);
    bool getPlayer(qint16 id, Player* &player); //given a name, search the list & return matching player struct
    bool deleteplayer(qint16 id);                       //not sure if necessary, given the disconnect handler
    void sendMessageToClients(QByteArray &msg);          //generic message sending function, sends data to all
    bool sendMessageToSpecificClient(qint16, QByteArray&);
    void sendMessageToAllExcept(qint16, QByteArray&);
public slots:
    void DisconnectHandler();
    void disconnectAllClients();

signals:
    void UpdateLobby();
    void removeID(int);

private:
    Player* playerOne = nullptr;  //points to the first player in the sequence
    Player* lastPlayer = nullptr;
    qint16 numPlayers = 0;  //store the number of players. SHOULD NEVER BE MORE THAN 5

    qint16 nextID = 0;     //stores the next available player ID

    bool disconnectingAll = false;


};

#endif // LOBBY_H
