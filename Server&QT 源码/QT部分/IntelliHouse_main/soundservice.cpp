#include <QtGlobal>
#include <QProcess>
#include <QDebug>
#include <cassert>
#include "soundservice.hpp"

SoundService *SoundService::pService(0);

SoundService::SoundService(QObject *parent) :
    QObject(parent), isPlaying(false), madplayerProcess(new QProcess(this))
{
}

SoundService::SoundService(const SoundService& ) :
    QObject(0)
{
    assert(false);
}

SoundService& SoundService::operator=(const SoundService& ) {
    assert(false);
    return *this;
}

SoundService::~SoundService() {
    close();
}

void SoundService::close() {
    if (madplayerProcess && isPlaying) {
        madplayerProcess->close();
        isPlaying = false;
    }

}

void SoundService::play(QString path, bool repeat) {
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    QStringList args;
    if (repeat) {
        args << "-r";
    }
    args << path;
    assert(madplayerProcess);
    madplayerProcess->start("madplay",args);
#else
    qDebug() << "begin playing sound " << path;
    if (repeat) {
        qDebug() << " and repeat is true";
    }
    qDebug() << endl;
#endif
    isPlaying = true;
}

void SoundService::stop() {
    close();
}

SoundService* SoundService::getService() {
    if (!pService) {
        pService = new SoundService();
    }
    return pService;
}

void SoundService::releaseService() {
    if (pService) {
        delete pService;
        pService = 0;
    }
}
