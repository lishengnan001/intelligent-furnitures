#ifndef PICTUREBUTTON_H
#define PICTUREBUTTON_H

#include <QAbstractButton>

class PictureButton : public QAbstractButton
{
    Q_OBJECT
private:
    QPixmap pixmap;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual QSize sizeHint() const;
public:
    PictureButton(const QString &path, QWidget *parent=0);
    PictureButton(QWidget *parent = 0);
    ~PictureButton();
    void setPixmap(const QString &path);
};

#endif // PICTUREBUTTON_H
