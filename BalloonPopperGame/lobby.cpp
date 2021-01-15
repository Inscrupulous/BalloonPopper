#include <lobby.h>
#include <QTcpSocket> // if not included cant do socket->state()
Lobby::Lobby(){

}

Lobby::~Lobby(){

}
//if the lobby is not full, try to add a new player
//creates new player object using given socket, name.
//generates unique id to give to player
bool Lobby::AddPlayer(QTcpSocket* socket, QString name, int color)
{
    //qDebug() << "adding player with name: " << name;
    if(numPlayers >= 5)             //if lobby is full
        return false;               //failed to add player to the lobby

    //set up new player object
    Player* player = new Player;    //make new player object
    player->name = name;            //initialize name and socket ptr
    player->client = socket;        //place socket into the player object
    player->color = color;
    player->id = nextID++;          //set id & increment id counter
    if(playerOne == nullptr)        //if the lobby is empty
    {
        playerOne = player;         //this new player becomes playerOne
    }
    if(lastPlayer != nullptr)
    {
        lastPlayer->nextPlayer = player;//the last player in the list now points to the new player
    }
    lastPlayer = player;            //set the lastPlayer pointer to the new last player

    numPlayers++;   //increment counter
    return true;    //return true if the player was added
}

//search through the list until a player is found, matching the id given
//the socket of the player is returned via reference parameter to
//the socket's pointer.

//returns true if found, false if not
bool Lobby::getPlayer(qint16 id, Player* &returnPlayer)
{
    Player* iterator = playerOne;   //pointer to iterate through the list

    while(iterator != nullptr)      //loop until the end of the list
    {
        if(iterator->id == id)      //if we find it (NO DUPLICATE ID's)
        {
            returnPlayer = iterator;//place socket in reference parameter
            return true;                    //return true
        }

        iterator = iterator->nextPlayer;    //move iterator to next player
    }

    return false;                   //returns false if no player was found
}

bool deletePlayer()
{
    /* Implement if necessary */
    return true;
}

//Takes in a byte array of a pre-assembled message.
//loops through the list of clients, sending the message to each one
void Lobby::sendMessageToClients(QByteArray &msg)
{
    Player* iterator = playerOne;

    while(iterator != nullptr) //loop through each player
    {
        //send the message to the player
        if(iterator->client->state() == QAbstractSocket::ConnectedState)
        {
            iterator->client->write(msg); //write the message to the player's socket
            iterator->client->flush();
        }

        iterator = iterator->nextPlayer; //go to next player
    }
}

//returns true if the message sent successfully
//if the player was not found in the list, returns false
bool Lobby::sendMessageToSpecificClient(qint16 id, QByteArray &msg)
{
    Player* iterator = playerOne;   //pointer to iterate through the list

    while(iterator != nullptr)      //loop until the end of the list
    {
        if(iterator->id == id)      //if we find it (NO DUPLICATE ID's)
        {
            iterator->client->write(msg); //write the message to the player's socket
            iterator->client->flush();
            return true;                    //return true
        }

        iterator = iterator->nextPlayer;    //move iterator to next player
    }
    return false;
}

void Lobby::sendMessageToAllExcept(qint16 id, QByteArray &msg)
{
    Player* iterator = playerOne;

    while(iterator != nullptr) //loop through each player
    {
        //send the message to the player
        if(iterator->client->state() == QAbstractSocket::ConnectedState && iterator->id != id) // only send if not excluded
        {
            iterator->client->write(msg); //write the message to the player's socket
            iterator->client->flush();
        }

        iterator = iterator->nextPlayer; //go to next player
    }
}

// Disconnect Handler to be implemented *HAVENT TESTED

//what it does:
//if p1 disconnects, set p2 as new p1 and remove p1
//if another player
void Lobby::DisconnectHandler(){
    if(disconnectingAll) return; // if this disconnect is from disconnecting all clients then do nothing
    Player* iterator = playerOne;   //pointer to iterate through the list
    Player* previousConnectedPlayer = playerOne;
    Player* deletionPtr = nullptr;

    while(iterator != nullptr)      //loop until the end of the list
    {
        if(iterator->client != nullptr)      // make sure tcpSocket isn't empty
        {
            if(iterator->client->state() == QAbstractSocket::UnconnectedState){
                if(iterator == playerOne){
                    playerOne = iterator->nextPlayer;   //set the next player in lsit to new p1
                    delete iterator;                    //deallocate memory from previous p1
                    return;
                }
                previousConnectedPlayer->nextPlayer = iterator->nextPlayer; // connect the previous to the next
                qDebug() << "Player that is being deleted: " << iterator->name;
                deletionPtr = iterator; // save pointer to delete
                --numPlayers; // decrease lobby count
                emit removeID(iterator->id);
            }
            else {
                previousConnectedPlayer = iterator;    // This player is still connected so move up
                lastPlayer = previousConnectedPlayer; // last player should always be at the end of the connected player list
            }
        }
        iterator = iterator->nextPlayer;    //move iterator to next player
        if(deletionPtr != nullptr){
            delete deletionPtr;      // delete data
            deletionPtr = nullptr;   // safe guard
        }
    }
    if(iterator != playerOne){
        emit UpdateLobby(); // signal that the lobby has been updated
    }
}

//Disconnects all clients currently in the lobby (host disconnects last)
void Lobby::disconnectAllClients(){
    disconnectingAll = true; // tell disconnect handler that these disconnects are being taken care of
    QList<Player*> pList; // List for deleting pointers
    pList.append(playerOne); // add playerOne
    if(playerOne && playerOne->nextPlayer){ // playerOne is not NULL and nextPlayer is not NULL
        Player* iterator = playerOne->nextPlayer;   //pointer to iterate through the list
        while(iterator != nullptr)      //loop until the end of the list
        {
            iterator->client->disconnectFromHost(); // disconnect current client
            pList.append(iterator); // add player to deletion list
            iterator = iterator->nextPlayer;    //move iterator to next player
        }
        playerOne->client->disconnectFromHost(); // after whole lobby is disconnected, disconnect host
    }
    else if(playerOne){ // playerOne is not NULL but nextPlayer is NULL (i.e. only one player in lobby)
        playerOne->client->disconnectFromHost(); // disconnect player one
    }
    for(auto player : pList){
        delete player; // loop through deletion list to delete all used memory
    }
    playerOne = nullptr; // reset variables accordingly
    lastPlayer = nullptr;
    disconnectingAll = false;
}
