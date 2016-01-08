#ifndef ABSTRACTMODULE_HPP
#define ABSTRACTMODULE_HPP


class QPixmap;
class QRect;
class AbstractSerialService;
class AbstractTeleService;
class AbstractSoundService;

class AbstractModule;

class Coordinator {
public:
    virtual ~Coordinator() {}
    virtual void setModule(AbstractModule *pModule) = 0;
    virtual void emitConnected() = 0;
    virtual void emitDisconnected() = 0;
    virtual void emitActivate() = 0;
    virtual void emitDeactivate() = 0;
};

class AbstractModule
{
public:
    virtual ~AbstractModule() {}
    virtual void getImage(QPixmap& pixmap) = 0;
    virtual void onTimer() = 0;
    virtual bool onMsg(const QByteArray& msg) = 0;
    virtual void getSupportedNodeIDs(QByteArray& ids) = 0;
    virtual void createWindow(QWidget *pParentWindow = 0, const QRect *pRect = 0) = 0;
    virtual bool negotiatePlatform(QString platform) = 0;
    virtual void setTimerInterval(quint16 interval) = 0;
    virtual void setSerialService(AbstractSerialService *pService) = 0;
    virtual void setTeleService(AbstractTeleService *pService) = 0;
    virtual void setSoundService(AbstractSoundService *pService) = 0;
    virtual QString getID() = 0;
    virtual void setCoordinator(Coordinator *pCoor) = 0;
    virtual Coordinator* getCoordinator() = 0;
    virtual void moduleActivate() = 0;
};

Q_DECLARE_INTERFACE(AbstractModule,"com.chinasofti.etc.abstractmodule/1.0")

#endif // ABSTRACTMODULE_HPP
