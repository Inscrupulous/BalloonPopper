#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hscoresdialog.h"
#include "hostnamedialog.h"
#include "playernamedialog.h"
#include "customizedialog.h"
#include "balloonserver.h"
#include "messagetype.h"
#include "gameplaydialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusLabel = new QLabel("Welcome to Pop The Balloons! Click Host or Join to get started");
    QFont f("Arial", 10, QFont::Bold);
    statusLabel->setFont(f);
    statusBar()->addWidget(statusLabel);

    helpAction = new QAction("Help", this);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    ui->mainToolBar->addAction(helpAction);

    client = new Client(this);
    cd = new CustomizeDialog(client, this);
    timerPointer = new Timer(this);

    ui->PlayButton->setEnabled(false);
    ui->SelectTimeButton->setEnabled(false);

    // Connect Statements for each Push Button on the main menu.
    connect(ui->HostButton, SIGNAL(clicked(bool)), this, SLOT(Host()));
    connect(ui->JoinButton, SIGNAL(clicked(bool)), this, SLOT(Join()));
    connect(ui->SelectTimeButton, SIGNAL(clicked(bool)), this, SLOT(SelectTime()));
    connect(ui->SelectColorButton, SIGNAL(clicked(bool)), this, SLOT(SelectColorWindow()));
    connect(ui->HighScoreButton, SIGNAL(clicked(bool)), this, SLOT(ShowHighScores()));
    connect(ui->PlayButton, SIGNAL(clicked(bool)), this, SLOT(Play()));
    connect(ui->ExitMenuButton, SIGNAL(clicked(bool)), this, SLOT(ExitMenu()));

    //Connect customize
    connect(client, &Client::colorsUpdated, cd, &CustomizeDialog::updateColorList);

    // Set up client
    connect(client, &Client::nameChanges, this, &MainWindow::PlayerNameChange);
    connect(client, &Client::startGame, this, &MainWindow::Play);
    // Handle client disconnect
    connect(client, &Client::disconnected, this, &MainWindow::DisconnectHandle);

    hs = new HScoresDialog(this);
}

