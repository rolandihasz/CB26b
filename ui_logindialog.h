/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *mainLayout;
    QWidget *topBar;
    QHBoxLayout *topBarLayout;
    QLabel *m_timeLabel;
    QSpacerItem *horizontalSpacerTop;
    QPushButton *m_helpBtn;
    QPushButton *m_powerBtn;
    QWidget *centerWrapper;
    QHBoxLayout *centerWrapperLayout;
    QFrame *formFrame;
    QGridLayout *gridLayout;
    QLabel *userLabel;
    QLineEdit *m_usernameEdit;
    QLabel *passLabel;
    QLineEdit *m_passwordEdit;
    QLabel *dbLabel;
    QComboBox *m_databaseCombo;
    QHBoxLayout *buttonsLayout;
    QSpacerItem *horizontalSpacerButtons;
    QPushButton *m_cancelBtn;
    QPushButton *m_loginBtn;
    QLabel *footerLabel;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(385, 245);
        LoginDialog->setMinimumSize(QSize(385, 245));
        LoginDialog->setMaximumSize(QSize(385, 245));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/database.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        LoginDialog->setWindowIcon(icon);
        LoginDialog->setStyleSheet(QString::fromUtf8("/* ===================================================================\n"
"   COFFEEBREAK TELEMETRY - WORLD CLASS MISSION CONTROL THEME\n"
"   LOGIN DIALOG\n"
"   =================================================================== */\n"
"\n"
"QDialog#LoginDialog {\n"
"    background-color: #0b0f19;\n"
"    color: #f8fafc;\n"
"    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, 'Inter', 'Roboto', sans-serif;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"/* --- Top Utility Bar --- */\n"
"QWidget#topBar {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #151e30, stop:1 #0c121e);\n"
"    border-bottom: 1px solid #1e293b;\n"
"}\n"
"\n"
"QLabel#m_timeLabel {\n"
"    color: #38bdf8;\n"
"    font-family: 'Consolas', 'Segoe UI', monospace;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"    letter-spacing: 0.5px;\n"
"}\n"
"\n"
"QPushButton#m_helpBtn {\n"
"    background-color: #1e293b;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    color: #38bdf8;\n"
"    font-family: 'Segoe UI', sans-serif;\n"
"    font-weight: bold;\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"QPushButton#m_helpBtn:hover {\n"
"    background-color: #27354d;\n"
"    border-color: #38bdf8;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"QPushButton#m_helpBtn:pressed {\n"
"    background-color: #0f172a;\n"
"    border-color: #0284c7;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"QPushButton#m_powerBtn {\n"
"    background-color: #1e293b;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    color: #f87171;\n"
"}\n"
"\n"
"QPushButton#m_powerBtn:hover {\n"
"    background-color: #7f1d1d;\n"
"    border-color: #ef4444;\n"
"}\n"
"\n"
"QPushButton#m_powerBtn:pressed {\n"
"    background-color: #991b1b;\n"
"    border-color: #f87171;\n"
"}\n"
"\n"
"/* --- Card Containers and Panels --- */\n"
"QFrame#formFrame, QFrame[isCard=\"true\"] {\n"
"    background-color: #0f172a;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QFrame#formFrame:hover, QFrame[isCard=\"true\"]:hover {\n"
"    border: 1px solid #334155;\n"
"}\n"
"\n"
"/* --- Form Field Labels --- */\n"
"QLabel#userLabel, QLabel#passLabel, QLabel#dbLabel {\n"
"    color: #94a3b8;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"/* --- Input Controls --- */\n"
"QLineEdit {\n"
"    background-color: #0b0f19;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    padding: 3px 8px;\n"
"    color: #f8fafc;\n"
"    font-family: 'Consolas', 'Courier New', monospace;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"    border-color: #475569;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 1px solid #38bdf8;\n"
"    background-color: #0f172a;\n"
"}\n"
"\n"
"/* --- Database Dropdown --- */\n"
"QComboBox {\n"
"    background-color: #0b0f19;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    padding: 3px 8px;\n"
"    color: #f8fafc;\n"
"    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, sans-serif;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QComboBox:hover {\n"
"    border-color: #38bdf8;\n"
"}\n"
"\n"
"QComboBox:focus {\n"
"    border: 1px solid #38bdf8;\n"
"    background-color: #0f172a;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    border-left: 1px solid #1e293b;\n"
"    border-top-right-radius: 4px;\n"
"    border-bottom-right-radius: 4px;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 5px solid #38bdf8;\n"
"    width: 0px;\n"
"    height: 0px;\n"
"    margin-top: 1px;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #0f172a;\n"
"    color: #f8fafc;\n"
"    border: 1px solid #1e293b;\n"
"    selection-background-color: #1e293b;\n"
"    selection-color: #38bdf8;\n"
"    outline: none;\n"
"    padding: 2px;\n"
"}\n"
"\n"
"/* --- Push Buttons --- */\n"
"QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #222f44, stop:1 #1a2436);\n"
"    color: #f1f5f9;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    padding: 4px 10px;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2d3e5a, stop:1 #223048);\n"
"    border-color: #38bdf8;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: #141d2c;\n"
"    border-color: #0284c7;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"/* Primary Action: Login */\n"
"QPushButton#m_loginBtn {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0284c7, stop:1 #0369a1);\n"
"    border: 1px solid #38bdf8;\n"
"    color: #ffffff;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton#m_loginBtn:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0ea5e9, stop:1 #0284c7);\n"
"    border-color: #7dd3fc;\n"
"}\n"
"\n"
"QPushButton#m_loginBtn:pressed {\n"
"    background: #0369a1;\n"
"    border-color: #0284c7;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"/* Secondary Action: Cancel */\n"
"QPushButton#m_cancelBtn {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #222f44, stop:1 #1a2436);\n"
"    border: 1px solid #334155;\n"
"    color: #cbd5e1;\n"
"}\n"
"\n"
"QPushButton#m_cancelBtn:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2d3e5a, stop:1 #223048);\n"
"    border-color: #f87171;\n"
"    color: #f87171;\n"
"}\n"
"\n"
"QPushButton#m_cancelBtn:pressed {\n"
"    background: #141d2c;\n"
"    border-color: #ef4444;\n"
"    color: #ef4444;\n"
"}\n"
"\n"
"/* --- Footer --- */\n"
"QLabel#footerLabel {\n"
"    color: #64748b;\n"
"    font-size: 10px;\n"
"}\n"
""));
        mainLayout = new QVBoxLayout(LoginDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 8);
        topBar = new QWidget(LoginDialog);
        topBar->setObjectName("topBar");
        topBar->setMinimumSize(QSize(0, 28));
        topBar->setMaximumSize(QSize(16777215, 28));
        topBarLayout = new QHBoxLayout(topBar);
        topBarLayout->setSpacing(4);
        topBarLayout->setObjectName("topBarLayout");
        topBarLayout->setContentsMargins(8, 0, 4, 0);
        m_timeLabel = new QLabel(topBar);
        m_timeLabel->setObjectName("m_timeLabel");

        topBarLayout->addWidget(m_timeLabel);

        horizontalSpacerTop = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topBarLayout->addItem(horizontalSpacerTop);

        m_helpBtn = new QPushButton(topBar);
        m_helpBtn->setObjectName("m_helpBtn");
        m_helpBtn->setMinimumSize(QSize(24, 24));
        m_helpBtn->setMaximumSize(QSize(24, 24));
        m_helpBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        topBarLayout->addWidget(m_helpBtn);

        m_powerBtn = new QPushButton(topBar);
        m_powerBtn->setObjectName("m_powerBtn");
        m_powerBtn->setMinimumSize(QSize(24, 24));
        m_powerBtn->setMaximumSize(QSize(24, 24));
        m_powerBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/power-switch.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_powerBtn->setIcon(icon1);
        m_powerBtn->setIconSize(QSize(14, 14));

        topBarLayout->addWidget(m_powerBtn);


        mainLayout->addWidget(topBar);

        centerWrapper = new QWidget(LoginDialog);
        centerWrapper->setObjectName("centerWrapper");
        centerWrapperLayout = new QHBoxLayout(centerWrapper);
        centerWrapperLayout->setObjectName("centerWrapperLayout");
        centerWrapperLayout->setContentsMargins(14, 0, 14, 0);
        formFrame = new QFrame(centerWrapper);
        formFrame->setObjectName("formFrame");
        formFrame->setFrameShape(QFrame::Shape::StyledPanel);
        formFrame->setFrameShadow(QFrame::Shadow::Raised);
        formFrame->setProperty("isCard", QVariant(true));
        gridLayout = new QGridLayout(formFrame);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(8);
        gridLayout->setContentsMargins(16, 14, 16, 12);
        userLabel = new QLabel(formFrame);
        userLabel->setObjectName("userLabel");
        userLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(userLabel, 0, 0, 1, 1);

        m_usernameEdit = new QLineEdit(formFrame);
        m_usernameEdit->setObjectName("m_usernameEdit");
        m_usernameEdit->setMinimumSize(QSize(0, 24));
        m_usernameEdit->setMaximumSize(QSize(16777215, 24));

        gridLayout->addWidget(m_usernameEdit, 0, 1, 1, 1);

        passLabel = new QLabel(formFrame);
        passLabel->setObjectName("passLabel");
        passLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(passLabel, 1, 0, 1, 1);

        m_passwordEdit = new QLineEdit(formFrame);
        m_passwordEdit->setObjectName("m_passwordEdit");
        m_passwordEdit->setMinimumSize(QSize(0, 24));
        m_passwordEdit->setMaximumSize(QSize(16777215, 24));
        m_passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(m_passwordEdit, 1, 1, 1, 1);

        dbLabel = new QLabel(formFrame);
        dbLabel->setObjectName("dbLabel");
        dbLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(dbLabel, 2, 0, 1, 1);

        m_databaseCombo = new QComboBox(formFrame);
        QIcon iconDb;
        iconDb.addFile(QString::fromUtf8(":/icons/icons/database.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_databaseCombo->addItem(iconDb, QString());
        m_databaseCombo->setObjectName("m_databaseCombo");
        m_databaseCombo->setMinimumSize(QSize(0, 24));
        m_databaseCombo->setMaximumSize(QSize(16777215, 24));

        gridLayout->addWidget(m_databaseCombo, 2, 1, 1, 1);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setSpacing(8);
        buttonsLayout->setObjectName("buttonsLayout");
        horizontalSpacerButtons = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        buttonsLayout->addItem(horizontalSpacerButtons);

        m_cancelBtn = new QPushButton(formFrame);
        m_cancelBtn->setObjectName("m_cancelBtn");
        m_cancelBtn->setMinimumSize(QSize(86, 26));
        m_cancelBtn->setMaximumSize(QSize(86, 26));
        m_cancelBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/exit.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_cancelBtn->setIcon(icon2);
        m_cancelBtn->setIconSize(QSize(16, 16));

        buttonsLayout->addWidget(m_cancelBtn);

        m_loginBtn = new QPushButton(formFrame);
        m_loginBtn->setObjectName("m_loginBtn");
        m_loginBtn->setMinimumSize(QSize(86, 26));
        m_loginBtn->setMaximumSize(QSize(86, 26));
        m_loginBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/home.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_loginBtn->setIcon(icon3);
        m_loginBtn->setIconSize(QSize(16, 16));

        buttonsLayout->addWidget(m_loginBtn);


        gridLayout->addLayout(buttonsLayout, 3, 0, 1, 2);


        centerWrapperLayout->addWidget(formFrame);


        mainLayout->addWidget(centerWrapper);

        footerLabel = new QLabel(LoginDialog);
        footerLabel->setObjectName("footerLabel");
        footerLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        footerLabel->setOpenExternalLinks(true);

        mainLayout->addWidget(footerLabel);


        retranslateUi(LoginDialog);

        m_loginBtn->setDefault(true);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "CoffeeBreak Login", nullptr));
        m_timeLabel->setText(QCoreApplication::translate("LoginDialog", "Sat Oct 17 2026 14:11:08", nullptr));
