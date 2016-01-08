#include "coorimpl.hpp"

CoorImpl::CoorImpl(QObject *parent) :
    QObject(parent),
    pModule(0)
{
}

void CoorImpl::emitConnected() {
    emit connected();
}

void CoorImpl::emitDisconnected() {
    emit disconnected();
}

void CoorImpl::emitActivate() {
    emit activate();
}

void CoorImpl::emitDeactivate() {
    emit deactivate();
}

AbstractModule * CoorImpl::getModule() {
    return pModule;
}

void CoorImpl::setModule(AbstractModule *pModule) {
    if (0 != this->pModule) {
        this->pModule->setCoordinator(0);
    }
    this->pModule = pModule;
    this->pModule->setCoordinator(this);
}

void CoorImpl::onActivate() {
    if (0 != pModule) {
        pModule->moduleActivate();
    }
}
