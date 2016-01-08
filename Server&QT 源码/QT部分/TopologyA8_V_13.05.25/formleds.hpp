#ifndef FORMLEDS_H
#define FORMLEDS_H

#include <QWidget>
#include <QTimer>

//#include "posix_qextserialport.h"
//#include "common.h"

namespace Ui {
    class FormLeds;
}

class FormLeds : public QWidget
{
    Q_OBJECT

public:
    explicit FormLeds(QWidget *parent = 0);
    ~FormLeds();


private slots:
    void on_btnBack_clicked();

signals:
    void closeWindow();

private:
    Ui::FormLeds *ui;
};

#endif // FORMLEDS_H
