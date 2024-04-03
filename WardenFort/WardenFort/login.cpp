#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QLineEdit> // Include for QLineEdit
#include <QCheckBox> // Include for QCheckBox
#include "wardenfort.h"
#include "signup.h"

login::login(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);

    
    // Set the echo mode of the password input field to Password
    ui->typePASS_box->setEchoMode(QLineEdit::Password);

    // Connect the clicked() signal of the loginButton to the on_loginButton_clicked() slot
    

    connect(ui->signUpButton, &QPushButton::released, this, &login::on_signUpButton_released);

    // Connect the stateChanged() signal of the QCheckBox to the on_eye_open_clicked() and on_eye_closed_clicked() slots
    connect(ui->eye_open, &QCheckBox::stateChanged, this, &login::on_eye_open_clicked);
    connect(ui->eye_closed, &QCheckBox::stateChanged, this, &login::on_eye_closed_clicked);

    connect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
    connect(ui->typeUN_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
    connect(ui->typePASS_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
    // Set icons for the eye checkbox
    ui->eye_open->setIcon(QIcon("eye_open.png"));
    ui->eye_closed->setIcon(QIcon("eye_closed.png"));
}

login::~login()
{
    delete ui;
}

void login::on_signUpButton_released() {
    signup* signupWindow = new signup;
    this->close();

    signupWindow->show();

    disconnect(ui->signUpButton, &QPushButton::released, this, &login::on_signUpButton_released);
}

void login::on_loginButton_released()
{
    QString username = ui->typeUN_box->text();
    QString password = ui->typePASS_box->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM user_db WHERE username = :username AND passwd = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        qDebug() << "Login successful for username:" << username;
        this->close(); // Close the login window

        // Create an instance of WardenFort
        WardenFort* wardenFortWindow = new WardenFort;
        wardenFortWindow->show();
        wardenFortWindow->scanActiveLANAdapters();

        // Set the welcome text
        QString welcomeText = "Welcome, " + username + "!";
        wardenFortWindow->setWelcomeText(welcomeText);

        // Disconnect the signal-slot connection to prevent multiple executions
        disconnect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
        disconnect(ui->typeUN_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
        disconnect(ui->typePASS_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
    }
    else {
        qDebug() << "Invalid username or password.";
        QMessageBox::warning(this, "Login Error", "Invalid username or password.");
        disconnect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
        disconnect(ui->typeUN_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
        disconnect(ui->typePASS_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
    }
    connect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
    connect(ui->typeUN_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
    connect(ui->typePASS_box, &QLineEdit::returnPressed, this, &login::on_loginButton_released);
}



void login::on_eye_closed_clicked()
{
    // Set the echo mode of the password input field to Password
    ui->typePASS_box->setEchoMode(QLineEdit::Password);

    // Change the icon to open eye
    ui->eye_open->setVisible(true);
    ui->eye_closed->setVisible(false);
}

void login::on_eye_open_clicked()
{
    // Set the echo mode of the password input field to Normal
    ui->typePASS_box->setEchoMode(QLineEdit::Normal);

    // Change the icon to closed eye
    ui->eye_open->setVisible(false);
    ui->eye_closed->setVisible(true);
    
    
}

void login::on_typePASS_box_returnPressed()
{
    // Trigger the login process
    on_loginButton_released();
}
