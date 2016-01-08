#include <cassert>
#include <algorithm>
#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>
#include "abstractmodule.hpp"
#include "moduleregistry.hpp"
#include "serialservice.hpp"
#include "teleservice.hpp"
#include "soundservice.hpp"
#include "settings.hpp"
#include "coorimpl.hpp"

ModuleRegistry *ModuleRegistry::pReg(0);

ModuleRegistry::ModuleRegistry(QObject *parent) :
    QObject(parent), coors(), libs()
{
}

ModuleRegistry::ModuleRegistry(const ModuleRegistry& reg) :
    QObject(reg.parent()), coors()
{
    assert(false);
}

ModuleRegistry& ModuleRegistry::operator=(const ModuleRegistry& ) {
    assert(false);
    return *this;
}

ModuleRegistry::~ModuleRegistry() {
    libs.clear();
    coors.clear();
}

ModuleRegistry* ModuleRegistry::getRegistry() {
    if (!pReg) {
        pReg = new ModuleRegistry();
    }
    return pReg;
}

void ModuleRegistry::releaseRegistry() {
    if (pReg) {
        delete pReg;
        pReg = 0;
    }
}

void ModuleRegistry::loadLibrary(QString path) {
    Settings *pSettings = Settings::getSettings();
    QString platform = pSettings->getValue("config/platform");
    QDir dir = path;
    QStringList filters;
#if defined(_WS_QWS) || defined(Q_WS_QWS) || defined(Q_WS_X11)
	filters << "*.so" ;
#elif defined(Q_WS_WIN)
	filters << "*.dll" ;
#endif
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString filePath = dir.absoluteFilePath(fileInfo.fileName());
        QPluginLoader *pLoader(new QPluginLoader(filePath, this));
        QObject *pObject = pLoader->instance();
        if (0 != pObject) {
            AbstractModule *pModule = qobject_cast<AbstractModule *>(pObject);
            if (pModule->negotiatePlatform(platform)) {
                CoorImpl *pCoor = new CoorImpl(this);
                pCoor->setModule(pModule);
                coors.append(pCoor);
                libs.append(pLoader);
                continue;
            }
        }
        pLoader->unload();
        delete pLoader;
    }
}

void ModuleRegistry::loadModules() {
    //所有模块构造的时候都应该以pReg对象作为父，
    //这样就可以保证由pReg对象负责释放所有模块
    //对象的内存
    Settings *pSettings = Settings::getSettings();
    QByteArray ids;

    QString pluginsDir;
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    pluginsDir = QCoreApplication::applicationDirPath().append("/plugins/");
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
    pluginsDir = QCoreApplication::applicationDirPath().append("/../plugins/");
#endif
    loadLibrary(pluginsDir);
    pluginsDir = pSettings->getValue("config/libPath");
    if (0 != pluginsDir.size()) {
        loadLibrary(pluginsDir);
    }

    initModules();

    QVector<CoorImpl *>::Iterator iter;
    for (iter = coors.begin(); iter != coors.end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        ids.clear();
        pModule->getSupportedNodeIDs(ids);
        int size = ids.size();
        for (int pos = 0; pos < size; pos++) {
            hash.insertMulti(ids[pos], pModule);
        }
    }
}

ModuleRegistry::Iterator ModuleRegistry::begin() {
    return coors.begin();
}

ModuleRegistry::Iterator ModuleRegistry::end() {
    return coors.end();
}

AbstractModule * ModuleRegistry::getModule(const QString& moduleID) {
    for (Iterator iter = begin(); iter != end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        if (!moduleID.compare(pModule->getID())) {
            return pModule;
        }
    }
    return 0;
}

void ModuleRegistry::findModules(const quint8 nodeID, QVector<AbstractModule *>& modules) {
    QHash<quint8, AbstractModule*>::iterator iter = hash.find(nodeID);
    while (iter != hash.end() && nodeID == iter.key()) {
        modules.append(iter.value());
        iter++;
    }
}

void ModuleRegistry::initModules() {
    ISerialService *pSerialService = SerialService::getService();
    TeleService *pTeleService = TeleService::getService();
    SoundService *pSoundService = SoundService::getService();
    for(Iterator iter = begin(); iter != end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        pModule->setSerialService(pSerialService);
        pModule->setTeleService(pTeleService);
        pModule->setSoundService(pSoundService);
        pModule->createWindow();
    }
}

quint8 ModuleRegistry::count() {
    return coors.count();
}
