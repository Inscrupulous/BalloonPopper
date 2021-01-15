#include "hostnamedialog.h"
#include "ui_hostnamedialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>

HostNameDialog::HostNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostNameDialog)
{
    ui->setupUi(this);

    // Connect close and submit buttons to actions
    connect(ui->HostNameCloseButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->HostPlayerNameButton, SIGNAL(clicked(bool)), this, SLOT(submitName()));
    connect(ui->HostNameCloseButton, &QPushButton::clicked, [=]{
        emit closeClicked();
    });

    // Set up validators - only allow up to 10 characters in a name - can be letters and number only
    QRegularExpression regex("[A-Za-z0-9]{10}");
    QRegularExpressionValidator* lineEditValidator = new QRegularExpressionValidator(regex, ui->HostNameLineEdit);
    ui->HostNameLineEdit->setValidator(lineEditValidator);
}

HostNameDialog::~HostNameDialog()
{
    delete ui;
}

void HostNameDialog::submitName()
{
    // Post name to host QLabel in Main Menu Window
    if(ui->HostNameLineEdit->text() != "")
    {
        name = ui->HostNameLineEdit->text();
        emit submitClicked(name);    // Emitted for name change in mainwindow
        this->accept();
    }
    else
    {
        ui->noNameLabel->setText("Please enter a name");
    }
}
