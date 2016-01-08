#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <QWidget>
#include <QTimeLine>
#include <QMovie>

class QTimer;
class Panel;

using namespace std;

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
    Ui::Container *ui;
    const static quint8 TIMER_INTERVAL = 100;
    const static quint16 TIME_THRESHOLD = 5000;
    quint8 pageNum;
    quint8 curPage;
    static const int pageWidth = 480;
    static const int frameDelta = 5;
    QTimeLine timeLine;
    bool animationInProgress;
    bool animationDirection;
    QPoint slidePos;
    QTimer *pTimer;
    Panel *pPanel;
    QMovie aliveMovie;
    quint16 healthValue;
    bool serverState;
    bool isConnect;
signals:
    void healthChanged(bool health);
private slots:
    void on_buildServer_clicked();
    void onTimer();
    void shiftPage(int frame);
    void setHealth(bool health);
protected:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    void shiftNext();
    void shiftPrevious();
};

#endif // CONTAINER_HPP
