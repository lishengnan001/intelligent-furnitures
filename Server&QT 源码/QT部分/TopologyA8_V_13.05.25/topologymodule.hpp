#ifndef TOPOLOGYMODULE_HPP
#define TOPOLOGYMODULE_HPP
#include "module.hpp"

class TopologyModule : public Module
{
    Q_OBJECT

private:
    TopologyModule(const TopologyModule& module);
    TopologyModule operator=(const TopologyModule& module);
    static QByteArray bytes;
    char *getName(int name);

public:
    explicit TopologyModule(QObject *parent = 0);
    virtual ~TopologyModule();
    void getImage(QPixmap& pixmap);
    void getSupportedNodeIDs(QByteArray& ids);
    void createWindow(QWidget *pParentWindow = 0, const QRect *pRect = 0);
    bool negotiatePlatform(QString platform);
    QString transQBtoQS(const QByteArray &data, int size);

public slots:
    void onDeactivate();
    void onLedLight(quint8 cmd);
    bool onMsg(const QByteArray& msg);
};

#endif // TOPOLOGYMODULE_HPP
