/********************************************************************************
** Form generated from reading UI file 'container.ui'
**
** Created: Fri Jan 23 10:46:59 2015
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTAINER_H
#define UI_CONTAINER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Container
{
public:
    QLabel *backGround;
    QWidget *location;
    QLabel *SlidePanel;
    QLabel *labelAlive;
    QLabel *article1;
    QLabel *article2;
    QLabel *logo3;
    QPushButton *buildServer;
    QLabel *serverState;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *logo1;
    QLabel *logo2;

    void setupUi(QWidget *Container)
    {
        if (Container->objectName().isEmpty())
            Container->setObjectName(QString::fromUtf8("Container"));
        Container->resize(800, 480);
        backGround = new QLabel(Container);
        backGround->setObjectName(QString::fromUtf8("backGround"));
        backGround->setGeometry(QRect(0, 0, 801, 481));
        backGround->setPixmap(QPixmap(QString::fromUtf8(":/image/background1.png")));
        backGround->setScaledContents(true);
        location = new QWidget(Container);
        location->setObjectName(QString::fromUtf8("location"));
        location->setGeometry(QRect(180, 250, 211, 91));
        SlidePanel = new QLabel(Container);
        SlidePanel->setObjectName(QString::fromUtf8("SlidePanel"));
        SlidePanel->setGeometry(QRect(60, 420, 480, 40));
        labelAlive = new QLabel(Container);
        labelAlive->setObjectName(QString::fromUtf8("labelAlive"));
        labelAlive->setGeometry(QRect(710, 70, 29, 25));
        article1 = new QLabel(Container);
        article1->setObjectName(QString::fromUtf8("article1"));
        article1->setGeometry(QRect(280, 110, 201, 61));
        QFont font;
        font.setPointSize(36);
        article1->setFont(font);
        article2 = new QLabel(Container);
        article2->setObjectName(QString::fromUtf8("article2"));
        article2->setGeometry(QRect(110, 170, 661, 61));
        article2->setFont(font);
        logo3 = new QLabel(Container);
        logo3->setObjectName(QString::fromUtf8("logo3"));
        logo3->setGeometry(QRect(10, 400, 201, 71));
        logo3->setPixmap(QPixmap(QString::fromUtf8(":/image/logo3.png")));
        logo3->setScaledContents(true);
        buildServer = new QPushButton(Container);
        buildServer->setObjectName(QString::fromUtf8("buildServer"));
        buildServer->setGeometry(QRect(420, 250, 80, 80));
        buildServer->setMinimumSize(QSize(80, 80));
        buildServer->setMaximumSize(QSize(80, 80));
        buildServer->setSizeIncrement(QSize(0, 0));
        buildServer->setMouseTracking(false);
        buildServer->setAcceptDrops(false);
        buildServer->setAutoFillBackground(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/image/icon/server.png"), QSize(), QIcon::Normal, QIcon::Off);
        buildServer->setIcon(icon);
        buildServer->setIconSize(QSize(80, 80));
        buildServer->setAutoExclusive(false);
        buildServer->setAutoDefault(false);
        buildServer->setDefault(false);
        buildServer->setFlat(true);
        serverState = new QLabel(Container);
        serverState->setObjectName(QString::fromUtf8("serverState"));
        serverState->setGeometry(QRect(420, 320, 101, 21));
        QFont font1;
        font1.setPointSize(14);
        serverState->setFont(font1);
        layoutWidget = new QWidget(Container);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 150, 66));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        logo1 = new QLabel(layoutWidget);
        logo1->setObjectName(QString::fromUtf8("logo1"));
        logo1->setPixmap(QPixmap(QString::fromUtf8(":/image/logo1.png")));
        logo1->setScaledContents(true);

        horizontalLayout->addWidget(logo1);

        logo2 = new QLabel(layoutWidget);
        logo2->setObjectName(QString::fromUtf8("logo2"));
        logo2->setPixmap(QPixmap(QString::fromUtf8(":/image/logo2.png")));
        logo2->setScaledContents(true);

        horizontalLayout->addWidget(logo2);


        retranslateUi(Container);

        QMetaObject::connectSlotsByName(Container);
    } // setupUi

    void retranslateUi(QWidget *Container)
    {
        Container->setWindowTitle(QApplication::translate("Container", "Container", 0, QApplication::UnicodeUTF8));
        backGround->setText(QString());
        SlidePanel->setText(QString());
        labelAlive->setText(QApplication::translate("Container", "TextLabel", 0, QApplication::UnicodeUTF8));
        article1->setText(QApplication::translate("Container", "\344\270\255\350\275\257\345\233\275\351\231\205", 0, QApplication::UnicodeUTF8));
        article2->setText(QApplication::translate("Container", "\347\211\251\350\201\224\347\275\221&\347\247\273\345\212\250\344\272\222\350\201\224\347\273\274\345\220\210\345\256\236\350\256\255\345\271\263\345\217\260", 0, QApplication::UnicodeUTF8));
        logo3->setText(QString());
        buildServer->setText(QString());
        buildServer->setShortcut(QString());
        serverState->setText(QApplication::translate("Container", "Server start", 0, QApplication::UnicodeUTF8));
        logo1->setText(QString());
        logo2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Container: public Ui_Container {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTAINER_H
