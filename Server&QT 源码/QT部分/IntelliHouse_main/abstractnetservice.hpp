#ifndef ABSTRACTNETSERVICE_HPP
#define ABSTRACTNETSERVICE_HPP

class AbstractNetService
{
public:
    virtual ~AbstractNetService() {}
    virtual void acceptConnection() = 0;
    virtual void startRead() = 0;
    virtual void startWrite(const QByteArray& data) = 0;
};

#endif // ABSTRACTNETSERVICE_HPP
