#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Qt SQL drivers:"
             << QSqlDatabase::drivers();

    qDebug() << "Plugin paths:"
             << QCoreApplication::libraryPaths();

    if (!QSqlDatabase::drivers().contains("QMYSQL")) {
        qDebug() << "ERROR: QMYSQL driver is missing.";
    }else
        qDebug() << "QMYSQL driver is available.";

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "DRIVER={MySQL ODBC 26.7 Unicode Driver};"
        "SERVER=127.0.0.1;"
        "PORT=3306;"
        "DATABASE=school_db;"
        "UID=school_app;"   //Username
        "PWD=ReplaceWithYourOwnStrongPassword;"    //Password
        );

    if (!db.open()) {
        qCritical() << "Connection failed:"
                    << db.lastError().text();
        return 1;
    }

    QSqlQuery query(db);

    if (query.exec("SELECT VERSION()") && query.next()) {
        qDebug() << "Connected to MySQL:"
                 << query.value(0).toString();
    } else {
        qCritical() << query.lastError().text();
        return 2;
    }

    return 0;
}
