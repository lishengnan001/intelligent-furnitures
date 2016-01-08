#ifndef ABSTRACTSERIALSERVICE_HPP
#define ABSTRACTSERIALSERVICE_HPP

class AbstractSerialService {
public :
    virtual ~AbstractSerialService() {}
    virtual bool open() = 0;
    virtual bool openForRFID() = 0;
    virtual void close() = 0;
    virtual void reset() = 0;
    virtual void lock() = 0;
    virtual void release() = 0;
    virtual bool isLocked() = 0;
    virtual qint64 read(QByteArray& data) = 0;
    virtual qint64 write(const QByteArray& data) = 0;
};


#endif // ABSTRACTSERIALSERVICE_HPP
