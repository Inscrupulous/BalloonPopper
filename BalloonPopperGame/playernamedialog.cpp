#include "playernamedialog.h"
#include "ui_playernamedialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>

PlayerNameDialog::PlayerNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerNameDialog)
{
    ui->setupUi(this);

    // Connect close and submit buttons to actions
    connect(ui->PlayerNameCloseButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->PlayerNameCloseButton, &QPushButton::clicked, [=]{
        emit closeClicked();
    });
    connect(ui->SubmitPlayerNameButton, SIGNAL(clicked(bool)), this, SLOT(submitName()));

    // Set up validators - only allow up to 10 characters in a name - can be letters and number only
    QRegularExpression regex("[A-Za-z0-9]{10}");
    QRegularExpressionValidator* lineEditValidator = new QRegularExpressionValidator(regex, ui->PlayerNameLineEdit);
    ui->PlayerNameLineEdit->setValidator(lineEditValidator);

    // Set up validators for IP and Port line edits
    //ui->ipAddressLineEdit->setInputMask("000.000.000.000;");
    QRegularExpression IPregex("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    QRegularExpressionValidator* IPlineEditValidator = new QRegularExpressionValidator(IPregex, ui->ipAddressLineEdit);
    ui->ipAddressLineEdit->setValidator(IPlineEditValidator);

    QRegularExpression portRegex("[0-9]{1,5}");
    QRegularExpressionValidator* portLineEditValidator = new QRegularExpressionValidator(portRegex, ui->portNumberLineEdit);
    ui->portNumberLineEdit->setValidator(portLineEditValidator);
}

PlayerNameDialog::~PlayerNameDialog()
{
    delete ui;
}

void PlayerNameDialog::submitName()
{
    // If the port # and address are valid inputs
    if(ui->portNumberLineEdit->hasAcceptableInput() && ui->ipAddressLineEdit->hasAcceptableInput())
    {   // Post name to player QLabel in Main Menu Window
        if(ui->PlayerNameLineEdit->text() != "")
        {
            name = ui->PlayerNameLineEdit->text();
            ipAddress = QHostAddress(ui->ipAddressLineEdit->text());
            portNumber = ui->portNumberLineEdit->text().toUShort();
            emit ValidInput();
            ui->invalidInputLabel->setText("");
            this->accept();
        }
        else
        {
            ui->invalidInputLabel->setText("Please enter a name");
        }
    }
    else
    {
        ui->invalidInputLabel->setText("Please make a valid input");
    }
}
