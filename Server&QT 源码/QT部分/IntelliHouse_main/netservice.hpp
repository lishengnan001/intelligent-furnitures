#ifndef NETSERVICE_HPP
#define NETSERVICE_HPP

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include "abstractnetservice.hpp"
#include <sys/socket.h>

#define ZIGBEE_INFO_LENGTH 32

struct socket_cmd {

  unsigned short  cmd_len;
  unsigned char   cmd_id;
  unsigned char   para_num;

  union {
    int intvalue;
    float f_value;
    unsigned long ul_value;

    struct bw_speed {
      short port;
      unsigned short direction;
      int speed;

    }BW_SPEED;

    struct broadcast_storm {
      unsigned int storm_en;
      unsigned int protect_mc;
      unsigned int bc_value;
      unsigned int threshold[6];
    }BROADCAST_STORM;

    struct remote_upgrade {
      int which_upgrade;
      char kernel_name[64];
      char ramdisk_name[64];

    }REMOTE_UPGRADE;

    unsigned short vid[4];

    unsigned char vlpri_status;   // b_cos_status
    unsigned char prid[4];        // gus_all_cos

    unsigned char portq_status;  // b_portpri_status
    unsigned char port_queue[4];  // guc_all_portpri

  } cmd_ins ;
};

struct global_socket {

  struct socket_cmd *cmd;

  int gateway_num;
/*
  struct gateway {
    struct sockaddr_in serv_addr;
    int sock_fd;
    unsigned int state :1,
                 online:1,
                 inuse :1,
                 sync  :1,
                 unuse :28;
  } gw_array[S_SIZE];
*/
  int sk_server_fd;
  int nsk_server_fd;

  unsigned int write_flag;

  char * input_bf_ptr;
  ssize_t input_bf_size;

  char * output_bf_ptr;
  ssize_t output_bf_size;
};

enum sk_cmd_id {

    CMD_ID_SET_LED_1_ON          , //  1
    CMD_ID_SET_LED_1_OFF         , //  2
    CMD_ID_SET_LED_2_ON          , //  3
    CMD_ID_SET_LED_2_OFF         , //  4
    CMD_ID_SET_LED_3_ON          , //  5
    CMD_ID_SET_LED_3_OFF         , //  6
    CMD_ID_SET_LED_4_ON          , //  7
    CMD_ID_SET_LED_4_OFF         , //  8
    CMD_ID_SET_LED_ALL_ON        , //  9
    CMD_ID_SET_LED_ALL_OFF       , // 10
    CMD_ID_SET_MOTOR_LEFT        , // 11
    CMD_ID_SET_MOTOR_RIGHT       , // 12
    CMD_ID_SET_MOTOR_STOP        , // 13
    CMD_ID_SET_PWD_00            , // 14
    CMD_ID_SET_PWD_01            , // 15
    CMD_ID_SET_PWD_02            , // 16
    CMD_ID_SET_PWD_03            , // 17
    CMD_ID_SET_PWD_04            , // 18
    CMD_ID_SET_PWD_05            , // 19
    CMD_ID_SET_PWD_06            , // 20
    CMD_ID_SET_PWD_07            , // 21
    CMD_ID_SET_PWD_08            , // 22
    CMD_ID_SET_PWD_09            , // 23
    CMD_ID_SET_VOLUME_DECREASE   , // 24
    CMD_ID_SET_VOLUME_INCREASE   , // 25
    CMD_ID_SET_CHANNEL_FORWARD   , // 26
    CMD_ID_SET_CHANNEL_BACKWARD  , // 27



};

class QTextStream;
using namespace std;

class NetService : public QObject, public AbstractNetService
{
    Q_OBJECT
public:
    explicit NetService(QObject * parent = 0);
    ~NetService();
    static NetService* getService();
    static void releaseService();
    bool socketConnected;

public slots:
	void acceptConnection();
	void startRead();
    void startWrite(const QByteArray& data);
   // void serviceDispatch(char* pBuffer);
    void serviceDispatch(QByteArray ba);
private:
    QTcpServer tcpServer;
    QTcpSocket* tcpSocket;
    char buffer[512];
    static NetService *pService;
    static QByteArray ledBytes;
    static QByteArray pwmBytes;
    static QByteArray airBytes;
    static QByteArray tvBytes;
    static QByteArray ledScreemBytes;
    char inputBuf[512];
    struct global_socket glb_ins;
};

#endif // NETSERVICE_HPP
