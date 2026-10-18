#include "mainwindow.h"
#include "logindialog.h"
#include "databasemanager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPalette pal = a.palette();
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Text, Qt::black);
    pal.setColor(QPalette::ButtonText, Qt::black);
    pal.setColor(QPalette::ToolTipText, Qt::black);
    a.setPalette(pal);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "cb26_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // Initialize local encrypted/authenticated SQLite database
    DatabaseManager dbManager;
    dbManager.initLocalDatabase();

    // Launch CoffeeBreak Login popup dialog
    LoginDialog loginDialog(&dbManager);
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0; // Exit cleanly if user cancelled or closed dialog
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
