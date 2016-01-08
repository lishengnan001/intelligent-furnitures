#ifndef ABSTRACTTELESERVICE_HPP
#define ABSTRACTTELESERVICE_HPP

class AbstractTeleService {
public:
    virtual ~AbstractTeleService() {}
    virtual int AlarmDoppler(QString dest) = 0;
    virtual int AlarmSmoke(QString dest) = 0;
};

#endif // ABSTRACTTELESERVICE_HPP
