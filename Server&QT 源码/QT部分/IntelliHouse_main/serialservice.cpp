#include <QtGlobal>
#include <iostream>
#include <QDebug>
#include <QByteArray>
#include <cassert>
#include <QCoreApplication>
#include <QFile>
#include "qextserialport.h"
#include "serialservice.hpp"
#include "settings.hpp"
#include "netservice.hpp"
#include "config_etc_cs.h"

using namespace std;

ISerialService *SerialService::pService(0);

#if defined(_WS_QWS) || defined(Q_WS_QWS)
        const QString SerialService::COM("/dev/ttySAC0");
#elif defined(Q_WS_X11)
	const QString SerialService::COM("/dev/ttyS0");
#else
	const QString SerialService::COM("com1");
#endif

ISerialService::ISerialService(QObject *parent) :
    QObject(parent)
{
    Settings *pSettings = Settings::getSettings();
    if (pSettings->getValue("config/debug").compare("0")) {
#if defined(_WS_QWS) || defined(Q_WS_QWS)
        QString path = QCoreApplication::applicationDirPath() + "/";
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
        QString path = QCoreApplication::applicationDirPath() + "/../";
#endif
        serialFile = auto_ptr<QTextStream>(new QTextStream(new QFile(path.append("serial.dat"))));
        if (!serialFile->device()->isOpen()) {
            if (serialFile->device()->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                qDebug() << "Serial data file opened!!" << endl;
            }
            else {
                qDebug() << "Serial data file open failed!!" << endl;
            }
        }
        else {
            qDebug() << "Serial data file already opened!!" << endl;
        }
    }
}

ISerialService::~ISerialService() {
    Settings *pSettings = Settings::getSettings();
    if (pSettings->getValue("config/debug").compare("0")) {
        if (serialFile->device()->isOpen()) {
            serialFile->device()->close();
            qDebug() << "Serial data file closed!!" << endl;
        }
        else {
            qDebug() << "Serial data file already closed!!" << endl;
        }
    }
}

void ISerialService::lock() {
    lockState = LOCKED;
}

void ISerialService::release() {
    lockState = UNLOCKED;
}

bool ISerialService::isLocked() {
    return LOCKED == lockState;
}

qint64 ISerialService::read(QByteArray& data) {
    int ret = innerRead(data);
    if (-1 != ret) {
        Settings *pSettings = Settings::getSettings();
        if (pSettings->getValue("config/debug").compare("0")) {
            if ((0 != strlen(data.toHex())) &&
                 ('a' != data.toHex()[8] && 'a' != data.toHex()[9])) {
                *serialFile << "[read]" << data.toHex() << endl;
            }
        }
   // qDebug()<<"data from coordnator is ..................:"
           // <<data.toHex();
#ifdef ETC_HAS_CENTRAL_SERVER
        NetService *pService = NetService::getService();

        if (pService->socketConnected) {

            if ((0 != strlen(data.toHex())) &&
                 ('a' != data.toHex()[8] && 'a' != data.toHex()[9])) {
                pService->startWrite(data);
                //qDebug()<<"data form coor is : "<<data.toHex();
            }

        }
#endif

    }
    return ret;
}

qint64 ISerialService::write(const QByteArray& data) {
    Settings *pSettings = Settings::getSettings();
    if (pSettings->getValue("config/debug").compare("0")) {
        *serialFile << "[write]" << data.toHex() << endl;
    }
    int ret = innerWrite(data);

    return ret;
}


MockSerial::MockSerial(QObject *parent) :
    ISerialService(parent), dataFile(0)
{
}

MockSerial::~MockSerial() {
    close();
}

bool MockSerial::open() {
    if (0 == dataFile.get()) {
#if defined(_WS_QWS) || defined(Q_WS_QWS)
        QString path = QCoreApplication::applicationDirPath() + "/data.dat";
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
        QString path = QCoreApplication::applicationDirPath() + "/../data.dat";
#endif
        dataFile = auto_ptr<QTextStream>(new QTextStream(new QFile(path)));
    }
    if (!dataFile->device()->isOpen()) {
        if (dataFile->device()->open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Serial opened!!" << endl;
        }
        else {
            qDebug() << "Serial open failed!!" << endl;
        }
    }
    else {
        qDebug() << "Serial already opened!!" << endl;
    }
    return true;
}

