#ifndef GAMEENDDIALOG_H
#define GAMEENDDIALOG_H

#include <QDialog>
#include "mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

namespace Ui {
class GameEndDialog;
}

class GameEndDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameEndDialog(MainWindow* lobby = nullptr, Client* c = nullptr, int time = 0, QWidget *parent = nullptr);
    ~GameEndDialog();

private:
    Ui::GameEndDialog *ui;
    MainWindow* mainWindow;
    Client* client = nullptr;
    QSqlDatabase db;       //temporary database to hold the most recent match scores
    QSqlQueryModel* model;
    int countdown = 15;
    bool playAgainPressed = true;
    int initTime;

private slots:
    void PlayAgain();
    void Exit();
    void timeToClose();
    void DisplayFinalScores(QTextStream&);
};

#endif // GAMEENDDIALOG_H
