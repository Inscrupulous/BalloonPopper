#include "customizedialog.h"
#include "ui_customizedialog.h"

#include <QLabel>
#include <QPixmap>
#include <QMessageBox>


CustomizeDialog::CustomizeDialog(Client *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomizeDialog)
{
    ui->setupUi(this);

    client = c;

    // Connect statements for the color options
    connect(ui->RedButton, SIGNAL(clicked()), this, SLOT(SelectRed()));
    connect(ui->PinkButton, SIGNAL(clicked()), this, SLOT(SelectPink()));
    connect(ui->CyanButton, SIGNAL(clicked()), this, SLOT(SelectCyan()));
    connect(ui->YellowButton, SIGNAL(clicked()), this, SLOT(SelectYellow()));
    connect(ui->GreenButton, SIGNAL(clicked()), this, SLOT(SelectGreen()));
    connect(ui->BlueButton, SIGNAL(clicked()), this, SLOT(SelectBlue()));
    connect(ui->PurpleButton, SIGNAL(clicked()), this, SLOT(SelectPurple()));
    connect(ui->BlackButton, SIGNAL(clicked()), this, SLOT(SelectBlack()));
    connect(ui->custSaveButton, SIGNAL(clicked()), this, SLOT(SaveColor()));
    connect(ui->custCloseButton, &QPushButton::clicked, this, &QDialog::accept);

}

CustomizeDialog::~CustomizeDialog()
{
    delete ui;
}

void CustomizeDialog::SelectRed()
{
    // Load Pixmap from resources
    // 0 = determine image format by looking at filename; auto conversion
    QPixmap myMap;
    if (!myMap.load(":/RedTank.png")) // myMap.load("RedTank.png", 0, Qt::AutoColor))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    // Display
    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->RedButton;
    colorChosen = static_cast<int>(Color::Red);
}

void CustomizeDialog::SelectPink()
{
    QPixmap myMap;
    if (!myMap.load(":/PinkTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->PinkButton;
    colorChosen = static_cast<int>(Color::Pink);
}

void CustomizeDialog::SelectCyan()
{
    QPixmap myMap;
    if (!myMap.load(":/CyanTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->CyanButton;
    colorChosen = static_cast<int>(Color::Cyan);
}

void CustomizeDialog::SelectYellow()
{
    QPixmap myMap;
    if (!myMap.load(":/YellowTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->YellowButton;
    colorChosen = static_cast<int>(Color::Yellow);
}

void CustomizeDialog::SelectGreen()
{
    QPixmap myMap;
    if (!myMap.load(":/GreenTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);        //set pixmap to show up in the dialog box
    currentColor = ui->GreenButton;         //set currentColor button to the green button
    colorChosen = static_cast<int>(Color::Green);
}

void CustomizeDialog::SelectBlue()
{
    QPixmap myMap;
    if (!myMap.load(":/BlueTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->BlueButton;
    colorChosen = static_cast<int>(Color::Blue);
}

void CustomizeDialog::SelectPurple()
{
    QPixmap myMap;
    if (!myMap.load(":/PurpleTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->PurpleButton;
    colorChosen = static_cast<int>(Color::Purple);
}

void CustomizeDialog::SelectBlack()
{
    QPixmap myMap;
    if (!myMap.load(":/BlackTank.png"))
    {
        QMessageBox* msgBox = new QMessageBox;
        msgBox->setText("Error - unable to assign color to avatar.");
        msgBox->show();
    }

    ui->TankLabel->setPixmap(myMap);
    currentColor = ui->BlackButton;
    colorChosen = static_cast<int>(Color::Black);
}

void CustomizeDialog::SaveColor()
{
    if(!client->isColorAvailabe(colorChosen)){ // If client is connected and color isn't available don't leave list
        qDebug() << "Error choosing color: Color wasn't available";
        updateColorList();
        return;
    }
    client->setColor(colorChosen);
    updateColorList();
    // Should disable button for color chosen for all other players
    if (prevColor != nullptr)               //if prevColor points to a previous color, enable it
    {
        prevColor->setEnabled(true);
    }
    if(currentColor != nullptr)        //check whether the player has selected ANY color at all
    {
        currentColor->setEnabled(false);    //needs to be sent to all players]
        prevColor = currentColor;           //it's a little weird having prevColor always equal to currentColor except in this function
    }
    this->close();
}

void CustomizeDialog::updateColorList(){
    // Update enabled buttons based on clients color info
    if(client->isColorAvailabe(static_cast<int>(Color::Red))){
        ui->RedButton->setEnabled(true);
    }
    else {
        ui->RedButton->setEnabled(false);   // Someone has taken color
        if(client->getMyColor() == static_cast<int>(Color::Red)){   //If its me set my prev choice enabled and update the pointer
                prevColor = ui->RedButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Pink))){

        ui->PinkButton->setEnabled(true);
    }
    else {
        ui->PinkButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Pink)){
                prevColor = ui->PinkButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Cyan))){
        ui->CyanButton->setEnabled(true);
    }
    else {
        ui->CyanButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Cyan)){
                prevColor = ui->CyanButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Yellow))){
        ui->YellowButton->setEnabled(true);
    }
    else {
        ui->YellowButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Yellow)){
                prevColor = ui->YellowButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Green))){
        ui->GreenButton->setEnabled(true);
    }
    else {
        ui->GreenButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Green)){
                prevColor = ui->GreenButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Blue))){
        ui->BlueButton->setEnabled(true);
    }
    else {
        ui->BlueButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Blue)){
                prevColor = ui->BlueButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Purple))){
        ui->PurpleButton->setEnabled(true);
    }
    else {
        ui->PurpleButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Purple)){
                prevColor = ui->PurpleButton;
        }
    }
    if(client->isColorAvailabe(static_cast<int>(Color::Black))){
        ui->BlackButton->setEnabled(true);
    }
    else {
        ui->BlackButton->setEnabled(false);
        if(client->getMyColor() == static_cast<int>(Color::Black)){
                prevColor = ui->BlackButton;
        }
    }
}
































