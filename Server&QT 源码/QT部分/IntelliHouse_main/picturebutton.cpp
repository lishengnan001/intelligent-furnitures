#include <QPainter>
#include "picturebutton.hpp"

PictureButton::PictureButton(const QString &path, QWidget *parent/* =0 */)
    : QAbstractButton(parent)
{
    setPixmap(path);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

PictureButton::PictureButton(const QPixmap& picture, QWidget *parent/* = 0*/)
    : QAbstractButton(parent), pixmap(picture)
{
    update();
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

PictureButton::PictureButton(QWidget *parent /* = 0 */)
    : QAbstractButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}


PictureButton::~PictureButton()
{

}

void PictureButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPixmap temp(pixmap.size());
    temp.fill(Qt::transparent);
    QPainter p(&temp);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0, pixmap);
    if (!isEnabled()) {
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(temp.rect(), QColor(0, 0, 0, 128));
    }
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), temp);
}

QSize PictureButton::sizeHint() const
{
    return pixmap.size();
}

void PictureButton::setPixmap(const QString &path)
{
    pixmap.load(path);
    update();
}

void PictureButton::setPixmap(const QPixmap &picture) {
    pixmap = picture;
    update();
}

void PictureButton::enabled() {
    if (!isEnabled()) {
        setEnabled(true);
    }
}

void PictureButton::disabled() {
    if (isEnabled()) {
        setEnabled(false);
    }
}

