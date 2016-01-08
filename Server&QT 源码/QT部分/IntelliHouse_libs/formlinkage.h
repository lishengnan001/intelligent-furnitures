#ifndef FORMLINKAGE_H
#define FORMLINKAGE_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPoint>
#include <QPixmap>
#include <QTimer>
#include <QProcess>

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include "stdio.h"
#include "gprsthread.h"

namespace Ui {
    class FormLinkage;
}

//class TeleService;

class FormLinkage : public QWidget
{
    Q_OBJECT

public:
    explicit FormLinkage(/*TeleService *service, */QWidget *parent = 0);
    ~FormLinkage();

    void setID(QString value);
    void setPrice(QString value);
    void setTrue();

    void setPhotoRsister(bool states);
    void setRfidValue(QString value);
    void setDistance(QString value);
    void setTempValue(QString value);
    void setHumValue(QString value);
    void setFlameStates(bool states);
    void setfingerText(QString value);
    void changeImage(quint8 index);
    void setSomeoneState(bool state);
    class ImageWidget *imageWidget;
    bool pic_write;
    QTimer *timer;
    QWidget *phone;

signals:
    void openLeds();
    void closeLeds();

    void btnLed1Clicked();
    void btnLed2Clicked();
    void btnLed3Clicked();
    void btnLed4Clicked();

    void engineForward();
  //  void engineBackward();
    void engineStop();

    void phoneNumber(const QString &teleNumber);
    //void phoneNumber(char *teleNumber);

    void functionAlarm(int function);

    void closeWindow();
    void LedLight(quint8 cmd);
    void motorOperation(quint8 cmd);

    void tabControlActive(int ms);

    void tabSecurityActive();

    void fingerRecord();
    void  fingerSearch();

    void Recharge(quint32 price);
    void Pay(quint32 price);

private slots:

    void on_backspace_2_clicked();
    void on_printsearch_clicked();
    void on_printRecord_2_clicked();
    void on_labelHum_linkActivated(QString link);
    void on_setTemp_editingFinished();
    void showForm();

    void on_btnBack_clicked();
    void on_btnBack2_clicked();
    void up_date();
   //void on_b_close_clicked();
    void on_b_stop_clicked();
    void on_b_capture_clicked();


    void on_btnChangeTemp_clicked();

    void on_btnSubmitTemp_clicked();


    void setTemp(int temp);
    void setHum(int hum);

    void on_btnLed1_clicked();
    void on_btnLed2_clicked();
    void on_btnLed3_clicked();
    void on_btnLed4_clicked();

    void on_backspace_clicked();
    void on_number_0_clicked();
    void on_number_19_clicked();
    void on_number_18_clicked();
    void on_number_17_clicked();
    void on_number_16_clicked();
    void on_number_15_clicked();
    void on_number_14_clicked();
    void on_number_13_clicked();
    void on_number_12_clicked();
    void on_number_11_clicked();
   // void on_btnBack_clicked();

    void on_enableInput_clicked();
    void on_Recharge_clicked();
    void on_Pay_clicked();

    //void on_btnChangeNum2_clicked();

    //void on_btnSubmitNum2_clicked();

  //  void setLightStates(bool states);

    void switchTabWidget(int index);
    //void updateRFIDUI(bool flag);

public slots:
    //void setDopplerAlarm(bool states);
    //void setSmokeAlarm(bool states);
    //void setDopplerLabel();
   // void setAlarmLabel(bool isSucceed);

   // void setLightStates(bool states);
    //void setSmokeStates(bool states);
   // void setLinkControlLight(bool states);


private:
    Ui::FormLinkage *ui;
    GprsThread *gprsthread;

    //defined for camera
    QImage Imagedata;
    //QPoint point;
    //QPainter draw;
   QPixmap  Pixmap;

    int    len;
    unsigned char *map;
    bool OnOff;
    bool enable_number;

    //QTimer *doppler_timer;
    //QTimer *smoke_timer;


    bool isLed1_on;
    bool isLed2_on;
    bool isLed3_on;
    bool isLed4_on;


    bool isFan_on;
    bool isAboveTemp;
    bool isAboveHum;



    bool isDopplerActive;
    bool isSmokeActive;

    QProcess *madplayerProcess;

    //QStringList dopplerArgs;
    //QStringList smokeArgs;
    //QStringList DopplerSmokeArgs;

    bool isPlaying;
    bool gprsEnable;
    /*void openGprs();
    void init_ttyS(int fd);
    void GSM_Conf_Message(int fd);
    int read_GSM_GPRS_datas(int fd, char *rcv_buf,int rcv_wait);
    int send_GSM_GPRS_cmd(int fd, char *send_buf);
    void GSM_Send_Message(int fd, int function);
    void GSM_GPRS_send_cmd_read_result(int fd, char *send_buf, int rcv_wait);
    void closeGprs();*/

    //int gprs_fd;
   //TeleService *teleService;




    bool virKeyboard1Visible;
    bool virKeyboard2Visible;


};

#endif // FORMLINKAGE_H
