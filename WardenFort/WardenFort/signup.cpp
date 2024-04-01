#include "signup.h"
#include "ui_signup.h"
#include <QGraphicsOpacityEffect>
#include <QPushButton>
#include <QMessageBox> // Include the header for QMessageBox
#include <QSqlQuery>
#include <QSqlError>
#include "login.h"
#include <QLineEdit> // Include for QLineEdit
#include <QCheckBox> // Include for QCheckBox

signup::signup(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::signup)
{
    ui->setupUi(this);
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.5); // Set opacity to 50%
    ui->label->setGraphicsEffect(opacityEffect);
    connect(ui->pushButton, &QPushButton::clicked, this, &signup::on_pushButton_clicked, Qt::UniqueConnection);

    connect(ui->backButton, &QPushButton::released, this, &signup::on_backButton_released);

    connect(ui->eye_open, &QCheckBox::stateChanged, this, &signup::on_eye_open_clicked);
    connect(ui->eye_closed, &QCheckBox::stateChanged, this, &signup::on_eye_closed_clicked);

    ui->lineEdit_5->setEchoMode(QLineEdit::Password);

    ui->eye_open->setIcon(QIcon("eye_open.png"));
    ui->eye_closed->setIcon(QIcon("eye_closed.png"));
}

signup::~signup()
{
    delete ui;
}

void signup::on_eye_closed_clicked()
{
    // Set the echo mode of the password input field to Password
    ui->lineEdit_5->setEchoMode(QLineEdit::Password);

    // Change the icon to open eye
    ui->eye_open->setVisible(true);
    ui->eye_closed->setVisible(false);
}

void signup::on_eye_open_clicked()
{
    // Set the echo mode of the password input field to Normal
    ui->lineEdit_5->setEchoMode(QLineEdit::Normal);

    // Change the icon to closed eye
    ui->eye_open->setVisible(false);
    ui->eye_closed->setVisible(true);
}

void signup::on_backButton_released() {
    login* loginWindow = new login;
    this->close();

    loginWindow->show();

    disconnect(ui->backButton, &QPushButton::released, this, &signup::on_backButton_released);
}

void signup::on_pushButton_clicked()
{
    QString firstname = ui->lineEdit->text(); // Assuming QLineEdit for username input
    QString lastname = ui->lineEdit_2->text(); // Assuming QLineEdit for password input
    QString username = ui->lineEdit_3->text(); // Assuming QLineEdit for username input
    QString email = ui->lineEdit_4->text(); // Assuming QLineEdit for password input
    QString password = ui->lineEdit_5->text(); // Assuming QLineEdit for username input
    
    QSqlQuery signQuery;
    signQuery.prepare("INSERT INTO user_db (username, passwd, firstName, lastName, email) VALUES (:username, :password, :fname, :lname, :email)");
    signQuery.bindValue(":username", username);
    signQuery.bindValue(":password", password);
    signQuery.bindValue(":fname", firstname);
    signQuery.bindValue(":lname", lastname);
    signQuery.bindValue(":email", email);
    qDebug() << "signup successful for username:" << username;
    if (!signQuery.exec()) {
        qDebug() << "Error executing query:" << signQuery.lastError().text();
        // Handle the error as necessary
    }
    else {
        // Commit the transaction after successful execution of the query
        QSqlDatabase::database().commit();
        qDebug() << "Signup successful for username:" << username;
    }


}

