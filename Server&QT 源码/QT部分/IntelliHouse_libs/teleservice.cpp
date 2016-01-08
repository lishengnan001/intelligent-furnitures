#include <QString>
#include <fcntl.h>	// open() close()
#include <unistd.h>	// read() write()

#include <termios.h>	// set baud rate
#include <fcntl.h>
#include <sys/select.h>
//#include <sys/time.h>
#include <sys/types.h>
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include "stdio.h"

#include "teleservice.h"

#define DEVICE_TTYS "/dev/ttySAC1"
#define MAX_LEN_OF_SHORT_MESSAGE	100

#define RECEIVE_BUF_WAIT_1S 1
#define RECEIVE_BUF_WAIT_2S 2
#define RECEIVE_BUF_WAIT_3S 3
#define RECEIVE_BUF_WAIT_4S 4
#define RECEIVE_BUF_WAIT_5S 5

TeleService::TeleService(QObject *parent) :
    QObject(parent)
{

}

TeleService::~TeleService() {

}

void TeleService::SetDest(QString dest) {
    char cmd_buf[23];

    bzero(cmd_buf,sizeof(cmd_buf));

    cmd_buf[0]='a';
    cmd_buf[1]='t';
    cmd_buf[2]='+';
    cmd_buf[3]='c';
    cmd_buf[4]='m';
    cmd_buf[5]='g';
    cmd_buf[6]='s';
    cmd_buf[7]='=';
    cmd_buf[8]='"';

    QByteArray ba = dest.toAscii();
    const char *cmd_number = ba.data();
    strcat(cmd_buf,cmd_number);


    cmd_buf[20]='"';
    cmd_buf[21]='\r';
    cmd_buf[22]='\0';

    // send cmd :  at+cmgs="(telephone number)"
    GSM_GPRS_send_cmd_read_result(cmd_buf,RECEIVE_BUF_WAIT_1S);

}

int TeleService::AlarmDoppler(QString dest) {
    openGprs();
    if (-1 == gprs_fd) {
        return -1;
    }
    SetDest(dest);

    char doppler_message_buf[MAX_LEN_OF_SHORT_MESSAGE];
    char *message = "Warning of ChinaSofti:somebody broken-in!";
    int count = sizeof("Warning of ChinaSofti:somebody broken-in!") - 1;
    bzero(doppler_message_buf,sizeof(doppler_message_buf));

    // input short message

    for (int i = 0; i < count; i ++){
        doppler_message_buf[i] = message[i];
    }

    doppler_message_buf[count ++] = 0x1A;
    doppler_message_buf[count ++] = '\r';
    doppler_message_buf[count] = '\0';

    GSM_GPRS_send_cmd_read_result(doppler_message_buf,RECEIVE_BUF_WAIT_4S);
    closeGprs();
    return 0;
}

int TeleService::AlarmSmoke(QString dest) {
    openGprs();
    if (-1 == gprs_fd) {
        return -1;
    }
    SetDest(dest);
    char smoke_message_buf[MAX_LEN_OF_SHORT_MESSAGE];

    bzero(smoke_message_buf,sizeof(smoke_message_buf));

    // input short message

    smoke_message_buf[0] = 'f';
    smoke_message_buf[1] = 'i';
    smoke_message_buf[2] = 'r';
    smoke_message_buf[3] = 'e';
    smoke_message_buf[4] = '!';
    smoke_message_buf[5] = '!';
    smoke_message_buf[6] = '!';

    smoke_message_buf[7] = 0x1A;
    smoke_message_buf[8] = '\r';
    smoke_message_buf[9] = '\0';

    GSM_GPRS_send_cmd_read_result(smoke_message_buf,RECEIVE_BUF_WAIT_4S);
    closeGprs();
    return 0;
}

int TeleService::read_GSM_GPRS_datas(char *rcv_buf,int rcv_wait)
{
    int retval;
    fd_set rfds;
    struct timeval tv;

    int ret,pos;

    tv.tv_sec = rcv_wait;	// wait 2.5s
    tv.tv_usec = 0;

    pos = 0; // point to rceeive buf

    while (1)
    {
            FD_ZERO(&rfds);
            FD_SET(gprs_fd, &rfds);

            retval = select(gprs_fd+1 , &rfds, NULL, NULL, &tv);

            if (retval == -1)
            {
                    perror("select()");
                    break;
            }
            else if (retval)
            {// pan duan shi fou hai you shu ju
                    ret = ::read(gprs_fd, rcv_buf+pos, 2048);
                    pos += ret;
                    if (rcv_buf[pos-2] == '\r' && rcv_buf[pos-1] == '\n')
                    {
                            FD_ZERO(&rfds);
                            FD_SET(gprs_fd, &rfds);

                            retval = select(gprs_fd+1 , &rfds, NULL, NULL, &tv);

                            if (!retval) break;// no datas, break
                    }
            }
            else
            {
                    printf("No data\n");
                    break;
            }
    }

    return 1;
}


int TeleService::send_GSM_GPRS_cmd(char *send_buf)
{
    ssize_t ret;

    ret = ::write(gprs_fd,send_buf,strlen(send_buf));
    if (ret == -1)
    {
            printf ("write device %s error\n", DEVICE_TTYS);
            return -1;
    }

    return 1;
}

void TeleService::GSM_GPRS_send_cmd_read_result(char *send_buf, int rcv_wait)
{
    char rcv_buf[2048];

    if((send_buf==NULL) || (send_GSM_GPRS_cmd(send_buf)))
    {	// send success , then read
            bzero(rcv_buf,sizeof(rcv_buf));
            if (read_GSM_GPRS_datas(rcv_buf,rcv_wait))
            {
                    printf ("%s\n",rcv_buf);
            }
            else
            {
                    printf ("read error\n");
            }

    }
    else
    {
            printf("write error\n");
    }
}

void TeleService::GSM_Conf_Message()
{
    char *send_buf="at+cmgf=1\r";
    char *send_center_buf="at+csca=\"+8613800100500\"\r";


    GSM_GPRS_send_cmd_read_result(send_buf,RECEIVE_BUF_WAIT_1S);

    // set short message center number
    GSM_GPRS_send_cmd_read_result(send_center_buf,RECEIVE_BUF_WAIT_1S);

    printf("end config short message env\n");
}


void TeleService::init_ttyS()
{
    struct termios options;
    bzero(&options, sizeof(options));       // clear options
    cfsetispeed(&options,B115200);            // setup baud rate
    cfsetospeed(&options,B115200);
    options.c_cflag |= (CS8 | CREAD);
    options.c_iflag = IGNPAR;
    tcflush(gprs_fd, TCIFLUSH);
    tcsetattr(gprs_fd, TCSANOW, &options);
}


void TeleService::openGprs()
{
    gprs_fd = ::open(DEVICE_TTYS, O_RDWR);

            if (gprs_fd == -1)
            {
                    printf("open device %s error\n",DEVICE_TTYS);
                    //QMessageBox::critical(this, tr("打开失败"), tr("未能打开串口 ") + DEVICE_TTYS + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
            }
            else
            {
                    init_ttyS();	// init device
                    GSM_Conf_Message();	// GSM/GPRS functions

            }

}

void TeleService::closeGprs()
{
    if (::close(gprs_fd)!=0) printf("close device %s error",DEVICE_TTYS);
}
