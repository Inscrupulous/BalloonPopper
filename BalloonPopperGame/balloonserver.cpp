#include "balloonserver.h"
#include "ui_server.h"
#include <QString>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <QtMath>
#include <QNetworkProxy>
#include "messagetype.h"
#include "defs.h"

//constructor, inherits from QDialog
BalloonServer::BalloonServer(QWidget *parent)
    : QDialog(parent)
{
    tcpServer = new QTcpServer(this); //initialize tcp server
    tcpServer->setProxy(QNetworkProxy::NoProxy);
    lobby = new Lobby; // Create lobby
    StartServer();
    connect(tcpServer, &QTcpServer::newConnection, this, &BalloonServer::SendHelloMessage); //when a new connection is made to a client, say hi!

    //Lobby connections
    connect(lobby, &Lobby::UpdateLobby, this, &BalloonServer::SendPlayerData);
    connect(lobby,&Lobby::removeID, [=](qint16 i){
        activeIDs.removeAll(i); // Remove disconnected players ID from active list
        qDebug() << "Removing id: " << i;
    });
    // Set up available color list
    availableColors.append(static_cast<int>(Color::Red));
    availableColors.append(static_cast<int>(Color::Pink));
    availableColors.append(static_cast<int>(Color::Cyan));
    availableColors.append(static_cast<int>(Color::Yellow));
    availableColors.append(static_cast<int>(Color::Green));
    availableColors.append(static_cast<int>(Color::Blue));
    availableColors.append(static_cast<int>(Color::Purple));
    availableColors.append(static_cast<int>(Color::Black));
}

void BalloonServer::closeServer(){
    lobby->disconnectAllClients();
}

void BalloonServer::playerDataReset(){
    Player* player = nullptr;
    for(auto i : activeIDs){
        if(lobby->getPlayer(i,player)){
            player->posX = 0;
            player->score = 0;
            //reset player data
        }
    }
    scheduleCreated = false;
}

//destructor
BalloonServer::~BalloonServer()
{
    delete lobby;
}

//slot function to start server once user clicks start button
void BalloonServer::StartServer()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use  the first non-localhost IPv4 address
    //This code just finds the address that the host is using
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
    if(!tcpServer->listen(QHostAddress(ipAddress)/*/QHostAddress::LocalHost*/))
    {
        //if it fails, we exit
        qDebug() << "Server failed to open\n" << tcpServer->errorString();
    }
    return;
}

// Send updated lobby/name data to all clients
void BalloonServer::SendPlayerData(){
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    UpdateColorList();
    outgoingData << static_cast<int>(MessageType::UpdatePlayerName) << endl << lobby->getLobbySize() << endl;
    // Set up client names
    Player* player = nullptr;
    for(auto i : activeIDs){ //Loop through current ID list to set up name list
        if(lobby->getPlayer(i,player)){
            outgoingData << player->name << endl // send name
                         << player->id << endl // send id
                         << player->color << endl   // send color
                         << player->score << endl;  // send score
        }
    }
    lobby->sendMessageToClients(out); // Send message to all clients
}

void BalloonServer::sendPlayerPosData(){
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::PlayerPosChange) << endl << lobby->getLobbySize() << endl;
    // Set up client names
    Player* player = nullptr;
    for(auto i : activeIDs){ //Loop through current ID list to set up the list to send
        if(lobby->getPlayer(i,player)){
            outgoingData << player->id << endl // send id
                         << player->posX << endl; // send known pos
        }
    }
    lobby->sendMessageToClients(out); // Send message to all clients
}

void BalloonServer::SendPlayerScoreData()
{
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::UpdatePlayerScores) << endl << lobby->getLobbySize() << endl;
    // Set up client names
    Player* player = nullptr;
    for(auto i : activeIDs)         //Loop through current ID list to set up the player name list
    {
        if(lobby->getPlayer(i, player))
        {
            outgoingData << player->name << endl    // send name
                         << player->score << endl;  // send score
        }
    }
    lobby->sendMessageToClients(out);               // Send message to all clients
}

void BalloonServer::SendFinalScoreData(QTextStream& incomingData)
{
    qint16 specificID;
    incomingData >> specificID;
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::SendFinalScores) << endl << lobby->getLobbySize() << endl;
    // Set up client names
    Player* player = nullptr;
    for(auto i : activeIDs)         //Loop through current ID list to set up name list
    {
        if(lobby->getPlayer(i, player))
        {    
            outgoingData << player->name << endl    // send name
                         << player->score << endl;   // send score
        }
    }
    lobby->sendMessageToSpecificClient(specificID, out);    // Send message to one client
}

