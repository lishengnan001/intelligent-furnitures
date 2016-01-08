#include <QWidget>
#include "abstractserialservice.hpp"
#include "abstractteleservice.hpp"
#include "abstractsoundservice.hpp"
#include "module.hpp"

Module::Module(QObject *parent) :
    QObject(parent),
    pSerialService(0),
    pTeleService(0),
    pSoundService(0),
    timerInterval(0),
    id((const char*)0),
    form(0),
    isActive(false),
    parentWindowAvaible(false),
    tick(TIME_THRESHOLD),
    pCoor(0)
{
}

Module::~Module() {
    if (!parentWindowAvaible && 0 != form) {
        delete form;
    }
}

void Module::setTimerInterval(quint16 interval) {
    timerInterval = interval;
}

QString Module::getID() {
    return id;
}

void Module::setSerialService(AbstractSerialService *pService) {
    pSerialService = pService;
}

void Module::setTeleService(AbstractTeleService *pService) {
    pTeleService = pService;
}

void Module::setSoundService(AbstractSoundService *pService) {
    pSoundService = pService;
}

AbstractSerialService* Module::getSerialService() {
    return pSerialService;
}

AbstractTeleService* Module::getTeleService() {
    return pTeleService;
}

AbstractSoundService* Module::getSoundService() {
    return pSoundService;
}

void Module::moduleActivate() {
    if (!isActive) {
        isActive = true;
        createWindow();
        form->show();
    }
}

void Module::moduleDeactivate() {
    if (isActive) {
        if (0 != form) {
            form->hide();
        }
        isActive = false;
    }
}

void Module::onTimer() {
    if (tick >= TIME_THRESHOLD) {
        if (0 != pCoor) {
            pCoor->emitDisconnected();
            pCoor->emitDeactivate();
            moduleDeactivate();
        }
    }
    else {
        tick += timerInterval;
    }
}

bool Module::negotiatePlatform(QString ) {
    return false;
}

bool Module::onMsg(const QByteArray& ) {
    tick = 0;
    if (0 != pCoor) {
        pCoor->emitConnected();
    }
    return true;
}

void Module::createWindow(QWidget *pParentWindow, const QRect *) {
    if (0 != pParentWindow) {
        parentWindowAvaible = true;
    }
}

void Module::setCoordinator(Coordinator *pCoor) {
    this->pCoor = pCoor;
}

Coordinator* Module::getCoordinator() {
    return pCoor;
}


