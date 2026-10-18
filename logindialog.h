#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QTimer>

class DatabaseManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginDialog;
}
QT_END_NAMESPACE

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(DatabaseManager *dbMgr = nullptr, QWidget *parent = nullptr);
    ~LoginDialog() override;

    QString username() const;
    QString databaseChoice() const;

private slots:
    void onLoginClicked();
    void onCancelClicked();
    void onHelpClicked();
    void onPowerClicked();
    void updateClock();

private:
    Ui::LoginDialog *ui;
    DatabaseManager *m_dbMgr = nullptr;
    QTimer *m_clockTimer = nullptr;
};

#endif // LOGINDIALOG_H
