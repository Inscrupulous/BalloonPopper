#include "gameplaydialog.h"
#include "ui_gameplaydialog.h"
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <typeinfo>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QList>
#include "messagetype.h"
#include "timer.h"
#include "defs.h"
#include "pellet.h"
#include "playerdata.h"

GamePlayDialog::GamePlayDialog(Client* c, MainWindow *lobby, int secs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GamePlayDialog)
{
  ui->setupUi(this);
  client = c; // save client socket object for communication
  // Hide Lobby screen and save it
  this->show();
  lobbyScreen = lobby;
  ui->countdown->setText(ui->countdown->text() + QString::number(secs)); // Set up countdown label
  //Connect shooting timer
  shootingTimer = new QTimer(this);
  shootingTimer->setInterval(1000/2); // set pellets per sec
  shootingTimer->setSingleShot(true);
  connect(shootingTimer, &QTimer::timeout, this, &GamePlayDialog::shootingCheck);

  //Connect generic key check timer
  gameUpdateTimer = new QTimer(this);
  connect(gameUpdateTimer, &QTimer::timeout, this, &GamePlayDialog::gameUpdate);

  //Connect timer to slot so it gets updated
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdown()));

  // set up client info and connects
  myID = client->getMyID();
  connect(client, &Client::PelletAdded, this, &GamePlayDialog::pelletAdd); // connection for pellet adding
  connect(client, &Client::PlayerPosChanged, this, &GamePlayDialog::playerPosUpdate); // connection for updating pos
  connect(client, &Client::InitializeSchedule, this, &GamePlayDialog::InitializeSchedule); // connection for balloon spawning
  connect(client, &Client::disconnected, lobby, &MainWindow::show);
  connect(client, &Client::disconnected, this, &GamePlayDialog::DisconnectHandler);
  connect(client, &Client::UpdateScores, this, &GamePlayDialog::UpdateScores);

  //Request balloon spawn schedule from server
  QByteArray req;
  QTextStream request(&req, QIODevice::WriteOnly);
  request << static_cast<int>(MessageType::RequestBalloonSchedule) << endl << myID << endl;
  client->SendByteArray(req);

  //It is started with a value of 1000 milliseconds, indicating that it will time out every second.
  timer->start(1000);
  SecondsToEnd = secs;
  initialTime = secs;

  // Scene setup
  scene = new QGraphicsScene(-XMAX/2+500, -YMAX/2, XMAX, YMAX, this); // set up coordinates
  connect(gameUpdateTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
  scene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  ui->graphicsView->setScene(scene);
  QString tmpColor;
  // Add Tanks to Game Window
  for(int i = 0; i<5; i++){
      client->getPlayerName(i);
      switch(client->getPlayerColor(i)){
        case static_cast<int>(Color::Red):
            tank = new Tank("Red");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Red";
            break;
        case static_cast<int>(Color::Pink):
            tank = new Tank("Pink");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Pink";
            break;
        case static_cast<int>(Color::Cyan):
            tank = new Tank("Cyan");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Cyan";
            break;
        case static_cast<int>(Color::Yellow):
            tank = new Tank("Yellow");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Yellow";
            break;
        case static_cast<int>(Color::Green):
            tank = new Tank("Green");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Green";
            break;
        case static_cast<int>(Color::Blue):
            tank = new Tank("Blue");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Blue";
            break;
        case static_cast<int>(Color::Purple):
            tank = new Tank("Purple");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor= "Purple";
            break;
        case static_cast<int>(Color::Black):
            tank = new Tank("Black");
            tank->setPos(0, 0);
            scene->addItem(tank);
            tmpColor = "Black";
            break;
      }
      if(tank != nullptr){ // if tank was actually allocated
          tank->setID(client->getPlayerID(i)); // set the tanks id
          tanks.append(tank); // append the tank to the list
          if(tank->getID() == myID){
              clientColor = tmpColor;
              myTank = tank;
          }
          tank = nullptr; // reset the variable
      }      
  }
  // Set the gameplay window as the keyboard focus
  // without this, arrow keys are accepted in the end dialog
  this->setFocus();
}

GamePlayDialog::~GamePlayDialog()
{
    delete ui;
    for(auto i : tanks){
        i->deleteLater();
    }
}

void GamePlayDialog::DisconnectHandler(){
    disconnect(client, nullptr, this, nullptr); //For redundancy
}

void GamePlayDialog::sendPelletRequest(qreal xpos){
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::ShootPellet) << endl // add message type
                 << myID << endl << xpos << endl; // send my ID and x pos
    client->SendByteArray(out);
}

void GamePlayDialog::sendPlayerPos(qreal xpos){
    QByteArray out;
    QTextStream outgoingData(&out, QIODevice::WriteOnly);
    outgoingData << static_cast<int>(MessageType::Moved) << endl // add message type
                 << myID << endl << xpos << endl; // send my ID and x pos
    client->SendByteArray(out);
}

