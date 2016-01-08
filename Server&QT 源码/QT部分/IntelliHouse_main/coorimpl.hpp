#ifndef COORIMPL_HPP
#define COORIMPL_HPP

#include <QObject>
#include "abstractmodule.hpp"

class CoorImpl : public QObject, Coordinator
{
    Q_OBJECT
public:
    explicit CoorImpl(QObject *parent = 0);
    void setModule(AbstractModule *pModule);
    AbstractModule * getModule();
    void emitConnected();
    void emitDisconnected();
    void emitActivate();
    void emitDeactivate();
protected:
    AbstractModule *pModule;
signals:
    void connected();
    void disconnected();
    void activate();
    void deactivate();
public slots:
    void onActivate();
};

#endif // COORIMPL_HPP
