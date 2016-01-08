#ifndef SERIALSERVICE_HPP
#define SERIALSERVICE_HPP

#include <QObject>
#include <memory>
#include "qextserialbase.h"
#include "abstractserialservice.hpp"

class QTextStream;
using namespace std;

class ISerialService : public QObject, public AbstractSerialService
{
    Q_OBJECT
public :
    explicit ISerialService(QObject *parent);
    virtual ~ISerialService();
    virtual void lock();
    virtual void release();
    virtual bool isLocked();
    qint64 read(QByteArray& data);
    qint64 write(const QByteArray& data);
protected:
    virtual qint64 innerRead(QByteArray& data) = 0;
    virtual qint64 innerWrite(const QByteArray& data) = 0;
private :
    enum {LOCKED, UNLOCKED};
    quint8 lockState;
    auto_ptr<QTextStream> serialFile;
};

class MockSerial : public ISerialService
{
    Q_OBJECT
public:
    MockSerial(QObject *parent = 0);
    ~MockSerial();
    bool open();
    bool openForRFID();
    void close();
    void reset();
    void lock();
    void release();
    bool isLocked();
protected:
    qint64 innerRead(QByteArray& data);
    qint64 innerWrite(const QByteArray& data);
private:
    auto_ptr<QTextStream> dataFile;
};

class SerialService : public ISerialService
{
    Q_OBJECT
public:
    static ISerialService* getService();
    static void releaseService();
    bool open();
    bool openForRFID();
    void close();
    void reset();
protected:
    qint64 innerRead(QByteArray& data);
    qint64 innerWrite(const QByteArray& data);
private:
    explicit SerialService(QObject *parent = 0);
    explicit SerialService(const SerialService& service);
    virtual ~SerialService();
    virtual SerialService& operator=(const SerialService& service);
    static ISerialService *pService;
    static const QString COM;
    static const quint8 TIME_OUT = 10;
    QextSerialBase *pSerial;
    BaudRateType baud;
    enum {OPEN, CLOSED};
    int portState;
    bool openWithBaud(BaudRateType baud);

signals:
    
public slots:
    
};

#endif // SERIALSERVICE_HPP