void GamePlayDialog::shootingCheck(){
    if(spaceWasReleased && shotProcessed){ // if space was released and the shot was processed
        keysPressed -= Qt::Key_Space;
    }
}

void GamePlayDialog::gameUpdate(){ // Check what keys are pressed and act accordingly
    //Check for balloon spawning
    if(prevTime >= (SecondsToEnd*1000)+timer->remainingTime()//If the previous time was greater than the remaining time, spawn a balloon
            && balloons.count() < 10/*max balloons allowed*/){ // if there are less than ten balloons on screen
        spawn();
    }
    //Check for movement and shooting
    if(keysPressed.contains(Qt::Key_Space) && !shootingTimer->isActive()){ // If player is trying to shoot and the correct time has passed
        qreal x = myTank->scenePos().x();
        sendPelletRequest(x);
        pelletAddThis(clientColor, x);
        shootingTimer->start(); // start timer so player can't shoot for a while
        shotProcessed = true; // A shot was processed
    }
    if(keysPressed.contains(Qt::Key_A) || keysPressed.contains(Qt::Key_Left)){
        // move tank left
        if (myTank->scenePos().x() > 0/*-XMAX/2*/)
        {
            myTank->setPos(myTank->scenePos().x() - 2.0, myTank->scenePos().y());
            sendPlayerPos(myTank->scenePos().x());
        }
        // can't leave screen
    }
    else if(keysPressed.contains(Qt::Key_D) || keysPressed.contains(Qt::Key_Right)){ // check for right movement only if currently not holding left
        // move tank right
        if (myTank->scenePos().x() < XMAX+55)
        {
            myTank->setPos(myTank->scenePos().x() + 2.0, myTank->scenePos().y());
            sendPlayerPos(myTank->scenePos().x());
        }
    }
}

void GamePlayDialog::keyPressEvent(QKeyEvent * event){
    if(event) keysPressed += event->key(); // add key that was pressed
    if(event && event->key() == Qt::Key_Space){
        shotProcessed = false;
        spaceWasReleased = false;
    }
    QDialog::keyPressEvent(event);
}

void GamePlayDialog::keyReleaseEvent(QKeyEvent *event){
    if(event){
        if(event->key() == Qt::Key_Space){ // If player released space
            if(shotProcessed){
                keysPressed -= Qt::Key_Space; // if shot was processed then remove space from the list
            }
            else {
                spaceWasReleased = true; // if shot wasn't processed then space needs to be removed later
            }
        }
        else {
            keysPressed -= event->key(); // remove key that was releasedd
        }
    }
    QDialog::keyReleaseEvent(event);
}

bool GamePlayDialog::eventFilter(QObject *watched, QEvent *event)
{   // to catch any unexpected/unaccounted for key press events - send to keyPressEvent handler
    // and if still not handled, passes it to inherited/original handler
    if (watched == ui->graphicsView){
          if (event->type() == QEvent::KeyPress) {
              QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
              GamePlayDialog::keyPressEvent(keyEvent);  // if key is pressed, call keyPressEvent handler
              return true;
          }
          return false;
      }
    else return QWidget::eventFilter(watched, event); // pass the event on to the parent class
}

void GamePlayDialog::updateCountdown()
{
  //do something along the lines of ui->countdown->setText(....);
  --SecondsToEnd;
  if(SecondsToEnd >= 0)
  {
      QString time = QString::number(SecondsToEnd);
      ui->countdown->setText("Seconds Left: " + time ); // Have not added this as it may grab focus
  }
  if(SecondsToEnd == 0) // stop game activity
  {
      if(shootingTimer->isActive()){
          shootingTimer->stop();
      }
      gameUpdateTimer->stop();
      gameUpdateTimer->setSingleShot(true);
  }
  else if (SecondsToEnd == -1){ // Make sure all data has been received then go to end game screen
      timer->stop(); // stop countdown timer
      timer->setSingleShot(true);
      GameEndDialog* end = new GameEndDialog(lobbyScreen, client, initialTime);
      end->show();
      this->hide();
  }
}

void GamePlayDialog::pelletAddThis(QString color, qreal x){
    pellet* p = new pellet(color);
    p->setPos(x,myTank->scenePos().y()-45);
    scene->addItem(p);
    connect(p, &pellet::poppedOne, this, &GamePlayDialog::BalloonHit);
    connect(p, &pellet::poppedOneSameColor, this, &GamePlayDialog::BalloonSameColorHit);
}

void GamePlayDialog::pelletAdd(QTextStream &incomingData){
    // Data order is:
    // ID(int), Color(String), Pos(qreal)
    int ID = 0;
    QString color = "0";
    qreal xpos;
    qreal ypos = myTank->scenePos().y() - 45; // y coordinate will always be the same no?

    incomingData >> ID >> color >> xpos;
    Pellet = new pellet(color);
    Pellet->setPos(xpos,ypos);
    scene->addItem(Pellet);
    connect(Pellet, &pellet::poppedOne, this, &GamePlayDialog::BalloonHit);
    connect(Pellet, &pellet::poppedOneSameColor, this, &GamePlayDialog::BalloonSameColorHit);
    if(!incomingData.atEnd()) dataRec(incomingData);
}

