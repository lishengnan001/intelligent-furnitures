#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>
#include <QTimeLine>
#include <QStringList>
#include "posix_qextserialport.h"

class QMouseEvent;
class QTimer;
class QProcess;
class TeleService;
class Posix_QextSerialPort;
class FormLeds;
class FormEngine;
class FormDoppler;
class FormGan;
class FormHall;
class FormLight;
class FormShock;
class FormSmoke;
class FormTemp;
class FormLed;
class FormAdd;
class FormLinkage;
class FormGPS;
class FormRFID;
class FormCurrent;
class FormFinger;
class FormSound;
class FormTouch;
class FormVoltage;
class FormGps;
class Panel;

namespace Ui {
    class Container;
}

class Container : public QWidget
{
    Q_OBJECT

public:
    explicit Container(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Container();

private:
    int pageNum;
    int curPage;
    static const int pageWidth = 480;
    static const int frameDelta = 5;
    QTimeLine timeLine;
    bool animationInProgress;
    bool animationDirection;
    QPoint slidePos;
    Posix_QextSerialPort *myCom;
    QTimer *timer;
    unsigned int timerdly;

    QTimer *tempTimer;

    bool isOpen;

    void startInit();
    void openCom(BaudRateType baudRate);
    void closeCom();

    bool rfidStarted;
    bool linkSecurityActive;
    bool linkControlActive;
    bool linkTouchActive;
    bool linkSoundActive;
    bool linkRFIDActive;
    bool linkSmokeActive;
    QString cardId;
    int linkSoundCounter;
    int linkDopplerBroken;

    TeleService *teleService;

    QProcess *madplayerProcess;

    QStringList welcomeArgs;


private:
    Ui::Container *ui;
    FormLeds* ledsForm;
    FormEngine* engineForm;
    FormDoppler* dopplerForm;
    FormGan* ganForm;
    FormHall* hallForm;
    FormLight* lightForm;
    FormShock* shockForm;
    FormSmoke* smokeForm;
    FormTemp* tempForm;
    FormLed* ledForm;
    FormAdd* addForm;
    FormLinkage* linkForm;
    FormGPS* gpsForm;
    FormRFID* rfidForm;
    FormCurrent* currentForm;
    FormFinger* fingerForm;
    FormSound* soundForm;
    FormTouch* touchForm;
    FormVoltage* voltageForm;
    FormGps* GPSForm;
    Panel* panel;
signals:
    void isDoppler(bool states);
    void isGan(bool states);
    void isHall(bool states);
    void isLight(bool states);
    void isLinkLight(bool states);
    void isLinkControlLight(bool states);
    void isLinkSmoke(bool states);
    void isLinkSecurity(bool states);
    void isShock(bool states);
    void isSmoke(bool states);
    void isCurrent(bool states);
    void isSound(bool states);
    void isTouch(bool states);
    void isVoltage(bool states);
    void isFinger(bool states);
    //void isLinkDopplerBroken(bool states);

    void setTemp(int temp);
    void setHum(int hum);
    void setTempHum(int temp,int hum);

    void setX(int x);
    void setY(int y);
    void setZ(int z);
    void setXYZ(int x,int y,int z);

    void updateRFIDUI(QString cardID);
    void updateLinkRFIDUI(bool flag);

public slots:
    void showGPSForm();
private slots:
    void shiftPage(int frame);
    void readMyCom();

    void led1_On_writeCom();
    void led1_Off_writeCom();

    void led2_On_writeCom();
    void led2_Off_writeCom();

    void led3_On_writeCom();
    void led3_Off_writeCom();

    void led4_On_writeCom();
    void led4_Off_writeCom();

    void all_Led_on_writeCom();
    void all_Led_off_writeCom();

    void engine_Forward_writeCom();
    void engine_Backward_writeCom();
    void engine_Stop_writeCom();

    void led_writeCom(int value);

    void temp_writeCom();
    void hum_writeCom();
    void temphum_writeCom();

    void x_writeCom();
    void y_writeCom();
    void z_writeCom();
    void xyz_writeCom();

    void linkCloseNeedDo();

    void rfidStart();
    void rfidStop();

    void gpsquit();

    void tabSecurityActive();
    void tabTouchActive();
    void tabSoundActive();
    void tabControlActive();
    void tabRFIDActive();
    void tabSmokeActive();
    void tabRFIDDeactive();

protected:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    void shiftNext();
    void shiftPrevious();
};

#endif // CONTAINER_H
