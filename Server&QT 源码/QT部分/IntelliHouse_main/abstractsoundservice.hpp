#ifndef ABSTRACTSOUNDSERVICE_HPP
#define ABSTRACTSOUNDSERVICE_HPP

class AbstractSoundService
{
public:
    virtual ~AbstractSoundService() {}
    virtual void play(QString path, bool repeat) = 0;
    virtual void stop() = 0;
};

#endif // ABSTRACTSOUNDSERVICE_HPP