bool MockSerial::openForRFID() {
    open();
    qDebug() << "open serial for RFID!!" << endl;
    return true;
}

void MockSerial::close() {
    if (dataFile->device()->isOpen()) {
        dataFile->device()->close();
        qDebug() << "Serial closed!!" << endl;
    }
    else {
        qDebug() << "Serial already closed!!" << endl;
    }
}

qint64 MockSerial::innerRead(QByteArray& data) {
    if (0 != dataFile.get()  && dataFile->device()->isOpen()) {
        data = QByteArray::fromHex(dataFile->readLine(1024).toAscii());
        return data.size();
    }
    else {
        return 0;
    }
}

qint64 MockSerial::innerWrite(const QByteArray& data) {
    qDebug() << data.toHex() << endl;
    return data.length();
}

void MockSerial::reset() {
    qDebug() << "Serial reset!!" << endl;
}

void MockSerial::lock() {
    ISerialService::lock();
    qDebug() << "Serial locked!!" << endl;
}

void MockSerial::release() {
    ISerialService::release();
    qDebug() << "Serial released!!" << endl;
}

bool MockSerial::isLocked() {
    qDebug() << "isLocked!!" << endl;
    return ISerialService::isLocked();
}

SerialService::SerialService(QObject *parent) :
    ISerialService(parent), pSerial(0), baud(BAUD115200), portState(CLOSED)
{
}

SerialService::SerialService(const SerialService& service) :
    ISerialService(service.parent()), pSerial(service.pSerial), baud(service.baud), portState(service.portState)
{
    assert(false);
}

SerialService& SerialService::operator=(const SerialService& ) {
    /*if (this != &service) {
        this->pSerial = service.pSerial;
        this->portState = service.portState;
        this->baud = service.baud;
    }*/
    assert(false);
    return *this;
}

SerialService::~SerialService() {
    if (pSerial) {
        if (CLOSED != portState) {
            pSerial->close();
            portState = CLOSED;
        }
        delete pSerial;
        pSerial = 0;
    }
}

ISerialService* SerialService::getService() {
    if (!pService) {
#if defined(_WS_QWS) || defined(Q_WS_QWS)
        pService = new SerialService();
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
        pService = new MockSerial();
#endif
    }
    return pService;
}

void SerialService::releaseService() {
    if (pService) {
        pService->close();
        delete pService;
        pService = 0;
    }
}

bool SerialService::openWithBaud(BaudRateType baud) {
    if (pSerial) {
        close();
    }
    else {
        pSerial = new QextSerialPort(COM, QextSerialBase::Polling);
    }
    pSerial->open(QIODevice::ReadWrite);
    pSerial->setBaudRate(baud);
    pSerial->setDataBits(DATA_8);
    pSerial->setParity(PAR_NONE);
    pSerial->setStopBits(STOP_1);
    pSerial->setFlowControl(FLOW_OFF);
    pSerial->setTimeout(TIME_OUT);
    portState = OPEN;
    this->baud = baud;
    return true;
}

bool SerialService::open() {
    return openWithBaud(BAUD115200);
}

bool SerialService::openForRFID() {
    return openWithBaud(BAUD19200);
}

void SerialService::close() {
    if (pSerial && CLOSED != portState) {
        pSerial->close();
    }
    portState = CLOSED;
}

qint64 SerialService::innerRead(QByteArray& data) {
    if (pSerial && OPEN == portState) {
        data = pSerial->readAll();
        return data.length();
    }
    return -1;
}

qint64 SerialService::innerWrite(const QByteArray& data) {
    if (pSerial) {
        return pSerial->write(data);
    }
    return -1;
}

void SerialService::reset() {
    openWithBaud(baud);
}
