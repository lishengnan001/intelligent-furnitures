#ifndef SOUNDSERVICE_HPP
#define SOUNDSERVICE_HPP

#include <QObject>
#include <QProcess>
#include "abstractsoundservice.hpp"

class SoundService : public QObject, public AbstractSoundService
{
    Q_OBJECT

public:
    void play(QString path, bool repeat);
    void stop();
    static SoundService* getService();
    static void releaseService();

private:
    bool isPlaying;
    QProcess *madplayerProcess;
    static SoundService* pService;
    explicit SoundService(QObject *parent = 0);
    explicit SoundService(const SoundService& service);
    SoundService& operator=(const SoundService& service);
    ~SoundService();
    void close();

signals:
    
public slots:
    
};

#endif // SOUNDSERVICE_HPP
