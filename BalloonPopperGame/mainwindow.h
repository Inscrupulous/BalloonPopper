#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include <QLabel>
#include <QMessageBox>
#include <QAction>
#include "hscoresdialog.h"
#include "customizedialog.h"
#include "balloonserver.h"
#include "timer.h"

// Forward declaration of game
class GamePlayDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QAction* helpAction;
    Client* client;
    QLabel* statusLabel;
    HScoresDialog* hs;
    CustomizeDialog* cd;
    BalloonServer* bs = nullptr;
    GamePlayDialog* game = nullptr;
    Timer* timerPointer = nullptr;
    QMessageBox msgBox;

private slots:
    void showHelp();
    void Host();
    void Join();
    void SelectTime();
    void SelectColorWindow();
    void ShowHighScores();
    void Play();
    void ExitMenu();
    void HostSubmitted(QString nme);
    void PlayerNameChange();

public slots:
    void DisconnectHandle();
    void LobbyRefresh();
};

#endif // MAINWINDOW_H
