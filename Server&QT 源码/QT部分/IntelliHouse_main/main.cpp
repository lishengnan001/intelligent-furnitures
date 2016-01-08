#include <QtGlobal>
#include <QtGui/QApplication>
#include <cassert>
#include "container.hpp"
#include "moduleregistry.hpp"
#include "serialservice.hpp"
#include "teleservice.hpp"
#include "soundservice.hpp"
#include "settings.hpp"
#include "netservice.hpp"
#include "config_etc_cs.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Settings::getSettings();
    ModuleRegistry* pReg(ModuleRegistry::getRegistry());
    pReg->loadModules();
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    Container container(0, Qt::Tool | Qt::FramelessWindowHint);
    container.move(0, 0);
#else
    Container container;
#endif
    container.show();
#if defined(_WS_QWS) || defined(Q_WS_QWS) || defined(Q_WS_X11)
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    QString path = QCoreApplication::applicationDirPath() + "/sound/welcome.mp3";
#elif defined(Q_WS_X11)
    QString path = QCoreApplication::applicationDirPath() + "/../sound/welcome.mp3";
#endif
    SoundService *pSoundService = SoundService::getService();
    pSoundService->play(path, false);
#endif

#if defined(ETC_HAS_CENTRAL_SERVER)
    NetService *pNetService = NetService::getService();
    pNetService->socketConnected = false;
#endif

    int ret = app.exec();
    SerialService::releaseService();
    TeleService::releaseService();
    SoundService::releaseService();
    Settings::releaseSettings();
    ModuleRegistry::releaseRegistry();
    NetService::releaseService();

    return ret;
}
