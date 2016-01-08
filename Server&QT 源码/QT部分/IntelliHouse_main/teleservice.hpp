#ifndef TELESERVICE_HPP
#define TELESERVICE_HPP
#include <QtGlobal>
#include <QObject>
#include "abstractteleservice.hpp"

class TeleService : public QObject, public AbstractTeleService
{
    Q_OBJECT

public:
    int AlarmDoppler(QString dest);
    int AlarmSmoke(QString dest);
    static TeleService* getService();
    static void releaseService();

signals:

public slots:

private:
    static TeleService* pService;
    explicit TeleService(QObject *parent = 0);
    explicit TeleService(const TeleService& service);
    TeleService& operator=(const TeleService& service);
    ~TeleService();
#if defined(_WS_QWS) || defined(Q_WS_QWS)
    int gprs_fd;
    void openGprs();
    void closeGprs();
    void init_ttyS();
    void GSM_Conf_Message();
    void GSM_GPRS_send_cmd_read_result(char *send_buf, int rcv_wait);
    int send_GSM_GPRS_cmd(char *send_buf);
    int read_GSM_GPRS_datas(char *rcv_buf,int rcv_wait);
    void SetDest(QString dest);
#endif // defined defined(_WS_QWS) || defined(Q_WS_QWS)
};

#endif // TELESERVICE_HPP
