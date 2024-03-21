// main.cpp
#include <QApplication>
#include "WardenFort.h"
#include <QSqlError>
#include "login.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    WardenFort w;
    //w.show();

    // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/WardenFort/WardenFort/WardenFort_Merge/WardenFort/WardenFort/wardenfort.db"); // Set the full path to your database file here

    // Open the database connection
    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return 1;
    }

    qDebug() << "Database connected successfully";

    // Now that the database connection is established, you can show your login window
    login login; // Assuming you have a login window defined
    login.show();

    // Call the function to scan active LAN adapters and capture TCP packets
    //w.scanActiveLANAdapters(); // Corrected invocation

    return a.exec();
}