#if QT_CONFIG(tooltip)
        m_helpBtn->setToolTip(QCoreApplication::translate("LoginDialog", "Information & Help", nullptr));
#endif // QT_CONFIG(tooltip)
        m_helpBtn->setText(QCoreApplication::translate("LoginDialog", "?", nullptr));
#if QT_CONFIG(tooltip)
        m_powerBtn->setToolTip(QCoreApplication::translate("LoginDialog", "Exit CoffeeBreak", nullptr));
#endif // QT_CONFIG(tooltip)
        m_powerBtn->setText(QString());
        userLabel->setText(QCoreApplication::translate("LoginDialog", "Username:", nullptr));
        m_usernameEdit->setText(QCoreApplication::translate("LoginDialog", "admin", nullptr));
        passLabel->setText(QCoreApplication::translate("LoginDialog", "Password:", nullptr));
        m_passwordEdit->setText(QCoreApplication::translate("LoginDialog", "admin", nullptr));
        m_passwordEdit->setPlaceholderText(QCoreApplication::translate("LoginDialog", "admin", nullptr));
        dbLabel->setText(QCoreApplication::translate("LoginDialog", "Database:", nullptr));
        m_databaseCombo->setItemText(0, QCoreApplication::translate("LoginDialog", "SQLite Local DB", nullptr));

        m_cancelBtn->setText(QCoreApplication::translate("LoginDialog", "Cancel", nullptr));
        m_loginBtn->setText(QCoreApplication::translate("LoginDialog", "Login", nullptr));
        footerLabel->setText(QCoreApplication::translate("LoginDialog", "CoffeeBreak \302\251 2019 | <a href=\"https://lnrsoft.com\" style=\"color: #38bdf8; text-decoration: none;\">LnRSoft.com</a>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
