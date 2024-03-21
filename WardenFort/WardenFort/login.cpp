#include "login.h"
#include "wardenfort.h"
#include "ui_login.h"
#include "signup.h"
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>

login::login(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);

    // Connect signals to slots
    connect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
    connect(ui->signup, &QPushButton::released, this, &login::on_signup_released);

    signupWindow = new signup(this);
    signupWindow->hide();
}

login::~login()
{
    delete ui;
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
        WardenFort* wardenFortWindow = new WardenFort;
        wardenFortWindow->show();
        wardenFortWindow->scanActiveLANAdapters();

        // Disconnect the signal-slot connection to prevent multiple executions
        disconnect(ui->loginButton, &QPushButton::released, this, &login::on_loginButton_released);
    }
    else {
        qDebug() << "Invalid username or password.";
        QMessageBox::warning(this, "Login Error", "Invalid username or password.");
    }
}


void login::on_signup_released()
{
    signupWindow->show();
    this->hide(); // Hide the login window
}