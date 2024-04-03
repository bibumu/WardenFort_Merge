#include "accountsettings.h"
#include "ui_accountsettings.h"
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

accountSettings::accountSettings(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::accountSettings)
{
    ui->setupUi(this);
    connect(ui->applyButton, &QPushButton::clicked, this, &accountSettings::applyChanges);
}

accountSettings::~accountSettings()
{
    delete ui;
    QPixmap pixmap (":/wardenfort/pfp");
    ui->pfp->setPixmap(pixmap);
}

void accountSettings::getUsername(const QString& text)
{
    Username = text;
    Username.replace("Welcome,", "");
    Username.replace("!", "");
    Username = Username.simplified();
}

void accountSettings::applyChanges() 
{
    QSqlDatabase db = QSqlDatabase::database();

    // Check if the database is open
    if (!db.isOpen()) {
        qDebug() << "Database is not open";
        return;
    }
    QString firstName = ui->fnameLine->text();
    QString lastName = ui->lnameLine->text(); 
    QString newUsername = ui->usernameLine->text(); 
    QString email = ui->emailLine->text(); 
    
    // Prepare the SQL query to update the password
    QSqlQuery query;
    query.prepare("UPDATE user_db SET firstName = :firstName, lastName = :lastName, email = :email, username = :newUsername WHERE username = :oldUsername");
    query.bindValue(":newUsername", newUsername);
    query.bindValue(":oldUsername", Username);
    query.bindValue(":email", email);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);


    // Execute the query
    if (query.exec()) {
        qDebug() << "yey updated successfully for user:" << Username;
    }
    else {
        qDebug() << "Error updating password:" << query.lastError().text();
    }
    Username = newUsername;
}
