#include <cassert>
#include <QtGui>
#include <QWidget>
#include <QCoreApplication>
#include "topologymodule.hpp"
#include "graphwidget.h"
#include "abstractserialservice.hpp"
#include <QDebug>


QByteArray TopologyModule::bytes("\xCC\xEE\x01\x09\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF", 16);

TopologyModule::TopologyModule(QObject *parent) :
        Module(parent)
{
    id = "topology";
}

TopologyModule::TopologyModule(const TopologyModule& module) : Module(module.parent())
{
    assert(false);
}

TopologyModule TopologyModule::operator=(const TopologyModule&)
{
    assert(false);
    return *this;
}

TopologyModule::~TopologyModule()
{
}

void TopologyModule::getImage(QPixmap& pixmap)
{
    QString path = QCoreApplication::applicationDirPath();
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
    path.append("/../images/topology/Topology.png");
#elif defined(Q_WS_QWS)
    path.append("/images/topology/Topology.png");
#endif
    pixmap.load(path);
}

void TopologyModule::getSupportedNodeIDs(QByteArray& ids)
{
    ids.append(0x01);
    ids.append(0x02);
    ids.append(0x03);
    ids.append(0x04);
    ids.append(0x05);
    ids.append(0x06);
    ids.append(0x07);
    ids.append(0x08);
    ids.append(0x09);
    ids.append(0x0A);
    ids.append(0x0B);
    ids.append(0x0C);
    ids.append(0x0D);
    ids.append(0x0E);
    ids.append(0x0F);
    ids.append(0x10);
    ids.append(0x11);
    ids.append(0x12);
    ids.append(0x13);
    ids.append(0x14);
    ids.append(0x15);
    ids.append(0x16);
    ids.append(0x17);
    ids.append(0x18);
    ids.append(0x19);
    ids.append(0x1A);
    ids.append(0x1B);
    ids.append(0x1C);
    ids.append(0x1D);
    ids.append(0x1E);
    ids.append(0x1F);
    ids.append(0x20);
    ids.append(0x21);
    ids.append(0x22);
    ids.append(0x23);
    ids.append(0x24);
    ids.append(0x25);
    ids.append(0x26);
    ids.append(0x27);
    ids.append(0x28);
    ids.append(0x29);
    ids.append(0x2A);
    ids.append(0x2B);
    ids.append(0x2C);
    ids.append(0x2D);
    ids.append(0x2E);
    ids.append(0x2F);
    ids.append(0x32);
    ids.append(0x33);

}

void TopologyModule::createWindow(QWidget *pParentWindow, const QRect *pRect)
{
    if (0 == form) {
        Module::createWindow(pParentWindow, pRect);
        form = new GraphWidget(pParentWindow);
        if (0 != pRect) {
            form->setGeometry(*pRect);
        }
        GraphWidget *graphMessage = dynamic_cast<GraphWidget*>(form);
        graphMessage->buildCoor(Qt::red, Qt::darkRed, "Coor", 0, 0);

        connect(form, SIGNAL(closeWindow()), this, SLOT(onDeactivate()));
    }
}

bool TopologyModule::negotiatePlatform(QString platform)
{
    return !platform.compare("2530");
}

void TopologyModule::onDeactivate()
{
    moduleDeactivate();
}

void TopologyModule::onLedLight(quint8 cmd)
{
    if (1 > cmd || 10 < cmd) {
        return ;
    }
    switch(cmd) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        bytes[4] = cmd;
        break;
    case 9:
        bytes[4] = 0x0c;
        break;
    case 10:
        bytes[4] = 0x0d;
        break;
    default:
        break;
    }

    AbstractSerialService *pService = getSerialService();
    pService->write(bytes);
}

bool TopologyModule::onMsg(const QByteArray& msg)
{
    char *nodeName;
    Module::onMsg(msg);  // Ensure what parent task to do
    if (msg[4] == 0xaa){
        int sAddr,pAddr;
        sAddr = (msg[7] << 8) + msg[8];
        pAddr = (msg[9] << 8) + msg[10];
        nodeName = getName(int(msg[3]));
        GraphWidget *graphMessage = dynamic_cast<GraphWidget*>(form);
        if (msg[6] == 0x01) {
            graphMessage->buildNodeLink(Qt::yellow, Qt::darkYellow, nodeName, sAddr, pAddr);
        }
        else{
            graphMessage->buildNodeLink(Qt::blue, Qt::darkBlue, nodeName,sAddr, pAddr);
        }
    }

    //qDebug() <<"**********************msg is:"<<QByteArray(msg).toHex()<<endl;
    return true;
}

QString TopologyModule::transQBtoQS(const QByteArray &data, int size)
{
    QString str;
    for (int i = 0; i < size; i ++){
        str.append(QString(data[i]));
    }
    return(str);
}
char *TopologyModule::getName(int name)
{
    switch (name){
        case 0:
            return ("Coor");
            break;
        case 1:
            return ("Hall");
            break;
        case 2:
            return ("PRes");
            break;
        case 3:
            return ("TemHum");
            break;
        case 4:
            return ("Shake");
            break;
        case 5:
            return ("Reed");
            break;
        case 6:
            return ("Accele");
            break;
        case 7:
            return ("Smoke");
            break;
        case 8:
            return ("InfRed");
            break;
        case 9:
            return ("Motor");
            break;
        case 10:
            return ("LedPwm");
            break;
        case 11:
            return ("Sound");
            break;
        case 12:
            return ("Voltage");
            break;
        case 13:
            return ("Current");
            break;
        case 14:
            return ("Touch");
            break;
        case 15:
            return ("SupWave");
            break;
        case 16:
            return ("RFPAY");
            break;
        case 17:
            return ("LedPwm");
            break;
        case 18:
            return ("Flame");
            break;
        case 22:
            return ("IRCode");
            break;
        case 24:
            return ("Relay");
            break;
        case 31:
            return ("LScreem");
            break;
        case 32:
            return ("Alarm");
            break;
        case 34:
            return ("Curtain");
            break;
        case 35:
            return ("SLamp1");
            break;
        case 38:
            return ("Fan");
            break;
        case 40:
            return ("LH1");
            break;
        case 41:
            return ("LH2");
            break;
        case 42:
            return ("LH3");
            break;
        case 43:
            return ("LH4");
            break;
        case 44:
            return ("SLamp2");
            break;
        case 45:
            return ("LPWM1");
            break;
        case 46:
            return ("LPWM2");
            break;
        case 47:
            return ("LPWM3");
            break;
        case 48:
            return ("LPWM4");
            break;
        case 50:
            return ("RFID");
            break;
        case 51:
            return ("FPrint");
            break;
        default :
                break;
    }
}
Q_EXPORT_PLUGIN2(Leds, TopologyModule);
