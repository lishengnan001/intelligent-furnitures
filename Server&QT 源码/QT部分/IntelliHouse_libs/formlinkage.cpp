#include "formlinkage.h"
#include "ui_formlinkage.h"
#include "imagewidget.h"
#include "camerainit.h"
#include "calculator.h"

#include <QTime>
#include <QMessageBox>
//#include <QSound>

#include <fcntl.h>	// open() close()
#include <unistd.h>	// read() write()

#include <termios.h>	// set baud rate

#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#define DEBUG

int flag = 1;
#ifdef DEBUG
#include <QDebug>
#endif

//#include "teleservice.h"

#define DEVICE_TTYS "/dev/ttySAC1"

FormLinkage::FormLinkage(/*TeleService *service, */QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLinkage)//,
    //teleService(service)
{
    this->map = (unsigned char*)malloc(640*480*20);
    ui->setupUi(this);
    ui->keyboard->hide();
    ui->enableInput->setEnabled(false);
    ui->Pay->setEnabled(false);
    ui->Recharge->setEnabled(false);
    gprsthread = new GprsThread;
    gprsthread->start();
    gprsthread->clearRun();
#if defined(_WS_QWS) || defined(Q_WS_QWS)
  showFullScreen();
#else
  resize(800, 480);
#endif

  QPalette palette;        //以下三行为程序设置背景图片
  palette.setBrush(QPalette::Background,QBrush(QPixmap(":/image/background.png")));
  setPalette(palette);

  OnOff = false;
  enable_number = false;
  gprsEnable = false;

  isDopplerActive = false;
  isSmokeActive = false;


  isFan_on = false;
  isAboveTemp = false;
  isAboveHum = false;

  virKeyboard1Visible = false;
  virKeyboard2Visible = false;


  madplayerProcess = new QProcess(this);
  isPlaying = false;


  this->imageWidget = new ImageWidget(this);
  this->imageWidget->setBackgroundRole(QPalette::Dark);
  this->imageWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  this->imageWidget->setObjectName(QString::fromUtf8("imageWidget"));
  this->imageWidget->setGeometry(QRect(45, 65, 540, 385));

  timer = new QTimer;
  connect(this->timer, SIGNAL(timeout()), this, SLOT(up_date()));

  deviceOpen();
  deviceInit();

  pic_write = false;

  connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(switchTabWidget(int)));


  hide();
}

FormLinkage::~FormLinkage()
{
    free(this->map);
    this->map = 0;
    delete ui;
}

void FormLinkage::showForm()
{

    ui->tabWidget->setCurrentWidget(ui->tabSecurity);
    emit tabSecurityActive();

    emit engineStop();

    ui->labelflame->setPixmap(QPixmap(":/image/icon/noSmoke.png"));
    isFan_on = false;
    isAboveTemp = false;
    isAboveHum = false;

    //setLightStates(false);
    //setLinkControlLight(true);
    show();

    //openGprs();

}


void FormLinkage::on_btnBack_clicked()
{
    //setLightStates(false);
    madplayerProcess->close();
    isPlaying = false;
    this->hide();

    emit closeWindow();
    OnOff = false;
    timer->stop();
    ui->b_stop->setIcon(QPixmap(":/image/icon/videoOff.png"));
    //closeGprs();
}

void FormLinkage::on_btnBack2_clicked() {
    emit closeWindow();
    OnOff = false;
    timer->stop();
    ui->b_stop->setIcon(QPixmap(":/image/icon/videoOff.png"));
}


void FormLinkage::setTemp(int temp)
{
    //temperature = temp;
    ui->labelTemp->setText(QString::number(temp));
    //int tempValue = ui->setTemp->text().toInt();
#ifdef DEBUG
//    qDebug()<<"int tempValue ="<<tempValue<<endl;
    qDebug()<<"............the temp value is : "<<temp<<endl;
#endif

}



void FormLinkage::on_btnLed1_clicked()
{

    if (!isLed1_on) {
        emit LedLight(1);

        qDebug()<<"send LedLight1.............."<<endl;
    } else {
        emit LedLight(2);
        qDebug()<<"send LedLight2.............."<<endl;
    }
}

void FormLinkage::on_btnLed2_clicked()
{

    if (!isLed2_on) {
        emit LedLight(3);
    } else {
        emit LedLight(4);
    }
}

void FormLinkage::on_btnLed3_clicked()
{

    if (!isLed3_on) {
        emit LedLight(5);
    } else {
        emit LedLight(6);
    }
}

void FormLinkage::switchTabWidget(int index)
{
    if (virKeyboard1Visible) {
        //on_btnSubmitNum_clicked();
    }
    else if (virKeyboard2Visible) {
        //on_btnSubmitNum2_clicked();
    }
    //setLightStates(false);
    if(index == ui->tabWidget->indexOf(ui->tabControl))
    {
        emit tabControlActive(1000);
#ifdef DEBUG
        qDebug()<<"emit tabControlActive(1000)"<<endl;
#endif

    }
    else if (index == ui->tabWidget->indexOf(ui->tabSecurity))
    {
        emit tabSecurityActive();
#ifdef DEBUG
        qDebug()<<"emit tabSecurityActive()"<<endl;
#endif


    }
}

void FormLinkage::up_date()
{
    unsigned char image_buf[1536000+54];
    frameRead(image_buf);
    this->Imagedata = QImage::fromData(image_buf,800*480*4+54,NULL);
    Pixmap = QPixmap::fromImage (this->Imagedata, 0);
    ui->labelVideo->setPixmap(this->Pixmap);
     qDebug()<<"start get vedio............................."<<endl;
    if (pic_write)
    {
        FILE* bmp_f = fopen("a.bmp", "w+");
        fwrite(image_buf, 1, 800*480*4+54, bmp_f);
        fclose(bmp_f);
        pic_write = false;
         qDebug()<<"take photoshop............................."<<endl;
    }

}



