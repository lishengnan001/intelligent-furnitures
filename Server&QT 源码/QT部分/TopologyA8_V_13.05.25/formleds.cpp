#include "formleds.hpp"
#include "ui_formleds.h"

#include <cassert>

#ifdef DEBUG
#include <QDebug>
#endif


FormLeds::FormLeds(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLeds)
{
    ui->setupUi(this);

#if defined(_WS_QWS) || defined(Q_WS_QWS)
    showFullScreen();
#else
    resize(800, 480);
#endif

    QString path = QCoreApplication::applicationDirPath();
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
    path.append("/../images/common/background.png");
#elif defined(Q_WS_QWS)
    path.append("/images/common/background.png");
#endif

    // Following three line for set background image
    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(QPixmap(path)));
    setPalette(palette);

    // Interface initial setup is hidden
    hide();
}

FormLeds::~FormLeds()
{
    delete ui;
}


void FormLeds::on_btnBack_clicked()
{
    emit closeWindow();
}
