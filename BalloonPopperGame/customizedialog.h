#ifndef CUSTOMIZEDIALOG_H
#define CUSTOMIZEDIALOG_H

#include <QDialog>
#include <QDebug>
#include "messagetype.h"
#include "client.h"

namespace Ui {
class CustomizeDialog;
}

class CustomizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomizeDialog(Client* client = nullptr, QWidget *parent = nullptr);
    ~CustomizeDialog();

private:
    Ui::CustomizeDialog *ui;
    Client *client = nullptr;
    QPushButton *currentColor = nullptr;
    QPushButton *prevColor = nullptr;       //prevColor is used only to re-enable a previous color selection.
    int colorChosen = static_cast<int>(Color::Red);
    int colorSaved = static_cast<int>(Color::Red);

signals:
    void NewColorSaved(int);

private slots:
    void SelectRed();       //should each player have a color selected, but not saved, upon joining?
    void SelectPink();
    void SelectCyan();
    void SelectYellow();
    void SelectGreen();
    void SelectBlue();
    void SelectPurple();
    void SelectBlack();
    void SaveColor();

public slots:
    void updateColorList();
};

#endif // CUSTOMIZEDIALOG_H
