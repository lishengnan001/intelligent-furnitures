#ifndef MODULEREGISTRY_HPP
#define MODULEREGISTRY_HPP

#include <QObject>
#include <QVector>
#include <QHash>

class AbstractModule;
class CoorImpl;
class SerialService;
class QPluginLoader;

class ModuleRegistry : public QObject
{
    Q_OBJECT
private:
    explicit ModuleRegistry(QObject *parent = 0);
    explicit ModuleRegistry(const ModuleRegistry& reg);
    ModuleRegistry& operator=(const ModuleRegistry& reg);
    virtual ~ModuleRegistry();
    void loadLibrary(QString path);
    static ModuleRegistry* pReg;
    QVector<CoorImpl *> coors;
    QVector<QPluginLoader *> libs;
    QHash<quint8, AbstractModule*> hash;

public:
    typedef QVector<CoorImpl *>::Iterator Iterator;
    static ModuleRegistry* getRegistry();
    static void releaseRegistry();
    void loadModules();
    Iterator begin();
    Iterator end();
    AbstractModule * getModule(const QString& moduleID);
    void findModules(const quint8 nodeID, QVector<AbstractModule*>& modules);
    quint8 count();
protected:
    void initModules();
signals:
    
public slots:
    
};

#endif // MODULEREGISTRY_HPP
