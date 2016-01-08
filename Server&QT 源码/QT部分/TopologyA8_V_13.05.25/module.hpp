#ifndef MODULE_HPP
#define MODULE_HPP

#include <QObject>
#include <QPixmap>
#include "abstractmodule.hpp"

class ISerialService;
class TeleService;
class SoundService;
class QWidget;

class Module : public QObject, public AbstractModule
{
    Q_OBJECT
    Q_INTERFACES(AbstractModule)
public:
    explicit Module(QObject *parent = 0);
    virtual ~Module();
    virtual void getImage(QPixmap& pixmap) = 0;
    virtual void onTimer();
    virtual bool onMsg(const QByteArray& msg);
    virtual void getSupportedNodeIDs(QByteArray& ids) = 0;
    virtual void createWindow(QWidget *pParentWindow = 0, const QRect *pRect = 0);
    virtual bool negotiatePlatform(QString platform);
    void setTimerInterval(quint16 interval);
    void setSerialService(AbstractSerialService *pService);
    void setTeleService(AbstractTeleService *pService);
    void setSoundService(AbstractSoundService *pService);
    QString getID();
    virtual void setCoordinator(Coordinator *pCoor);
    virtual Coordinator* getCoordinator();
    virtual void moduleActivate();
private:
    AbstractSerialService *pSerialService;
    AbstractTeleService *pTeleService;
    AbstractSoundService *pSoundService;
protected:
    int timerInterval;
    QString id;
    QWidget *form;
    bool isActive;
    bool parentWindowAvaible;
    const static quint16 TIME_THRESHOLD = 5000;
    quint16 tick;
    Coordinator *pCoor;
    AbstractSerialService* getSerialService();
    AbstractTeleService* getTeleService();
    AbstractSoundService* getSoundService();
    virtual void moduleDeactivate();
};

#endif // MODULE_HPP