void GamePlayDialog::playerPosUpdate(QTextStream &incomingData){
    // Data order is:
    // lobbySize(int only once), id(int), score(int), Xpos(qreal)
    int ID;
    qreal xpos;
    qreal ypos = myTank->scenePos().y(); // stays the same no?
    int lobbySize = 0;

    incomingData >> lobbySize; // grab the lobby size and update those needing it
    for(int i = 0; i<lobbySize; i++){
        incomingData >> ID >> xpos;
        for(auto t : tanks){ // search for tank that needs to be moved
            if(t->getID() == ID && t->getID() != myID){
               t->setPos(xpos,ypos);
            }
        }
    }
    if(!incomingData.atEnd()) dataRec(incomingData);
}
void GamePlayDialog::spawn(){
    //Spawn a balloon and make connections
    prevTime = Times.takeFirst();
    Balloon* balloon = new Balloon(BalloonColors.takeLast(), Trajectories.takeLast());
    balloons.append(balloon);
    connect(balloon, &Balloon::deleteBalloon, this, &GamePlayDialog::BalloonDelete);
    balloons.back()->setPos(XCoordinates.takeLast(), YCoordinates.takeLast()); // The end/back of the list is the one we just added
    scene->addItem(balloons.back());
}

//receives
void GamePlayDialog::InitializeSchedule(QTextStream &incomingData){
    // Data order:
    int c;
    incomingData >> c; // get size of Lists
    int Time;
    int ID;
    int Color;
    int Xcoord;
    int Ycoord;
    double traj;
    for (int i = 0; i<c; i++) {
       incomingData >> Time >> ID >> Color>> Xcoord >> Ycoord >> traj;
       Times.append(Time);
       BalloonID.append(ID);
       BalloonColors.append(Color);
       XCoordinates.append(Xcoord);
       YCoordinates.append(Ycoord);
       Trajectories.append(traj);
    }

    prevTime = Times.takeFirst(); //set up first spawn time
    gameUpdateTimer->start(1000/120); // start game update timer to register movements and spawning
}

void GamePlayDialog::BalloonSameColorHit(QString color){
    balloons.takeFirst();
    if(clientColor == color){
        score -= 2;
        QByteArray out;
        QTextStream outgoingData(&out, QIODevice::WriteOnly);
        outgoingData << static_cast<int>(MessageType::IncPlayerScore) << endl
                     << myID << endl << score << endl; // send my ID and score
        client->SendByteArray(out);
    }
}

void GamePlayDialog::BalloonHit(QString color)
{
    balloons.takeFirst(); // remove one from list so more will spawn
    if(clientColor == color){
        score++;
        QByteArray out;
        QTextStream outgoingData(&out, QIODevice::WriteOnly);
        outgoingData << static_cast<int>(MessageType::IncPlayerScore) << endl
                     << myID << endl << score << endl; // send my ID and score
        client->SendByteArray(out);
    }
}

// Reimplemented data receival function for any data received but no ready read was emitted
void GamePlayDialog::dataRec(QTextStream &data){
    if(data.atEnd()) return;
    unsigned short dataOption;
    data >> dataOption;
    if(data.atEnd()) return;
    switch(dataOption){
    case static_cast<int>(MessageType::PlayerPosChange):
        playerPosUpdate(data);
        break;
    case static_cast<int>(MessageType::PelletAdded):
        pelletAdd(data);
        break;
    case static_cast<int>(MessageType::UpdatePlayerScores):
        UpdateScores(data);
        break;
    case static_cast<int>(MessageType::SendFinalScores):
        client->SendFinalScoresSignal(data); // Should never receive request inside this class
        break;
    }
    dataRec(data);
}

void GamePlayDialog::BalloonDelete(int ID){ // ID will be used to remove a specific balloon from the list *NOT IMPLEMENTED*
    balloons.takeLast();
}

void GamePlayDialog::UpdateScores(QTextStream &incomingData)
{
    int lobbySize;
    QString name;
    int score;
    incomingData >> lobbySize;
    for(int i = 0; i < lobbySize; i++)
    {
        incomingData >> name >> score;
        switch(i)
        {
        case 0:
            ui->player1Name->setText(name + ":");
            ui->player1Score->setNum(score);
            break;
        case 1:
            ui->player2Name->setText(name + ":");
            ui->player2Score->setNum(score);
            break;
        case 2:
            ui->player3Name->setText(name + ":");
            ui->player3Score->setNum(score);
            break;
        case 3:
            ui->player4Name->setText(name + ":");
            ui->player4Score->setNum(score);
            break;
        case 4:
            ui->player5Name->setText(name + ":");
            ui->player5Score->setNum(score);
            break;
        default:
            qDebug() << "There was an error updating in-game scores";
            break;
        }
    }
    if(!incomingData.atEnd()) dataRec(incomingData);
}