// Set up client connection. Get clients name. Update lobby
void BalloonServer::SendHelloMessage()
{
    tcpServer->pauseAccepting(); // pause accepting until this connection is initialized
    QTcpSocket* tmpSocket = tcpServer->nextPendingConnection(); // grab socket trying to connect

    //qDebug() << tmpSocket->errorString();        // NETWORKING PATCH FOR LAB
    tmpSocket->setProxy(QNetworkProxy::NoProxy);

    // The following could be done differently with a different AddPlayer method
    tmpSocket->waitForReadyRead(1000); // wait 1000ms max
    QString playerName;
    int requestedColor;
    QByteArray in = tmpSocket->readAll();   //place the client's latest messagei n a byte array
    QTextStream incomingData(&in, QIODevice::ReadOnly); //connect to a text stream
    incomingData >> playerName >> requestedColor;          //grab player name to add player
    if(availableColors.removeOne(requestedColor) && requestedColor != static_cast<int>(Color::NullColor)){ // if client requested color is available update available color list
        UpdateColorList();
    }
    else {
        requestedColor = availableColors.takeFirst();
    }
    if(lobby->AddPlayer(tmpSocket, playerName, requestedColor)){ // if successful set up connections
        connect(tmpSocket, &QTcpSocket::readyRead, [=]{
            DataReceival(tmpSocket);
        });
        connect(tmpSocket, &QTcpSocket::disconnected, lobby, &Lobby::DisconnectHandler);
        connect(tmpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                [=](QAbstractSocket::SocketError socketError){ // if an error occurred to the socket print to debug
            qDebug() << socketError;
        });

        // Set up clients ID
        activeIDs.append(lobby->getNextID()-1);
        QByteArray out;
        QTextStream idSend(&out, QIODevice::WriteOnly);
        idSend << static_cast<int>(MessageType::SetId) << endl << lobby->getNextID()-1 << endl;
        // Only the current client will receive this message
        tmpSocket->write(out);
        tmpSocket->flush();
        SendPlayerData();
    }
    else {
        QByteArray deny;
        QTextStream denialStream(&deny,  QIODevice::WriteOnly);
        denialStream << static_cast<int>(MessageType::UpdatePlayerName) << endl << 15 << endl;
        tmpSocket->write(deny);
        tmpSocket->flush();
        tmpSocket->disconnectFromHost();
        tmpSocket->deleteLater();
    }
    tcpServer->resumeAccepting();
}

/*******************************************
 *              Data Receival
 * For any function that processes data
 * You must first pass the incomingData
 * stream.
 * i.e. myProcessDataFunction(incomingData)
 * *****************************************/

// Grab the socket that emitted a ready read signal and send the data where it needs to go
void BalloonServer::DataReceival(QTcpSocket *socket)
{
    QTextStream incomingData(socket);
    incomingData >> dataOption;
    switch(dataOption)
    {
    case static_cast<int>(MessageType::StartGameRequest):
        StartGame(incomingData);
        break;
    case static_cast<int>(MessageType::ColorChangeRequest):
        ColorChangeRequest(incomingData);
        break;
    case static_cast<int>(MessageType::ShootPellet):
        pelletAdded(incomingData);
        break;
    case static_cast<int>(MessageType::Moved):
        playerMoved(incomingData);
        break;
    case static_cast<int>(MessageType::IncPlayerScore):
        IncScore(incomingData);
        break;
    case static_cast<int>(MessageType::SendFinalScores):
        SendFinalScoreData(incomingData);
        break;
    case static_cast<int>(MessageType::RequestBalloonSchedule):
        CreateAndSendSchedule(incomingData);
        break;
    default:
        qDebug() << "Server side No case statement set up";
        break;
    }
}

