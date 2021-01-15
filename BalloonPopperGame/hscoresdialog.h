#ifndef HSCORESDIALOG_H
#define HSCORESDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QtDebug>
#include "client.h"

namespace Ui {
class HScoresDialog;
}

class HScoresDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HScoresDialog(QWidget *parent = nullptr);
    ~HScoresDialog();
    void UpdateHScores();

private:
    Ui::HScoresDialog *ui;
	QSqlDatabase db;
    QSqlQueryModel *model;

private slots:
    void Close();
};

#endif // HSCORESDIALOG_H