void MainWindow::showHelp(){
    msgBox.setText(          "Click Host Game or Join Game to get started.\n"
                             "A lobby must have at least two players in order to play.\n"
                             "Shooting a balloon of a color the same as your own will decrease your score by two.\n"
                             "Shooting a balloon of a color other than your own will increase your score by one.\n"
                             "Whoever has the highest score at the end of the game wins.\n\n\n"
                             "\t---Controls---\n\n"
                             " Move left: \tA or Left Arrow Key\n\n"
                             "Move right: \tD or Right Arrow Key\n\n"
                             "     Shoot: \tSpace bar");
    msgBox.show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Host()
{
    HostNameDialog* hn = new HostNameDialog;
    hn->show();
    connect(hn, &HostNameDialog::rejected, [=]{
        ui->JoinButton->setEnabled(true);
        ui->HostButton->setEnabled(true);
        hn->deleteLater();
    });
    ui->JoinButton->setEnabled(false);
    ui->HostButton->setEnabled(false);
    // Make connection to server
    connect(hn, &HostNameDialog::submitClicked, this, &MainWindow::HostSubmitted);
    connect(hn, &HostNameDialog::submitClicked, [=]{
        hn->deleteLater();
    });
    connect(hn, &HostNameDialog::closeClicked, [=]{
        ui->JoinButton->setEnabled(true);
        ui->HostButton->setEnabled(true);
        hn->deleteLater();
    });
}

// Set up server and connect host
void MainWindow::HostSubmitted(QString name)
{
    bs = new BalloonServer(this);
    // client is host
    client->setHost(true);
    ui->SelectTimeButton->setEnabled(true);
    // set name and connect
    client->setName(name);
    client->connectToHost(bs->getIP(), bs->getPort());
    client->waitForConnected(3000);
    if(client->state() == QAbstractSocket::ConnectedState){
        // Disable join buttons
        ui->JoinButton->setEnabled(false);
        ui->HostButton->setEnabled(false);
        statusLabel->setText("Connected\tIP: " + bs->getIP().toString() + "\tPort: " + QString::number(bs->getPort()));
        ui->ExitMenuButton->setText("Disconnect");
    }
    else{
        qDebug() << client->state();
        statusLabel->setText("Connection Failed!");
        ui->JoinButton->setEnabled(true);
        ui->HostButton->setEnabled(true);
        bs->deleteLater();
    }
    connect(client, &Client::enoughPlayersJoined, ui->PlayButton, &QPushButton::setEnabled);
}

void MainWindow::Join()
{
    // Create & open window for player to enter name
    PlayerNameDialog* pn = new PlayerNameDialog;
    pn->show();
    connect(pn, &PlayerNameDialog::rejected, [=]{
        ui->JoinButton->setEnabled(true);
        ui->HostButton->setEnabled(true);
        pn->deleteLater();
    });
    ui->JoinButton->setEnabled(false);
    ui->HostButton->setEnabled(false);

    client->setHost(false);
    // Make connection to server
    connect(pn, &PlayerNameDialog::ValidInput, [=]{
        client->setName(pn->getPlayerName());
        client->connectToHost(pn->getIP(), pn->getPort());
        client->waitForConnected(3000);

        // Free up some space
        pn->deleteLater();

        if(client->state() == QAbstractSocket::ConnectedState){
            // Disable join buttons
            ui->JoinButton->setEnabled(false);
            ui->HostButton->setEnabled(false);
            statusLabel->setText("Connected");
            ui->ExitMenuButton->setText("Disconnect");
        }
        else{
            qDebug() << client->state();
            statusLabel->setText("Connection Failed!");
            ui->JoinButton->setEnabled(true);
            ui->HostButton->setEnabled(true);
        }
    });
    connect(pn, &PlayerNameDialog::closeClicked, [=]{
        ui->JoinButton->setEnabled(true);
        ui->HostButton->setEnabled(true);
        pn->deleteLater();
    });
}

void MainWindow::SelectTime()
{
    // What happens when the time button is clicked
    timerPointer->show();
}

void MainWindow::SelectColorWindow()
{
    cd->show();
}

void MainWindow::ShowHighScores()
{
    hs->UpdateHScores();
    hs->show();
}

void MainWindow::Play()
{
    msgBox.close();
    cd->close();
    hs->close();
    // What happens when the play button is clicked  
    if(client->isHost()) // if client is host create start game request
    {
        QByteArray startGame;
        QTextStream requestMessage(&startGame, QIODevice::WriteOnly);
        requestMessage << static_cast<int>(MessageType::StartGameRequest) << endl
                       << timerPointer->getSecs() << endl;
        client->SendByteArray(startGame);
        this->hide();
        game = new GamePlayDialog(client, this, timerPointer->getSecs(), this);
        //Game stuff
        connect(game, &QDialog::rejected, this, &MainWindow::show);
        connect(game, &QDialog::rejected, this, &MainWindow::DisconnectHandle);
    }
    else{
        this->hide();
        game = new GamePlayDialog(client, this, client->getServerTime(), this);
        //Game stuff
        connect(game, &QDialog::rejected, this, &MainWindow::show);
        connect(game, &QDialog::rejected, this, &MainWindow::DisconnectHandle);
    }
}

void MainWindow::ExitMenu()
{
    if(client->state() == QAbstractSocket::ConnectedState){
        DisconnectHandle();
        return;
    }
    // What happens when the exit button is clicked (from main menu, not during gameplay)
    QApplication::quit();
}

void MainWindow::PlayerNameChange()
{
    // Update player's name
    QLabel* playerLabelArray[5];
    QLabel* colorLabelArray[5];
    colorLabelArray[0] = ui->HostColorLabel;
    playerLabelArray[0] = ui->HostNameLabel;
    colorLabelArray[1] = ui->Player2ColorLabel;
    playerLabelArray[1] = ui->Player2NameLabel;
    colorLabelArray[2] = ui->Player3ColorLabel;
    playerLabelArray[2] = ui->Player3NameLabel;
    colorLabelArray[3] = ui->Player4ColorLabel;
    playerLabelArray[3] = ui->Player4NameLabel;
    colorLabelArray[4] = ui->Player5ColorLabel;
    playerLabelArray[4] = ui->Player5NameLabel;

    for(int i = 0; i<5; i++){
        playerLabelArray[i]->setText(client->getPlayerName(i)); // Set name on player label
        switch(client->getPlayerColor(i)){
            case static_cast<int>(Color::Red):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : red; ;}");
            break;
            case static_cast<int>(Color::Pink):
            colorLabelArray[i]->setStyleSheet("QLabel{ background-color : pink; ;}");
            break;
            case static_cast<int>(Color::Cyan):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : cyan; ;}");
            break;
            case static_cast<int>(Color::Yellow):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : yellow; ;}");
            break;
            case static_cast<int>(Color::Green):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : green; ;}");
            break;
            case static_cast<int>(Color::Blue):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : blue; ;}");
            break;
            case static_cast<int>(Color::Purple):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : purple; ;}");
            break;
            case static_cast<int>(Color::Black):
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : black; ;}");
            break;
        default:
            colorLabelArray[i]->setStyleSheet("QLabel{background-color : transparent; ;}");
        }
    }
}

