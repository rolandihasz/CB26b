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
        icon.addFile(QString::fromUtf8(":/icons/icons/database.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        LoginDialog->setWindowIcon(icon);
        LoginDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"    background-color: #e4e4e4;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}"));
        mainLayout = new QVBoxLayout(LoginDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 8);
        topBar = new QWidget(LoginDialog);
        topBar->setObjectName("topBar");
        topBar->setMinimumSize(QSize(0, 26));
        topBar->setMaximumSize(QSize(16777215, 26));
        topBar->setStyleSheet(QString::fromUtf8("QWidget#topBar {\n"
"    background-color: #8f9296;\n"
"    border-bottom: 1px solid #7d8084;\n"
"}"));
        topBarLayout = new QHBoxLayout(topBar);
        topBarLayout->setSpacing(0);
        topBarLayout->setObjectName("topBarLayout");
        topBarLayout->setContentsMargins(8, 0, 0, 0);
        m_timeLabel = new QLabel(topBar);
        m_timeLabel->setObjectName("m_timeLabel");
        m_timeLabel->setStyleSheet(QString::fromUtf8("color: #000000;\n"
"font-family: 'Segoe UI', Arial, sans-serif;\n"
"font-size: 13px;\n"
"font-weight: normal;"));

        topBarLayout->addWidget(m_timeLabel);

        horizontalSpacerTop = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topBarLayout->addItem(horizontalSpacerTop);

        m_helpBtn = new QPushButton(topBar);
        m_helpBtn->setObjectName("m_helpBtn");
        m_helpBtn->setMinimumSize(QSize(26, 26));
        m_helpBtn->setMaximumSize(QSize(26, 26));
        m_helpBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        m_helpBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #6fe36f;\n"
"    border: 1px solid #489e48;\n"
"    border-right: none;\n"
"    color: #000000;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #5bd45b;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #4ac04a;\n"
"}"));

        topBarLayout->addWidget(m_helpBtn);

        m_powerBtn = new QPushButton(topBar);
        m_powerBtn->setObjectName("m_powerBtn");
        m_powerBtn->setMinimumSize(QSize(26, 26));
        m_powerBtn->setMaximumSize(QSize(26, 26));
        m_powerBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        m_powerBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #ea5d5d;\n"
"    border: 1px solid #ba3838;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #dc4b4b;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #cb3939;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/power-switch.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_powerBtn->setIcon(icon1);
        m_powerBtn->setIconSize(QSize(16, 16));

        topBarLayout->addWidget(m_powerBtn);


        mainLayout->addWidget(topBar);

        centerWrapper = new QWidget(LoginDialog);
        centerWrapper->setObjectName("centerWrapper");
        centerWrapperLayout = new QHBoxLayout(centerWrapper);
        centerWrapperLayout->setObjectName("centerWrapperLayout");
        centerWrapperLayout->setContentsMargins(14, 0, 14, 0);
        formFrame = new QFrame(centerWrapper);
        formFrame->setObjectName("formFrame");
        formFrame->setStyleSheet(QString::fromUtf8("QFrame#formFrame {\n"
"    background-color: #ededed;\n"
"    border: 1px solid #b8b8b8;\n"
"    border-radius: 2px;\n"
"}\n"
"QLabel {\n"
"    color: #000000;\n"
"    font-size: 12px;\n"
"}\n"
"QLineEdit {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #999999;\n"
"    border-radius: 2px;\n"
"    padding: 2px 4px;\n"
"    color: #000000;\n"
"    font-size: 12px;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 1px solid #0078d7;\n"
"}\n"
"QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #999999;\n"
"    border-radius: 2px;\n"
"    padding: 2px 6px;\n"
"    color: #000000;\n"
"    font-size: 12px;\n"
"}\n"
"QComboBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    border-left: 1px solid #c8c8c8;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 5px solid #444444;\n"
"    width: 0px;\n"
"    height: 0px;\n"
"    margin"
                        "-top: 1px;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    selection-background-color: #0078d7;\n"
"    selection-color: #ffffff;\n"
"}"));
        formFrame->setFrameShape(QFrame::Shape::StyledPanel);
        formFrame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout = new QGridLayout(formFrame);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(8);
        gridLayout->setContentsMargins(16, 14, 16, 12);
        userLabel = new QLabel(formFrame);
        userLabel->setObjectName("userLabel");
        userLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(userLabel, 0, 0, 1, 1);

        m_usernameEdit = new QLineEdit(formFrame);
        m_usernameEdit->setObjectName("m_usernameEdit");
        m_usernameEdit->setMinimumSize(QSize(0, 24));
        m_usernameEdit->setMaximumSize(QSize(16777215, 24));

        gridLayout->addWidget(m_usernameEdit, 0, 1, 1, 1);

        passLabel = new QLabel(formFrame);
        passLabel->setObjectName("passLabel");
        passLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(passLabel, 1, 0, 1, 1);

        m_passwordEdit = new QLineEdit(formFrame);
        m_passwordEdit->setObjectName("m_passwordEdit");
        m_passwordEdit->setMinimumSize(QSize(0, 24));
        m_passwordEdit->setMaximumSize(QSize(16777215, 24));
        m_passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(m_passwordEdit, 1, 1, 1, 1);

        dbLabel = new QLabel(formFrame);
        dbLabel->setObjectName("dbLabel");
        dbLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(dbLabel, 2, 0, 1, 1);

        m_databaseCombo = new QComboBox(formFrame);
        m_databaseCombo->addItem(icon, QString());
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
        m_cancelBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #fbfbfb;\n"
"    border: 1px solid #adadad;\n"
"    border-radius: 3px;\n"
"    color: #000000;\n"
"    font-size: 12px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #e5f1fb;\n"
"    border-color: #0078d7;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #cce4f7;\n"
"}"));
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
        m_loginBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #fbfbfb;\n"
"    border: 1px solid #adadad;\n"
"    border-radius: 3px;\n"
"    color: #000000;\n"
"    font-size: 12px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #e5f1fb;\n"
"    border-color: #0078d7;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #cce4f7;\n"
"}"));
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
        footerLabel->setStyleSheet(QString::fromUtf8("color: #000000;\n"
"font-size: 11px;"));
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
        passLabel->setText(QCoreApplication::translate("LoginDialog", "Password:", nullptr));
        dbLabel->setText(QCoreApplication::translate("LoginDialog", "Database:", nullptr));
        m_databaseCombo->setItemText(0, QCoreApplication::translate("LoginDialog", "SQLite Local DB", nullptr));

        m_cancelBtn->setText(QCoreApplication::translate("LoginDialog", "Cancel", nullptr));
        m_loginBtn->setText(QCoreApplication::translate("LoginDialog", "Login", nullptr));
        footerLabel->setText(QCoreApplication::translate("LoginDialog", "CoffeeBreak \302\251 2019 | <a href=\"https://lnrsoft.com\" style=\"color: #0078d7; text-decoration: none;\">LnRSoft.com</a>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