void BalloonServer::DataReceival(QTextStream &incomingData) // Catch any left over data
{
    incomingData >> dataOption;
    if(incomingData.atEnd()) return;
    switch(dataOption)
    {
    case static_cast<int>(MessageType::ColorChangeRequest):
        ColorChangeRequest(incomingData);
        break;
    case static_cast<int>(MessageType::ShootPellet):
        pelletAdded(incomingData);
        break;
    case static_cast<int>(MessageType::Moved):
        playerMoved(incomingData);
        break;
    case static_cast<int>(MessageType::IncPlayerScore):
        IncScore(incomingData);
        break;
    case static_cast<int>(MessageType::SendFinalScores):
        SendFinalScoreData(incomingData);
        break;
    case static_cast<int>(MessageType::RequestBalloonSchedule):
        CreateAndSendSchedule(incomingData);
        break;
    default:
        qDebug() << "Server side no case statement set up\nData option received: " << dataOption;
        break;
    }
}

void BalloonServer::pelletAdded(QTextStream& incomingData){
    qint16 ID;
    QString color = "0";
    int pos;
    incomingData >> ID >> pos; // get player ID and pos

    //Now formulate reply
    QByteArray out;
    QTextStream reply(&out, QIODevice::WriteOnly);

    // We are only going to send the color for the return
    Player* player = nullptr;
    if(lobby->getPlayer(ID, player)){
        switch(player->color){
            case static_cast<int>(Color::Red):
                color = "Red";
                break;
            case static_cast<int>(Color::Pink):
                color = "Pink";
                break;
            case static_cast<int>(Color::Cyan):
                color = "Cyan";
                break;
            case static_cast<int>(Color::Yellow):
                color = "Yellow";
                break;
            case static_cast<int>(Color::Green):
                color = "Green";
                break;
            case static_cast<int>(Color::Blue):
                color = "Blue";
                break;
            case static_cast<int>(Color::Purple):
                color = "Purple";
                break;
            case static_cast<int>(Color::Black):
                color = "Black";
                break;
        }
    }

    reply << static_cast<int>(MessageType::PelletAdded) << endl
          << ID << endl // may not need if not just empty read it
          << color << endl // will be used for color of pellet
          << pos << endl; // position to spawn in
    lobby->sendMessageToAllExcept(ID,out); // send to all other clients
    if(!incomingData.atEnd()) DataReceival(incomingData);
}

void BalloonServer::playerMoved(QTextStream &incomingData){
    qint16 ID;
    qreal pos = 0;
    incomingData >> ID >> pos; // get player ID and pos

    // Set new player coords
    Player* player = nullptr;
    if(lobby->getPlayer(ID, player)){
        player->posX = pos; // update player pos
    }
    sendPlayerPosData();
    if(!incomingData.atEnd()) DataReceival(incomingData);
}


/* Sends the following values to all clients except host/player1
 *      MessageType::LoadGame
 *      time => number of seconds the game will take
 * */
void BalloonServer::StartGame(QTextStream &incomingData){
    // Grab host set time
    int time;
    incomingData >> time;
    gameTimeLimit = time;
    // Create message
    QByteArray out;
    QTextStream outgoingRequest(&out, QIODevice::WriteOnly);
    outgoingRequest << static_cast<int>(MessageType::LoadGame) << endl
                    << time << endl;
    int skip = 0;
    // Send to all except host
    for(auto i : activeIDs){
        if(skip != 0){ // skip first player(should always be host)
            lobby->sendMessageToSpecificClient(i,out);
        }
        else skip++;
    }
    if(!incomingData.atEnd()) DataReceival(incomingData);
}

void BalloonServer::ColorChangeRequest(QTextStream &incomingData){
    int color;
    qint16 id;
    incomingData >> id;
    incomingData >> color;

    // Check if color is available and if so update player data
    if(availableColors.removeOne(color)){
        Player* player;
        if(lobby->getPlayer(id,player)){
            player->color = color;
        }
    }
    SendPlayerData();
    if(!incomingData.atEnd()) DataReceival(incomingData);
}

void BalloonServer::UpdateColorList(){
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
    Player* player;
    for(auto i : activeIDs){
        if(lobby->getPlayer(i,player)){
            colors.removeAll(player->color);
        }
    }
    // clear previous list and set it equal to the new list
    availableColors.clear();
    availableColors = colors;
}

