#ifndef LINKAGE_H
#define LINKAGE_H

#include "module.hpp"

class LinkageModule : public Module
{
    Q_OBJECT

private:
    LinkageModule(const LinkageModule& module);
    LinkageModule operator=(const LinkageModule& module);

    static QByteArray bytes;
    static QByteArray fingerbytes;
    static QByteArray rfidRechargebytes;
public:
    explicit LinkageModule(QObject *parent = 0);
    virtual ~LinkageModule();

    void getImage(QPixmap& pixmap);
    void getSupportedNodeIDs(QByteArray& ids);
    void createWindow(QWidget *pParentWindow = 0, const QRect *pRect = 0);
    bool negotiatePlatform(QString platform);
signals:
    void setTemp(int temp);
    void setHum(int hum);
    void setRfidvalue(quint32 value);

public slots:
    void onDeactivate();
    bool onMsg(const QByteArray& msg);
    void onLedLight(quint8 cmd);
    void onMotorOperation(quint8 cmd);
    void fingerprintRecord();
    void fingerprintSearch();
    void rfidRecharge(quint32 price);
    void rfidPay(quint32 value);
};




#endif // LINKAGE_H
