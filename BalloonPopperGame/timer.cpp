#include "timer.h"
#include "ui_timer.h"
#include "gameplaydialog.h"

Timer::Timer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Timer)
{
    ui->setupUi(this);
    ui->TimeSelect1->setText("30 seconds");
    ui->TimeSelect2->setText("60 seconds");
    ui->TimeSelect3->setText("90 seconds");
    prevTime = ui->TimeSelect1;
    prevTime->setEnabled(false); // disable the default choice
    // Connect statements for the Timer options
    connect(ui->TimeSelect1, SIGNAL(clicked()), this, SLOT(Select30()));
    connect(ui->TimeSelect2, SIGNAL(clicked()), this, SLOT(Select60()));
    connect(ui->TimeSelect3, SIGNAL(clicked()), this, SLOT(Select90()));
    connect(ui->TimerClose, &QPushButton::clicked, this, &QDialog::accept);
}

Timer::~Timer()
{
    delete ui;
}

void Timer::Select30()
{
    secs = 30;
    prevTime->setEnabled(true);
    prevTime = ui->TimeSelect1;
    prevTime->setEnabled(false);
}
void Timer::Select60()
{
    secs = 60;
    prevTime->setEnabled(true);
    prevTime = ui->TimeSelect2;
    prevTime->setEnabled(false);
}
void Timer::Select90()
{
    secs = 90;
    prevTime->setEnabled(true);
    prevTime = ui->TimeSelect3;
    prevTime->setEnabled(false);
}
