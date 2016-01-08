#include "imagewidget.h"
#include <QtCore>
#include <QtDebug>

ImageWidget::ImageWidget(QWidget *parent)
        : QWidget(parent)
{
        QDesktopWidget desktop;
        pixmap = QPixmap(desktop.width(), desktop.height());

        bFit = true;

    setAcceptDrops(true);
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        if(bFit)
        {
                QPixmap fitPixmap = pixmap.scaled(width(),height(), Qt::KeepAspectRatio);
                painter.drawPixmap(0, 0, fitPixmap);
        }
        else
                painter.drawPixmap(0, 0, pixmap);

}

void ImageWidget::setPixmap(QString fileName)
{
        pixmap.load(fileName);

        update();
}

void ImageWidget::setPixmap(QByteArray& bytes)
{
        pixmap.loadFromData(bytes, "bmp", NULL);
        update();
}

void ImageWidget::setPixmap(const uchar* buf)
{
        pixmap.loadFromData(buf, 800*480*4+54, "bmp", NULL);
        update();
}
