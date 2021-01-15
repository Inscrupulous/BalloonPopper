#ifndef HOSTNAMEDIALOG_H
#define HOSTNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class HostNameDialog;
}

class HostNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostNameDialog(QWidget *parent = nullptr);
    ~HostNameDialog();

private:
    Ui::HostNameDialog *ui;
    QString name = "";

signals:
    void submitClicked(QString newPlayerName);
    void closeClicked();

private slots:
    void submitName();  // may need to be moved to MenuMainWindow class so both Host and Players can use slot
};

#endif // HOSTNAMEDIALOG_H
