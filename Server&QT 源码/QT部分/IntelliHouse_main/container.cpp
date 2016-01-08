#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include "container.hpp"
#include "ui_container.h"
#include "panel.hpp"
#include "serialservice.hpp"
#include "soundservice.hpp"
#include "abstractmodule.hpp"
#include "moduleregistry.hpp"
#include "coorimpl.hpp"
#include <QProcess>
#define DEBUG

Container::Container(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    ui(new Ui::Container),
    curPage(1),
    timeLine(1000),
    animationInProgress(false),
    animationDirection(false),
    pTimer(new QTimer(this)),
    aliveMovie(),
    healthValue(5000)
{
    ui->setupUi(this);
    pPanel = new Panel(ui->location);
#if defined(_WS_QWS) || defined(Q_WS_QWS)
	aliveMovie.setFileName(QCoreApplication::applicationDirPath() + "/animation/alive.gif");
#elif defined(Q_WS_WIN) || defined(Q_WS_X11)
	aliveMovie.setFileName(QCoreApplication::applicationDirPath() + "/../animation/alive.gif");
#endif
    ui->labelAlive->setMovie(&aliveMovie);
    aliveMovie.start();
    timeLine.setFrameRange(0, pageWidth / frameDelta);
    connect(&timeLine, SIGNAL(frameChanged(int)), this, SLOT(shiftPage(int)));
    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(QPixmap(":/image/backGround2.PNG")));
    setPalette(palette);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    ModuleRegistry::Iterator iter;
    ModuleRegistry *pReg = ModuleRegistry::getRegistry();
    for(iter = pReg->begin(); iter != pReg->end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        pModule->setTimerInterval(TIMER_INTERVAL);
    }
    pageNum = pReg->count() / 8 + 1;
    connect(this, SIGNAL(healthChanged(bool)), this, SLOT(setHealth(bool)));
    ISerialService *pSerialService = SerialService::getService();
    pSerialService->open();
    pTimer->start(TIMER_INTERVAL);
    serverState = false;
    isConnect = false;
}

Container::~Container()
{
    SoundService* pSoundService(SoundService::getService());
    pSoundService->stop();
    delete ui;
}

void Container::shiftPrevious() {
    if ((curPage > 1) && (curPage <= pageNum) && !animationInProgress) {
        animationInProgress = true;
        animationDirection = false;
        timeLine.start();
    }
}

void Container::shiftNext() {
    if ((curPage < pageNum) && (curPage >= 1) && !animationInProgress) {
        animationInProgress = true;
        animationDirection = true;
        timeLine.start();
    }
}

void Container::shiftPage(int frame) {
    int width = pageWidth * pageNum;
    int height = pPanel->height();
    int top = 0;
    int left = 0 - (curPage - 1) * pageWidth;
    if (animationDirection) {
        left -= frame * frameDelta;
    }
    else {
        left += frame * frameDelta;
    }
    pPanel->setGeometry(left, top, width, height);
    if ((pageWidth / frameDelta) == frame) {
        animationInProgress = false;
        animationDirection ? curPage++ : curPage--;
    }
}

void Container::mousePressEvent(QMouseEvent * event) {
    slidePos = event->pos();
}

void Container::mouseReleaseEvent(QMouseEvent * event) {
    int startx = slidePos.x();
    int endx = event->pos().x();
    if (endx < (startx - 10)) {
        shiftNext();
    }
    else if (endx > (startx + 10)) {
        shiftPrevious();
    }
}

void Container::onTimer() {
    bool healthState = false;
    ModuleRegistry *pReg = ModuleRegistry::getRegistry();
    ISerialService *pSerialService = SerialService::getService();
    if (!pSerialService->isLocked()) {
        QByteArray bytes;
        qint64 len = pSerialService->read(bytes);
        QByteArray header;
        header.append((char)0xee);
        header.append((char)0xcc);
        const static quint8 msglen = 16;
        int pos = 0;
        pos = bytes.indexOf(header, pos);
        while (0 <= pos && (pos + msglen) <= len) {
            if (!healthState) {
                emit healthChanged(true);
                healthState = true;
            }
            if ((char)0xff == bytes[pos + msglen - 1]) {
                QByteArray msg = bytes.mid(pos, msglen);
                QVector<AbstractModule*> modules;
                pReg->findModules(msg[3], modules);
                QVector<AbstractModule*>::Iterator iter;
                for (iter = modules.begin(); iter != modules.end(); iter++) {
                    AbstractModule * pModule = *iter;
                    pModule->onMsg(msg);
                }
            }
            pos = bytes.indexOf(header, pos + 1);
        }
    }
    if (!healthState) {
        if (TIME_THRESHOLD <= healthValue) {
            emit healthChanged(false);
        }
        else {
            healthValue += TIMER_INTERVAL;
        }
        healthState = true;
    }
    ModuleRegistry::Iterator iter;
    for(iter = pReg->begin(); iter != pReg->end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        pModule->onTimer();
    }
}

void Container::setHealth(bool health) {
    if (health) {
        healthValue = 0;
        if (QMovie::Running != aliveMovie.state()) {
            aliveMovie.start();
        }
    }
    else {
        if (QMovie::NotRunning != aliveMovie.state()) {
            aliveMovie.stop();
        }
    }
}

void Container::on_buildServer_clicked()
{
    QProcess *clientProcess = new QProcess(this);
    if (!serverState){

        clientProcess->execute("/usr/local/SmartHouse/server.sh");
        qDebug()<<"starting server..................";
        serverState = true;
        ui->serverState->setText("Server stop");
    }else{

        clientProcess->execute("killall server_arm");

        serverState = false;
        ui->serverState->setText("Server start");
    }
}

