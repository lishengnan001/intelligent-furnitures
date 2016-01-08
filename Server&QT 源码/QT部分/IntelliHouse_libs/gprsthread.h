#ifndef GPRSTHREAD_H
#define GPRSTHREAD_H

#include <QThread>
#include <QProcess>
#include "teleservice.h"
class GprsThread : public QThread
{
public:
    GprsThread(QObject *parent = 0);
    ~GprsThread();
    virtual void run();
    void setRun();
    void clearRun();
    void setPhoneNumber(QString number);
private:
    TeleService *teleservice;
    bool isRuning;
    QString phoneNumber;
};

#endif // GPRSTHREAD_H
