#include "hscoresdialog.h"
#include "messagetype.h"
#include "ui_hscoresdialog.h"
#include <QtDebug>

HScoresDialog::HScoresDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HScoresDialog)
{
    ui->setupUi(this);
    connect(ui->hScoresCloseButton, SIGNAL(clicked()), this, SLOT(Close()));
}

HScoresDialog::~HScoresDialog()
{
    db.removeDatabase("QSQLITE");
    delete ui;
}

void HScoresDialog::Close()
{
    db.removeDatabase("QSQLITE");
    this->close();
}

void HScoresDialog::UpdateHScores()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../highscores.db");

    if(!db.open())
    {
        qDebug() << "Error: unable to open database";
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT * FROM highscores");
    if(!q.exec())
    {
        qDebug() << "Error: unable to locate table";
        return;
    }

    model = new QSqlQueryModel;
    model->setQuery("SELECT * FROM highscores ORDER BY scorePerSecond DESC, score DESC LIMIT 10");
    ui->tableView->setModel(model);
    q.finish();
    db.removeDatabase("QSQLITE");
}
