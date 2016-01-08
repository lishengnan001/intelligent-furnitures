#include <cassert>
#include <QtGui>
#include <QWidget>
#include <QCoreApplication>
#include <QDebug>
#include "linkage.h"
#include "formlinkage.h"
#include "abstractserialservice.hpp"


QByteArray LinkageModule::bytes("\xCC\xEE\x01\x09\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray LinkageModule::fingerbytes("\xCC\xEE\x01\x33\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray LinkageModule::rfidRechargebytes("\xCC\xEE\x01\x10\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
LinkageModule::LinkageModule(QObject *parent) :
    Module(parent)
{
    id = "LinkageModule";
    uartKey = false;
}

LinkageModule::LinkageModule(const LinkageModule& module) : Module(module.parent())
{
    assert(false);
}

LinkageModule LinkageModule::operator=(const LinkageModule&)
{
    assert(false);
    return *this;
}

LinkageModule::~LinkageModule()
{
}

void LinkageModule::getImage(QPixmap& pixmap)
{
    QString path = QCoreApplication::applicationDirPath();
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
    path.append("/../images/leds/Linkage.png");
#elif defined(Q_WS_QWS)
    path.append("/images/leds/Linkage.png");
#endif
    pixmap.load(path);
}

void LinkageModule::getSupportedNodeIDs(QByteArray& ids)
{
    ids.append(0x03);
    ids.append(0x08);
    ids.append(0x09);
    ids.append(0x02);
    ids.append(0x0f);
    ids.append(0x12);
    ids.append(0x21);
    ids.append(0x0c);
    ids.append(0x33);
    ids.append(0x32);
    ids.append(0x10);
}

void LinkageModule::createWindow(QWidget *pParentWindow, const QRect *pRect)
{
    if (0 == form) {
        Module::createWindow(pParentWindow, pRect);
        form = new FormLinkage(/*0,*/pParentWindow);
        if (0 != pRect) {
            form->setGeometry(*pRect);
        }

        uartKey = true;
        connect(form, SIGNAL(closeWindow()), this, SLOT(onDeactivate()));
        connect(form, SIGNAL(fingerRecord()), this, SLOT(fingerprintRecord()));
        connect(form, SIGNAL(fingerSearch()), this, SLOT(fingerprintSearch()));
        connect(form, SIGNAL(Recharge(quint32)), this, SLOT(rfidRecharge(quint32)));
        connect(form, SIGNAL(Pay(quint32)), this, SLOT(rfidPay(quint32)));
        qDebug()<<"conneted signal and slots......................."<<endl;
    }
}

bool LinkageModule::negotiatePlatform(QString platform)
{
    return !platform.compare("2530");
}

void LinkageModule::onDeactivate()
{
    moduleDeactivate();
    uartKey = false;
}



bool LinkageModule::onMsg(const QByteArray& msg)
{
    int value;
    int setValue;
    int rvalue;
    size_t RBufSize = 16;
    QString rstr;
    char rbuf[RBufSize];
    if (uartKey){
    Module::onMsg(msg);  // Ensure what parent task to do
    qDebug() <<"data of msg is .................:"<<QByteArray(msg).toHex()<<endl;
    FormLinkage *pForm = dynamic_cast<FormLinkage*>(form);
    if (0x01 == msg[4]){
        switch (msg[3]){
        case 0x02:
            value = msg[5] * 256 + msg[6];
            if (value > 0 && value < 2000)
            {
                if (value > 1000)
                    pForm->setPhotoRsister(true);
                else
                    pForm->setPhotoRsister(false);
              //qDebug()<<"signal is sended*********************"<<endl;
               //pForm->setPhotoRsister(QString::number(value, 'f', 1));
             }
            break;

        case 0x12:
            if(0x01 == msg[5])
                pForm->setFlameStates(false);
            else
                pForm->setFlameStates(true);
            break;
        case 0x08:
            if(0x01 == msg[5])
                pForm->setSomeoneState(true);
            else
                pForm->setSomeoneState(false);
            break;

        case 0x32:
            if (0x01 == msg[4]) {
                //FormRfid *pForm = dynamic_cast<FormRfid*>(form);
                size_t BufSize = 16;
                char buf[BufSize];
                ::snprintf(buf, BufSize, "%d%d%d%d\n", msg[5], msg[6], msg[7], msg[8]);
                qDebug()<<"buf:"<<buf<<endl;
                QString str = QString::fromAscii(buf);
                 qDebug()<<"str:"<<str<<endl;
                pForm->setRfidValue(str);
               // pForm->setRfidValue(QString::number(str, 'f', 1));
                qDebug() << ".......bbbbbbbbbbb......value = " << value << endl;
            }
            break;

        case 0x0f:
            value = (msg[5] * 256 + msg[6]);
            qDebug()<<"signal is sended**********************"<<endl;
            pForm->setDistance(QString::number(value, 'f', 1));
            break;

        case 0x03:
            value = (msg[5] << 8) + msg[6];
            setValue = value / 100;
            emit setTemp(setValue);
            qDebug()<<"signal is sended***************************"<<endl;
            pForm->setTempValue(QString::number(value/100.0, 'f', 1));
            value = (msg[7] << 8) + msg[8];
            setValue = value / 100;
            emit setHum(setValue);
            pForm->setHumValue(QString::number(value/100.0, 'f', 1));
            break;
         case 0x10:

            ::snprintf(rbuf, RBufSize, "%d%d%d%d\n", msg[5], msg[6], msg[7], msg[8]);
          rstr = QString::fromAscii(rbuf);
          pForm->setID(rstr);
          qDebug() << ".......RRRRRRRRRRRRRRRRRRRRRRRRRRR......rstr = " <<rstr<<endl;
            rvalue = (msg[9] << 8) + msg[10];

           pForm->setTrue();
            pForm->setPrice(QString::number(rvalue, 'd', 1));
            qDebug() << ".......bbbbbbbbbbb......value = " << rvalue << endl;
//

            if (0xDD == msg[4]) {
                 rvalue = msg[9] + msg[10];
                pForm->setPrice(QString::number(rvalue, 'd', 1));
                qDebug() << "........ccccccccccccc...value = " << rvalue << endl;
            }


            break;
        default :
                break;
        }
    }
        if (0xdd == msg[4]){
            qDebug()<<"signal is ***************************"<<endl;
            char fingerValue;
            if(0x33 == msg[3])
                {
                qDebug()<<"signal is  22***************************"<<endl;
                if(msg[5] == 0x02 && msg[6]  == 0x03 )
                    {
    //                value = (msg[5] << 8) + msg[6];
    //                setValue = value / 100;
    //                emit setTemp(setValue);
                    qDebug()<<"signal is sended come***************************"<<endl;

                   fingerValue = msg[7];
                   if(msg[7]<160 && msg[7]>0)
                   {
                        pForm->setfingerText(QString::number(fingerValue));
                   }
                }


            }
        }
  //  }

    return true;
}
}

void LinkageModule::onLedLight(quint8 cmd)
{
    if (1 > cmd || 10 < cmd) {
        return ;
    }
    switch(cmd) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        bytes[4] = cmd;
        break;
    case 9:
        bytes[4] = 0x0c;
        break;
    case 10:
        bytes[4] = 0x0d;
        break;
    default:
        break;
    }

    AbstractSerialService *pService = getSerialService();
    pService->write(bytes);
}

void LinkageModule::onMotorOperation(quint8 cmd)
{
    if (1 > cmd || 10 < cmd) {
        return ;
    }
    switch(cmd) {
    case 9:
        bytes[4] = 0x09;
        break;
    case 10:
        bytes[4] = 0x0b;
        break;
    default:
        break;
    }

    AbstractSerialService *pService = getSerialService();
    pService->write(bytes);
}


void LinkageModule::fingerprintRecord()
{
    qDebug()<<"come in"<<endl;
    fingerbytes[4] = 0x01;
    AbstractSerialService *pService = getSerialService();
    qDebug()<<"come in..........."<<fingerbytes.toHex()<<endl;
    pService->write(fingerbytes);
}

void LinkageModule::fingerprintSearch()
{
    fingerbytes[4] = 0x02;
    AbstractSerialService *pService = getSerialService();
    pService->write(fingerbytes);
}

void LinkageModule::rfidRecharge(quint32 price){
        qDebug()<<"come in"<<endl;
        rfidRechargebytes[4] = 0x02;
        rfidRechargebytes[5] = price >> 8;
        rfidRechargebytes[6] = price;
        AbstractSerialService *pService = getSerialService();
        qDebug()<<"come in..........."<<rfidRechargebytes.toHex()<<endl;
        pService->write(rfidRechargebytes);
}

void LinkageModule::rfidPay(quint32 value){
    qDebug()<<"come in"<<endl;
    rfidRechargebytes[4] = 0x03;
    rfidRechargebytes[5] = value >> 8;
    rfidRechargebytes[6] = value;
    AbstractSerialService *pService = getSerialService();
    qDebug()<<"come in..........."<<rfidRechargebytes.toHex()<<endl;
    pService->write(rfidRechargebytes);
}


Q_EXPORT_PLUGIN2(Linkage, LinkageModule);

