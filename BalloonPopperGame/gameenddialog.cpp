#include "gameenddialog.h"
#include "ui_gameenddialog.h"
#include "mainwindow.h"
#include <QtDebug>

GameEndDialog::GameEndDialog(MainWindow* lobbyScreen, Client* c, int time, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameEndDialog)
{
    ui->setupUi(this);
    client = c;
    int thisID = client->getMyID();
    mainWindow = lobbyScreen;
    initTime = time;
    // Connect close and submit buttons to actions
    connect(ui->ExitLobbypushButton, SIGNAL(clicked(bool)), this, SLOT(Exit()));
    connect(ui->PlayAgainpushButton, SIGNAL(clicked(bool)), this, SLOT(PlayAgain()));
    connect(client, &Client::SendFinalScores, this, &GameEndDialog::DisplayFinalScores);
    QTimer *selectTime = new QTimer(this);
    connect(selectTime, &QTimer::timeout,this, &GameEndDialog::timeToClose);
    selectTime->start(1000);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("finalScores.db");
    if(!db.open())
    {
        qDebug() << db.lastError();
        qDebug() << "Error: unable to connect to database";
        return;
    }

    QSqlQuery q("DROP TABLE scores");
    q.prepare("CREATE TABLE scores(name TEXT, score INT, scorePerSecond FLOAT)");
    if(!q.exec())
    {
        qDebug() << db.lastError();
        qDebug() << "Error: database error";
        return;
    }
    model = new QSqlQueryModel(this);

    QByteArray out;
    QTextStream requestMessage(&out, QIODevice::WriteOnly);
    requestMessage << static_cast<int>(MessageType::SendFinalScores) << endl
                   << thisID << endl;
    client->SendByteArray(out);
}

GameEndDialog::~GameEndDialog()
{
    delete ui;
}

void GameEndDialog::timeToClose(){
    if(countdown > 0){
       ui->counter->setText("Please pick an option: " + QString::number(--countdown));
    }
    else if(playAgainPressed){
        mainWindow->LobbyRefresh();
        this->deleteLater();
    }
    else {
        mainWindow->DisconnectHandle();
        this->deleteLater();
    }

}

void GameEndDialog::PlayAgain()
{
    playAgainPressed = true;
    ui->PlayAgainpushButton->setEnabled(false);
    ui->ExitLobbypushButton->setEnabled(false);
}
void GameEndDialog::Exit()
{
    playAgainPressed = false;
    mainWindow->DisconnectHandle();
    this->deleteLater();
}

void GameEndDialog::DisplayFinalScores(QTextStream& incomingData)
{
    int lobbySize;
    QString name;
    int score;
    SimplePlayer player1;               // Initialize identifiers for all players
    SimplePlayer player2;
    SimplePlayer player3;
    SimplePlayer player4;
    SimplePlayer player5;
    incomingData >> lobbySize;
    SimplePlayer plarray[lobbySize];    // Initialize array for players
    for(int i = 0; i < lobbySize; i++)  // Get name and score for each player
    {
        incomingData >> name >> score;

        float scoreTimeUnRounded = float(score) / float(initTime);
        float scoreTime = int(scoreTimeUnRounded * 1000 + .5);
        scoreTime = scoreTime/1000;

        switch(i)
        {
        case 0:
            player1.name = name;
            player1.score = score;
            player1.scoreTime = scoreTime;
            plarray[i] = player1;
            break;
        case 1:
            player2.name = name;
            player2.score = score;
            player2.scoreTime = scoreTime;
            plarray[i] = player2;
            break;
        case 2:
            player3.name = name;
            player3.score = score;
            player3.scoreTime = scoreTime;
            plarray[i] = player3;
            break;
        case 3:
            player4.name = name;
            player4.score = score;
            player4.scoreTime = scoreTime;
            plarray[i] = player4;
            break;
        case 4:
            player5.name = name;
            player5.score = score;
            player5.scoreTime = scoreTime;
            plarray[i] = player5;
            break;
        }
    }

    QSqlQuery q;
    for(int i = 0; i < lobbySize; i++)
    {
        q.prepare("INSERT INTO scores VALUES(:name, :score, :scorePerSecond)");
        q.bindValue(":name", plarray[i].name);
        q.bindValue(":score", plarray[i].score);
        q.bindValue(":scorePerSecond", plarray[i].scoreTime);
        if(!q.exec())
        {
            db.lastError();
            qDebug() << "Error adding values";
        }
    }

    model->setQuery("SELECT * FROM scores ORDER BY score DESC, scorePerSecond DESC;");
    ui->tableView->setModel(model);
    q.finish();
    db.close();

    db.setDatabaseName("../highscores.db");

    if(!db.open())
    {
        qDebug() << "Error: unable to open database";
        return;
    }

    QSqlQuery que;
    que.prepare("SELECT * FROM highscores");
    if(!que.exec())
    {
        qDebug() << "Error: unable to locate table";
        return;
    }

    for(int i = 0; i < lobbySize; i++)
    {
        que.prepare("INSERT INTO highscores VALUES(:player, :score, :scorePerSecond)");
        que.bindValue(":player", plarray[i].name);
        que.bindValue(":score", plarray[i].score);
        que.bindValue(":scorePerSecond", plarray[i].scoreTime);
        if(!que.exec())
        {
            db.lastError();
            qDebug() << "Error adding values";
        }
    }
    que.finish();
    db.removeDatabase("QSQLITE");
}
