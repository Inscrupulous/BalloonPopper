#ifndef PLAYERNAMEDIALOG_H
#define PLAYERNAMEDIALOG_H

#include <QDialog>
#include <QHostAddress>

namespace Ui {
class PlayerNameDialog;
}

class PlayerNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerNameDialog(QWidget *parent = nullptr);
    ~PlayerNameDialog();
    const QHostAddress getIP(){return ipAddress;}
    quint16 getPort(){return portNumber;}
    const QString getPlayerName(){return name;}

signals:
    void ValidInput();
    void closeClicked();

private:
    Ui::PlayerNameDialog *ui;
    QString name = "";
    QHostAddress ipAddress;
    quint16 portNumber;

private slots:
    void submitName();  // may need to be moved to MenuMainWindow class so both Host and Players can use slot
};

#endif // PLAYERNAMEDIALOG_H