void BalloonServer::CreateAndSendSchedule(QTextStream &incomingData){
    qint16 ID;
    incomingData >> ID;
    int v, w, c, d;
    double z;
    QList<int> Times;
    QList<int> BalloonID;
    QList<int> BalloonColors;
    QList<int> XCoordinates;
    QList<int> YCoordinates;
    QList<double> Trajectories;

    // Randomly populate Times QList based off of selected time limit
    for (int i = gameTimeLimit; i > 0; i--) {
        v = QRandomGenerator::global()->bounded(5, 10);
        for (int j = v; j > 0; j--) {
            w = QRandomGenerator::global()->bounded(((j-1)*1000)/v, ((j*1000)/v)-1);
            Times.append((i*1000) + w);
        }
    }

    // Populate BalloonID with unique ID value for each Balloon
    c = Times.count();
    for (int i = 0; i < c; i++) {
        BalloonID.append(i);
    }

    // Populate BalloonColors with a relatively equal number of all colors
    c = Times.count();
    d = floor(c/8);
    for(int i = 0; i < d; i++) {
        BalloonColors.append(static_cast<int>(Color::Red));
        BalloonColors.append(static_cast<int>(Color::Pink));
        BalloonColors.append(static_cast<int>(Color::Cyan));
        BalloonColors.append(static_cast<int>(Color::Yellow));
        BalloonColors.append(static_cast<int>(Color::Green));
        BalloonColors.append(static_cast<int>(Color::Blue));
        BalloonColors.append(static_cast<int>(Color::Purple));
        BalloonColors.append(static_cast<int>(Color::Black));
    }
    if((d*8) != c) {
        for (int i = 0; i < (c-(d*8)); i++) {
            v = QRandomGenerator::global()->bounded(7);
            if(v == 0) {BalloonColors.append(static_cast<int>(Color::Red));}
            else if(v == 1) {BalloonColors.append(static_cast<int>(Color::Pink));}
            else if(v == 2) {BalloonColors.append(static_cast<int>(Color::Cyan));}
            else if(v == 3) {BalloonColors.append(static_cast<int>(Color::Yellow));}
            else if(v == 4) {BalloonColors.append(static_cast<int>(Color::Green));}
            else if(v == 5) {BalloonColors.append(static_cast<int>(Color::Blue));}
            else if(v == 6) {BalloonColors.append(static_cast<int>(Color::Purple));}
            else {BalloonColors.append(static_cast<int>(Color::Black));}
        }
    }

    // Populate XCoordinates with random x coordinates to spawn balloons at
    for (int i = 0; i < c; i++) {
        v = QRandomGenerator::global()->bounded(XMAX);
        XCoordinates.append(v);
    }

    // Populate YCoordinates with random y coordinates to spawn balloons at
    for (int i = 0; i < c; i++) {
        v = QRandomGenerator::global()->bounded(-YMAX+60, -YMAX+110);
        YCoordinates.append(v);
    }

    // Populate Trajectories with random trajectories based on associate x coordinate
    for (int i = 0; i < c; i++) {
        if(XCoordinates[i] < 160) {
            z = QRandomGenerator::global()->bounded(70, 90);

            Trajectories.append(z);
        }
        else if (XCoordinates[i] >= 160 && XCoordinates[i] < 480) {
            z = QRandomGenerator::global()->bounded(70, 110);
            Trajectories.append(z);
        }
        else {
            z = QRandomGenerator::global()->bounded(90, 110);
            Trajectories.append(z);
        }
    }

    // Send Schedule to Clients
    if(scheduleCreated){
        lobby->sendMessageToSpecificClient(ID,Schedule);
        return;
    }
    scheduleCreated = true;
    QTextStream outgoingData(&Schedule, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::BalloonSchedule) << endl << Times.count() << endl;
    for (int i = 0; i < c; i++) {
        outgoingData << Times[i] << endl // send spawn time
                     << BalloonID[i] << endl // send Balloon ID
                     << BalloonColors[i] << endl // send color
                     << XCoordinates[i] << endl // send spawn coordinate
                     << YCoordinates[i] << endl // send y coordinate for spawn
                     << Trajectories[i] << endl; // send trajectory
    }
    lobby->sendMessageToSpecificClient(ID, Schedule);
}

void BalloonServer::IncScore(QTextStream &incomingData)
{
    qint16 ID;
    int score;
    incomingData >> ID >> score; // get player ID and score

    // Set player score
    Player* player;
    if(lobby->getPlayer(ID, player))
    {
        player->score = score; // update player score
    }
    SendPlayerScoreData();
    if(!incomingData.atEnd()) DataReceival(incomingData);
}
