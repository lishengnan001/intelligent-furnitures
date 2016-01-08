#include <QtGlobal>
#include <iostream>
#include <QDebug>
#include <QByteArray>
#include <cassert>


#include "netservice.hpp"
#include "settings.hpp"
#include "serialservice.hpp"
//#include "abstractserialservice.hpp"

QByteArray NetService::ledBytes("\xCC\xEE\x01\x09\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray NetService::pwmBytes("\xCC\xEE\x01\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray NetService::tvBytes("\xCC\xEE\x01\x16\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray NetService::airBytes("\xCC\xEE\x01\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);
QByteArray NetService::ledScreemBytes("\xCC\xEE\x01\x1f\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00x00\x00\x00\x00\x00\x00\x00\x00\x00x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff", 38);
using namespace std;

NetService *NetService::pService(0);

NetService::NetService(QObject* parent): QObject(parent)
{
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    tcpServer.listen(QHostAddress::Any, 8888);
    qDebug() << " Network Service start ... " << endl;
    qDebug() << " Socket server create and listen ... " << endl;
}

NetService::~NetService()
{
    tcpServer.close();
}

void NetService::acceptConnection()
{
    tcpSocket = tcpServer.nextPendingConnection();
    socketConnected = true;
    qDebug()<<"I am glad to server for you~~~~~~~~~~~~~~~!!!!!!!!!!!!!!!!";
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(startRead()));
}

void NetService::startRead()
{
    int i;
    ISerialService *pSerialService = SerialService::getService();
    tcpSocket->read(buffer, tcpSocket->bytesAvailable());
    qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~"<<QByteArray(buffer).toHex()<<"size is : "<<QByteArray(buffer).size();
    if (buffer[3] == 0x1f){
        ledScreemBytes = QByteArray::fromRawData(buffer, 38);
        for (i = 0; i < 50; i ++){
            buffer[i] = 0;
        }
        pSerialService->write(ledScreemBytes);
        qDebug()<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<QByteArray(ledScreemBytes).toHex()<<"size is : "<<QByteArray(ledScreemBytes).size();
    }
    else{
        //ledBytes = QByteArray::fromRawData(buffer, 16);
        for(int i=0; i<16; i++){
            ledBytes[i] = buffer[i];
        }
        for (int i = 0; i < 50; i ++){
            buffer[i] = 0;
        }
        pSerialService->write(ledBytes);
        qDebug()<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<QByteArray(ledBytes).toHex()<<"size is : "<<QByteArray(ledBytes).size();
        //ISerialService *pSerialService = SerialService::getService();

    }


    //serviceDispatch(buffer);
}

void NetService::startWrite(const QByteArray& data)
{
    //qDebug() << "Socket:" << data.toHex() << endl;
    tcpSocket->write(data.toHex(), ZIGBEE_INFO_LENGTH);
}
/**/
void NetService::serviceDispatch(QByteArray ba)
{
    struct global_socket *glb = & glb_ins;
    char *pBuffer = ba.data();
    glb->input_bf_ptr = pBuffer;
    glb->cmd = (struct socket_cmd *) glb->input_bf_ptr;
    qDebug()<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<ba.toHex()<<"size is : "<<ba.size();
    ISerialService *pSerialService = SerialService::getService();
    //ledBytes[3] = pBuffer[3];
    //ledBytes[4] = pBuffer[4];
    //pSerialService->write(pBuffer);
    switch ( glb->cmd->cmd_id ) {

    case CMD_ID_SET_LED_1_ON:
    {
        qDebug() << "set_led_1_on()" << endl;
        ledBytes[4] = 1;
        pSerialService->write(ledBytes);
    }
        break;

    case CMD_ID_SET_LED_1_OFF:
        qDebug() << "set_1_led_1_off()" << endl;
        ledBytes[4] = 2;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_2_ON:
        qDebug() << "set_1_led_2_on()" << endl;
        ledBytes[4] = 3;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_2_OFF:
        qDebug() << "set_1_led_2_off()" << endl;
        ledBytes[4] = 4;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_3_ON:
        qDebug() << "set_1_led_3_on()" << endl;
        ledBytes[4] = 5;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_3_OFF:
        qDebug() << "set_1_led_3_off()" << endl;
        ledBytes[4] = 6;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_4_ON:
        qDebug() << "set_1_led_4_on()" << endl;
        ledBytes[4] = 7;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_4_OFF:
        qDebug() << "set_1_led_4_off()" << endl;
        ledBytes[4] = 8;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_ALL_ON:
        qDebug() << "set_1_led_all_on()" << endl;
        ledBytes[4] = 0x0c;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_LED_ALL_OFF:
        qDebug() << "set_1_led_all_off()" << endl;
        ledBytes[4] = 0x0d;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_MOTOR_LEFT:
        qDebug() << "set_1_motor_left()" << endl;
        ledBytes[4] = 0x09;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_MOTOR_RIGHT:
        qDebug() << "set_1_motor_right()" << endl;
        ledBytes[4] = 0x0a;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_MOTOR_STOP:
        qDebug() << "set_1_motor_stop()" << endl;
        ledBytes[4] = 0x0b;
        pSerialService->write(ledBytes);
        break;

    case CMD_ID_SET_PWD_00:
        qDebug() << "set_pwm_00()" << endl;
        pwmBytes[4] = 0x00;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_01:
        qDebug() << "set_pwm_01()" << endl;
        pwmBytes[4] = 0x01;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_02:
        qDebug() << "set_pwm_02()" << endl;
        pwmBytes[4] = 0x02;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_03:
        qDebug() << "set_pwm_03()" << endl;
        pwmBytes[4] = 0x03;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_04:
        qDebug() << "set_pwm_04()" << endl;
        pwmBytes[4] = 0x04;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_05:
        qDebug() << "set_pwm_05()" << endl;
        pwmBytes[4] = 0x05;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_06:
        qDebug() << "set_pwm_06()" << endl;
        pwmBytes[4] = 0x06;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_07:
        qDebug() << "set_pwm_07()" << endl;
        pwmBytes[4] = 0x07;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_08:
        qDebug() << "set_pwm_08()" << endl;
        pwmBytes[4] = 0x08;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_PWD_09:
        qDebug() << "set_pwm_09()" << endl;
        pwmBytes[4] = 0x09;
        pSerialService->write(pwmBytes);
        break;

    case CMD_ID_SET_VOLUME_DECREASE:
        qDebug() << "set_volume_decrease" << endl;
        tvBytes[5] = 0x02;
        pSerialService->write(tvBytes);
        break;

    case CMD_ID_SET_VOLUME_INCREASE:
        qDebug() << "set_volume_increase" << endl;
        tvBytes[5] = 0x03;
        pSerialService->write(tvBytes);
        break;

    case CMD_ID_SET_CHANNEL_FORWARD:
        qDebug() << "set_channel_forward" << endl;
        tvBytes[5] = 0x01;
        pSerialService->write(tvBytes);
        break;

    case CMD_ID_SET_CHANNEL_BACKWARD:
        qDebug() << "set_channel_backward" << endl;
        tvBytes[5] = 0x04;
        pSerialService->write(tvBytes);
        break;

    default:
        break;
    }

}

NetService* NetService::getService() {
    if (!pService) {
        pService = new NetService();
    }
    return pService;
}

void NetService::releaseService() {
    if (pService) {
        delete pService;
        pService = 0;
    }
}
