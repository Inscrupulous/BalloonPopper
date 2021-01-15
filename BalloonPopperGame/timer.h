#ifndef TIMER_H
#define TIMER_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Timer;
}

class Timer : public QDialog
{
    Q_OBJECT

public:
    explicit Timer(QWidget *parent = nullptr);
    ~Timer();
    int secs = 30;
    int getSecs() const{return secs;}
private:
    Ui::Timer *ui;
    QPushButton *currentTime = nullptr;
    QPushButton *prevTime = nullptr;
private slots:
    void Select30();
    void Select60();
    void Select90();
};

#endif // TIMER_H