// Handle client disconnect errors
void MainWindow::DisconnectHandle()
{
    if(game){
        game->deleteLater();
        game = nullptr;
    }
    bool wasConnected = false;
    if(client->state() == QAbstractSocket::ConnectedState) wasConnected = true; // set up check to see what type of status to display
    // Make sure client is disconnected
    if(client->isHost()){
        bs->closeServer(); // close server connections if there are any
        bs->deleteLater(); // delete the previous server
    }
    client->disconnectFromHost();

    // reset all variables of concern
    client->setHost(false);
    ui->HostButton->setEnabled(true);
    ui->JoinButton->setEnabled(true);
    ui->SelectTimeButton->setEnabled(false);

    // Revert all button texts that were changed
    ui->ExitMenuButton->setText("EXIT");
    // set up color and name labels
    QLabel* playerLabelArray[5];
    QLabel* colorLabelArray[5];
    colorLabelArray[0] = ui->HostColorLabel;
    playerLabelArray[0] = ui->HostNameLabel;
    colorLabelArray[1] = ui->Player2ColorLabel;
    playerLabelArray[1] = ui->Player2NameLabel;
    colorLabelArray[2] = ui->Player3ColorLabel;
    playerLabelArray[2] = ui->Player3NameLabel;
    colorLabelArray[3] = ui->Player4ColorLabel;
    playerLabelArray[3] = ui->Player4NameLabel;
    colorLabelArray[4] = ui->Player5ColorLabel;
    playerLabelArray[4] = ui->Player5NameLabel;
    // revert color labels
    for(auto i : colorLabelArray){
        i->setStyleSheet("QLabel{background-color : transparent; ;}");
    }
    // clear name labels
    for(auto i : playerLabelArray){
        i->clear();
    }
    // Display why disconnect happened
    if(client->isServerAtLimit()){
       statusLabel->setText("Server has reached player limit!");
    }
    else if (wasConnected) {
        statusLabel->setText("Successfully disconnected");
    }
    else {
        statusLabel->setText("Connection error!");
    }
}


void MainWindow::LobbyRefresh(){
    game->deleteLater();
    game = nullptr;
    client->resetPlayerData();
    if(bs) bs->playerDataReset();
    if(client->isHost()){
        QTimer::singleShot(1000, this, &MainWindow::show); // make sure clients are back in lobby before host can click play (1 sec should be enough)
    }
    else {
        this->show();
    }
}


