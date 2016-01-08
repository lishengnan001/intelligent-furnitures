/********************************************************************************
** Form generated from reading UI file 'panel.ui'
**
** Created: Fri Jan 23 10:46:59 2015
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANEL_H
#define UI_PANEL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Panel
{
public:

    void setupUi(QWidget *Panel)
    {
        if (Panel->objectName().isEmpty())
            Panel->setObjectName(QString::fromUtf8("Panel"));
        Panel->resize(1200, 480);
        Panel->setMinimumSize(QSize(1200, 480));
        Panel->setMaximumSize(QSize(1200, 480));

        retranslateUi(Panel);

        QMetaObject::connectSlotsByName(Panel);
    } // setupUi

    void retranslateUi(QWidget *Panel)
    {
        Panel->setWindowTitle(QApplication::translate("Panel", "Panel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Panel: public Ui_Panel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANEL_H
