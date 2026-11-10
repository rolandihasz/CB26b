#include "logindialog.h"
#include "databasemanager.h"
#include "ui_logindialog.h"

#include <QDateTime>
#include <QLocale>
#include <QMessageBox>

LoginDialog::LoginDialog(DatabaseManager *dbMgr, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_dbMgr(dbMgr) {
  ui->setupUi(this);

  // Dialog window flags
  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint |
                 Qt::CustomizeWindowHint);

  // Connect action buttons
  connect(ui->m_loginBtn, &QPushButton::clicked, this,
          &LoginDialog::onLoginClicked);
  connect(ui->m_cancelBtn, &QPushButton::clicked, this,
          &LoginDialog::onCancelClicked);
  connect(ui->m_helpBtn, &QPushButton::clicked, this,
          &LoginDialog::onHelpClicked);
  connect(ui->m_powerBtn, &QPushButton::clicked, this,
          &LoginDialog::onPowerClicked);

  // Keyboard navigation: Enter on username moves to password, Enter on password
  // triggers Login
  connect(ui->m_usernameEdit, &QLineEdit::returnPressed, this,
          [this]() { ui->m_passwordEdit->setFocus(); });
  connect(ui->m_passwordEdit, &QLineEdit::returnPressed, this,
          &LoginDialog::onLoginClicked);

  // Clock timer for live updating date/time in the header bar
  m_clockTimer = new QTimer(this);
  connect(m_clockTimer, &QTimer::timeout, this, &LoginDialog::updateClock);
  m_clockTimer->start(1000);
  updateClock();

  ui->m_usernameEdit->setFocus();
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::updateClock() {
  if (ui && ui->m_timeLabel) {
    QString timeStr = QLocale(QLocale::English)
                          .toString(QDateTime::currentDateTime(),
                                    QStringLiteral("ddd MMM d yyyy hh:mm:ss"));
    ui->m_timeLabel->setText(timeStr);
  }
}

void LoginDialog::onLoginClicked() {
  QString user = ui->m_usernameEdit->text().trimmed();
  QString pass = ui->m_passwordEdit->text();

  if (user.isEmpty() || pass.isEmpty()) {
    QMessageBox::warning(this, tr("Login Error"),
                         tr("Please enter both username and password."));
    return;
  }

  // Strictly require an existing, connected SQLite database even for admin/admin
  if (!m_dbMgr) {
    QMessageBox::critical(
        this, tr("Database Unavailable"),
        tr("Database manager is not initialized. Login cannot proceed."));
    return;
  }

  if (!m_dbMgr->isConnected() && !m_dbMgr->openExistingDatabase()) {
    QMessageBox::critical(
        this, tr("Database Connection Error"),
        tr("SQLite database (coffeebreak.db) was not found or unable to connect.\n\n"
           "Login is not permitted when the database is unavailable (even for the main admin account)."));
    return;
  }

  if (m_dbMgr->authenticateUser(user, pass)) {
    accept();
  } else {
    if (!m_dbMgr->isConnected()) {
      QMessageBox::critical(
          this, tr("Database Connection Error"),
          tr("Lost connection to SQLite database (coffeebreak.db). Login aborted."));
    } else {
      QMessageBox::critical(
          this, tr("Authentication Failed"),
          tr("Invalid username or password!\n\nCredentials are verified against the SQLite 'users' table."));
    }
    ui->m_passwordEdit->clear();
    ui->m_passwordEdit->setFocus();
  }
}

void LoginDialog::onCancelClicked() { reject(); }

void LoginDialog::onHelpClicked() {
  QMessageBox::information(
      this, tr("CoffeeBreak Login Help"),
      tr("<b>CoffeeBreak Database Login</b><br><br>"
         "Credentials are authenticated strictly against the local SQLite database (<b>coffeebreak.db</b>).<br>"
         "If the database file is missing or unreachable, login is blocked for all accounts.<br><br>"
         "Default database account:<br>"
         "<b>Username:</b> admin<br>"
         "<b>Password:</b> admin<br><br>"
         "Encrypted credentials storage enabled (SHA-256 + 128-bit cryptographic salt).<br><br>"
         "Support: <a href=\"https://lnrsoft.com\">LnRSoft.com</a>"));
}

void LoginDialog::onPowerClicked() { reject(); }

QString LoginDialog::username() const {
  return (ui && ui->m_usernameEdit) ? ui->m_usernameEdit->text().trimmed()
                                    : QString();
}

QString LoginDialog::databaseChoice() const {
  return (ui && ui->m_databaseCombo) ? ui->m_databaseCombo->currentText()
                                     : QString();
}
