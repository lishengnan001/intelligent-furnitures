#include "gprsthread.h"
#include <QDebug>


GprsThread::GprsThread(QObject *parent) : QThread(parent)
{

    isRuning = false;
    teleservice = new TeleService(this);
}

GprsThread::~GprsThread()
{
    wait();

}

void GprsThread::run()
{
   // qDebug( "Ping!" );

    for ( ;; ){
        while(isRuning){
            qDebug()<<"send gprs warning!number of phone is........................................."<<phoneNumber<<endl;
            teleservice->AlarmDoppler(phoneNumber);
            //qDebug( "Ping!" );
            msleep(1000);
        }
       //qDebug( "Ping!" );
        msleep(1000);
    }
}

void GprsThread::setRun()
{
    isRuning = true;
}

void GprsThread::clearRun()
{
    isRuning = false;
}

void GprsThread::setPhoneNumber(QString number)
{
    phoneNumber = number;
}
