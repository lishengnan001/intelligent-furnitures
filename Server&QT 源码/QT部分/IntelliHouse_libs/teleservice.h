#ifndef TELESERVICE_H
#define TELESERVICE_H

#include <QObject>

class TeleService : public QObject
{
    Q_OBJECT
public:
    explicit TeleService(QObject *parent = 0);
    ~TeleService();
    int AlarmDoppler(QString dest);
    int AlarmSmoke(QString dest);

signals:

public slots:

private:
    int gprs_fd;
    void openGprs();
    void closeGprs();
    void init_ttyS();
    void GSM_Conf_Message();
    void GSM_GPRS_send_cmd_read_result(char *send_buf, int rcv_wait);
    int send_GSM_GPRS_cmd(char *send_buf);
    int read_GSM_GPRS_datas(char *rcv_buf,int rcv_wait);
    void SetDest(QString dest);

};

#endif // TELESERVICE_H