/*void FormLinkage::on_b_start_clicked()
{
    timer->start(100);
}*/

void FormLinkage::on_b_capture_clicked()
{
    this->pic_write = true;
}

void FormLinkage::on_b_stop_clicked()
{

    if(OnOff){
        OnOff = false;
        timer->stop();
        ui->b_stop->setIcon(QPixmap(":/image/icon/videoOff.png"));
    }
    else{
        OnOff = true;
        captureStart();
        timer->start(300);
        ui->b_stop->setIcon(QPixmap(":/image/icon/videoOn.png"));
    }

}


void FormLinkage::setRfidValue(QString value) {
    ui->RFID->setText(value);
}

void FormLinkage::setID(QString value) {
    ui->label_ID->setText(value);
    qDebug()<<"setRFID...................."<<endl;
}

void FormLinkage::setTrue() {
    ui->enableInput->setEnabled(true);
    ui->Pay->setEnabled(true);
}

void FormLinkage::setPrice(QString value) {
    ui->labelPrice->setText(value);
}

void FormLinkage::on_enableInput_clicked()
{
    //virtualKeyBoard->show();
    flag = 2;
    ui->keyboard->show();
    ui->Recharge->setEnabled(true);
    ui->Pay->setEnabled(false);
    ui->enableInput->setEnabled(false);
    ui->inputPrice->setFocus();
    ui->keyboard->show();



}

void FormLinkage::on_Recharge_clicked()
{

    ui->keyboard->hide();
    ui->enableInput->setEnabled(true);
    ui->Pay->setEnabled(true);



    QString str = "FF";
    bool ok;
    int hex = str.toInt(&ok, 16);  // hex == 255, ok == true
    int dec = str.toInt(&ok, 10);  // dec == 0, ok == false

    quint32 price = ui->inputPrice->text().toInt(&ok, 10);
     qDebug() << "price = " << price << endl;
     qDebug() << "...hh...orprice = " << ui->labelPrice->text() << endl;
     qDebug() << ".....jj...orprice.... = " << ui->inputPrice->text().toInt(&ok,10) << endl;
    #ifdef DEBUG
        qDebug() << "price = " << price << endl;
    #endif
    qDebug() << "ddddddddddddddddddd"<< endl;

    ui->keyboard->hide();
    if ((price > 0) && (price < 65535))
    {qDebug() << "flag = ........." << flag << endl;
        if(flag == 2)
        {

            emit Recharge(price);
            qDebug() << "+++++++++++++++++++++++++ "<< endl;
        }
       if(flag == 3)
        {
           qDebug() << "-------------------------"<< endl;
           emit Pay(price);
        }
    }
    ui->inputPrice->clear();
    ui->Recharge->setEnabled(false);

}

void FormLinkage::on_Pay_clicked()
{
//    ui->inputPrice->clear();
    flag = 3;
    ui->keyboard->show();
    ui->Recharge->setEnabled(true);
    ui->Pay->setEnabled(false);
    ui->enableInput->setEnabled(false);
    ui->inputPrice->setFocus();
    ui->keyboard->show();
}

void FormLinkage::on_number_11_clicked()
{
    int money = ui->number_11->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_12_clicked()
{
    int money = ui->number_12->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_13_clicked()
{
    int money = ui->number_13->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_14_clicked()
{
    int money = ui->number_14->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_15_clicked()
{
    int money = ui->number_15->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_16_clicked()
{
    int money = ui->number_16->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_17_clicked()
{
    int money = ui->number_17->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_18_clicked()
{
    int money = ui->number_18->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_19_clicked()
{
    int money = ui->number_19->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}

void FormLinkage::on_number_0_clicked()
{
    int money = ui->number_0->text().toInt();
    ui->inputPrice->setText(ui->inputPrice->text() + QString::number(money));
}




/***********************************************************/
void FormLinkage::setDistance(QString value) {
    ui->label->setText(value);
}

void FormLinkage::setTempValue(QString value) {
    ui->labelTemp->setText(value);
}

void FormLinkage::setHumValue(QString value) {
    ui->labelHum->setText(value);
}

void FormLinkage::setFlameStates(bool states)
{
    if(states == true)
    {
        ui->labelflame->setPixmap(QPixmap(":/image/icon/hasSmoke.png"));
    }
    else
    {
        ui->labelflame->setPixmap(QPixmap(":/image/icon/noSmoke.png"));
    }
}

void FormLinkage::setPhotoRsister(bool states)
{
    //ui->label_4->setText(value);
  //  if (ui->label_4->setText(value) > 1000)
   // if (value > 1000)
    if (states == true)
        ui->labelLight->setPixmap(QPixmap(":/image/icon/liang.png"));
    else
        ui->labelLight->setPixmap(QPixmap(":/image/icon/an_yuan.png"));
}


void FormLinkage::on_printRecord_2_clicked()
{
    emit fingerRecord();
}

void FormLinkage::on_printsearch_clicked()
{
    ui->label_f->clear();
    emit fingerSearch();
}
void FormLinkage::setfingerText(QString value)
{
    //ui->label_id->setText(tr("match successful"));
   ui->label_f->setText(tr("success!"));
}



void FormLinkage::on_backspace_2_clicked()
{
     ui->inputPrice->backspace();
}

void FormLinkage::setSomeoneState(bool state){
    if (state){
         ui->someone->setText("someone break in!");
     }else{
        ui->someone->setText("Safety");
     }
}

