#include <cassert>
#include <QString>
#include <QCoreApplication>
#include <QSettings>
#include "settings.hpp"

Settings *Settings::pSetting(0);

Settings::Settings(QObject *parent) :
    QObject(parent), pSettings(0)
{
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    QString path = QCoreApplication::applicationDirPath() + "/";
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
    QString path = QCoreApplication::applicationDirPath() + "/../";
#endif
    pSettings = new QSettings(path.append("config.ini"), QSettings::IniFormat, this);
}

Settings::Settings(const Settings& s) : QObject(s.parent()) {
    assert(false);
}

Settings& Settings::operator=(const Settings& ) {
    assert(false);
    return *this;
}

Settings::~Settings() {

}

QString Settings::getValue(const QString& key) {
    return pSettings->value(key, "").toString();
}

Settings* Settings::getSettings() {
    if (0 == pSetting) {
        pSetting = new Settings();
    }
    return pSetting;
}

void Settings::releaseSettings() {
    if (0 != pSetting) {
        delete pSetting;
        pSetting = 0;
    }
}
