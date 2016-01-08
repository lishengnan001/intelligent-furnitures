#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtGui>

class ImageWidget : public QWidget
{
        Q_OBJECT
public:
        bool bFit;

public:
        ImageWidget(QWidget *parent = 0);
        void setPixmap(QString fileName);
        void setPixmap(QByteArray& bytes);
        void setPixmap(const uchar* buf);


protected:
        void paintEvent(QPaintEvent *event);

private:
        QPixmap pixmap;
        QImage* qImage;
        uchar buffer[320*240*2];

};

#endif // IMAGEWIDGET_H
