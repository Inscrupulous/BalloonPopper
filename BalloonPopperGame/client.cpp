#include "client.h"
#include <QtNetwork>
#include <QNetworkProxy>
#include "messagetype.h"

//Client oriented connection side goes in this file
Client::Client(QObject *parent) : QTcpSocket (parent)
{
    this->setProxy(QNetworkProxy::NoProxy);
    connect(this, &QTcpSocket::connected, this, &Client::sendName);
    connect(this, &Client::disconnected, this, &Client::resetPlayerList);
    connect(this, &Client::readyRead, this, &Client::DataReceival);
    // Set up players for lobby
    Player* player = new Player;
    players.append(player);
    for (int i =0;i<4;i++) {
        player = new Player;
        players.append(player);
    }
    // Set up available colors
    updateColorList();
}

Client::~Client()
{
    for(auto i : players){
        delete i;
        i = nullptr;
    }
}

void Client::resetPlayerData(){
    for(auto i : players){
        i->posX = 0;
        i->score = 0;
    }
}

void Client::resetPlayerList(){
    for(auto i : players){
        // reset all players in client list to the original values
        i->color = static_cast<int>(Color::NullColor);
        i->name = "";
        i->score = 0;
        i->posX = 0;
    }
    updateColorList();
    emit colorsUpdated();
}

void Client::setColor(int i){
    // set my color then request it from the server
    myColor = i;
    sendColorRequest();
}

bool Client::isColorAvailabe(int i){
    return availableColors.contains(i);
}

void Client::SendByteArray(QByteArray &dataToSend)
{
    if(this->state() == QAbstractSocket::ConnectedState){
        this->write(dataToSend);
        this->flush();
    }
}

void Client::sendColorRequest(){
    QByteArray request;
    QTextStream out(&request, QIODevice::WriteOnly);
    out << static_cast<int>(MessageType::ColorChangeRequest) << endl
        << myId << endl << myColor << endl;
    SendByteArray(request);
}

void Client::sendName()
{
    // Send client name
    QByteArray sendName;
    QTextStream nameSend(&sendName, QIODevice::WriteOnly);
    nameSend << clientName << endl << myColor << endl;
    this->write(sendName);
    this->flush();
}

/****************************************
 *              Data Receival
 * Must pass incomingData stream into all
 * processing functions otherwise it will
 * not work!
 *
 * After incomingData stream is passed,
 * treat it like any other stream
 * i.e. cin >> char >> int;
 * **************************************/

void Client::DataReceival()
{
    // Set up stream to read from socket
    QTextStream incomingData(this);

    // What type of data is it
    incomingData >> dataOption;
    switch(dataOption)
    {
        case static_cast<int>(MessageType::UpdatePlayerName):
            ProcessPlayerData(incomingData);
            break;
        case static_cast<int>(MessageType::SetId):
            // Initial set up
            incomingData >> myId;
            if(incomingData.atEnd()) break; // if no more data don't proccess player name
            incomingData >> dataOption;
            ProcessPlayerData(incomingData);
            break;
        case static_cast<int>(MessageType::LoadGame):
            incomingData >> gameTimeLimit;
            emit startGame();
            if(!incomingData.atEnd()) return;
            break;
        case static_cast<int>(MessageType::PlayerPosChange):
            emit PlayerPosChanged(incomingData);
            break;
        case static_cast<int>(MessageType::BalloonSchedule):
            emit InitializeSchedule(incomingData);
            break;
        case static_cast<int>(MessageType::PelletAdded):
            emit PelletAdded(incomingData);
            break;
        case static_cast<int>(MessageType::UpdatePlayerScores):
            emit UpdateScores(incomingData);
            break;
        case static_cast<int>(MessageType::SendFinalScores):
            emit SendFinalScores(incomingData);
            break;
        default:
            qDebug() << "Client side no case set up";
            break;
    }
}

void Client::ProcessPlayerData(QTextStream &incomingData)
{
    incomingData >> lobbySize; // Get player amount
    if(lobbySize == 15){
        qDebug() << "Limit reached";
        serverLimitReached = true; // server already full
        return;
    }
    if(lobbySize > 1 && amHost)
        emit enoughPlayersJoined(true); // enable play button for host
    else if(lobbySize <= 1 && amHost)
        emit enoughPlayersJoined(false);

    int i;
    // Continue getting name data until there are no more names
    for(i = 0; i < lobbySize; i++)
    {
        incomingData >> players[i]->name >> players[i]->id >> players[i]->color
                >> players[i]->score;
        if(players[i]->id == myId){
            myColor = players[i]->color;
        }
    }
    for(; i<5;i++){
        // All other names are blank
        players[i]->name = "";
        players[i]->color = static_cast<int>(Color::NullColor);
    }
    // Update names in lobby and color list
    emit nameChanges();
    updateColorList();
}

void Client::updateColorList(){
    QList<int> colors;
    // Add all colors then remove any that overlap
    colors.append(static_cast<int>(Color::Red));
    colors.append(static_cast<int>(Color::Pink));
    colors.append(static_cast<int>(Color::Cyan));
    colors.append(static_cast<int>(Color::Yellow));
    colors.append(static_cast<int>(Color::Green));
    colors.append(static_cast<int>(Color::Blue));
    colors.append(static_cast<int>(Color::Purple));
    colors.append(static_cast<int>(Color::Black));

    // Remove all unavailable colors
    for(auto player : players){
        colors.removeAll(player->color);
    }
    // clear previous list and set it equal to the new list
    availableColors.clear();
    availableColors = colors;
    emit colorsUpdated(); // used to tell client that the list has changed
}
