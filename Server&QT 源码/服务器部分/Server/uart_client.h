/*
 * FILENAME: 
 *        etc_socket_client.h
 *
 * DESCRIPTION: 
 *        This is header file for gateway handling type and declaration.
 *
 * ORIGINAL DATE:
 *        05/08/2012
 *
 * AUTHOR:
 *        Yuhui
 *
 * MODIFIED DATE:
 *
 * MODIFIED LOG:
 *
 * MODIFIED AUTHOR:
 *
 * New Feature
 * 
 * VERSION:
 *        Comment Version:      0.0.5
 *
 * COPYRIGHT
 *        Copyright (c) 12        Chinasoft International LTD.
 */

#define L_SIZE 16
#define S_SIZE    64  /* Short */
#define SOCKET_CLIENT_PORT      8888  //6666
#define SOCKET_SERVER_GET_PORT  6666
#define SOCKET_SERVER_SEND_PORT 7777
#define SOCKET_BACKLOG     4
#define   XS_SIZE    32  /* Extra short */


#define HAVE_CONTRAL_SERVER

 #define SOCKETFDLEN 5
 #define UARTBUFSIZE 60
 #define LEDBUFSIZE  38
 #define SOCKETBUFSIZE 100
 

 struct gateway_endpoint {
 
   int endpoint_num;
   int gw_id;
 
   // assume only one type endpoint in the network
   struct hall {
	 unsigned char occur;
	 unsigned char hall_id;
	 unsigned char hall_state;
	 unsigned int  mf_detected_times;
	 unsigned int  mf_intensity;
   } hall_ins;
 
   struct photoresistor {
	 unsigned char occur;
	 unsigned char pr_id;
	 unsigned int  pr_val;
	 unsigned int  highest_value;
	 unsigned int  lowest_value;
	 unsigned int  average_value;
   } pr_ins;
 
   struct temp_hum {
	 unsigned char occur;
	 float temp_val;
	 float temp_highest;
	 float temp_lowest;
	 float temp_average;
	 float hum_val;
	 float hum_highest;
	 float hum_lowest;
	 float hum_average;
   } temp_hum_ins;
 
   struct shake {
	 unsigned char occur;
	 unsigned char shake_state;
	 unsigned int  times;
	 unsigned int  frequency;
   } shake_ins;
 
   struct reed {
	 unsigned char occur;
	 unsigned char reed_state;
	 unsigned int  mf_detected_times;
	 unsigned int  mf_intensity;
   } reed_ins;
 
   struct accelerate {
	 unsigned char occur;
	 float value_x;
	 float value_y;
	 float value_z;
   } acc_ins;
 
   struct smoke {
	 unsigned char occur;
	 unsigned char smoke_state;
	 unsigned int  times;
	 unsigned char emergency;
   } smoke_ins;
    struct fire {
	 unsigned char occur;
	 unsigned char fire_state;
	 unsigned int  times;
	 unsigned char emergency;
   } fire_ins;
   struct doppler {
	 unsigned char occur;
	 unsigned char doppler_state;
	 unsigned int  times;
	 unsigned char emergency;
   } doppler_ins;
 
   struct leds_motor {
	 unsigned char occur;
	 unsigned char leds_state[4];	// 0:close, 1:open
	 unsigned char leds_all_state;	// 0:close, 1:open
	 unsigned char motor_state; 	// 1:left, 2:right, 3:stop
   } leds_motor_ins;
 
   struct led_pwm {
	 unsigned char occur;
	 unsigned char pwm_val;  // 0~9
   } led_pwm_ins;
 
   struct sound {
	 unsigned char occur;
	 unsigned char sound_state;
	 unsigned int  times;
	 unsigned char emergency;
   } sound_ins;
 
   struct voltage {
	 unsigned char occur;
	 unsigned char voltage_state;
	 unsigned int  voltage_time;
   } voltage_ins;
 
   struct current {
	 unsigned char occur;
	 unsigned char current_state;
	 unsigned int  current_time;
   } current_ins;
 
   struct touch {
	 unsigned char occur;
	 unsigned char touch_state;
	 unsigned int  touch_time;
   } touch_ins;
 
   struct supwave {
	 unsigned char occur;
	 float sw_val;
   } supwave_ins;
 
   struct rfid {
	 unsigned char occur;
	 unsigned char rfid_val;
   } rfid_ins;
 
   struct pyroele {
	 unsigned char occur;
	 unsigned char pyr_state;
	 unsigned int  pyr_time;
   } pyr_ins;
 
   struct tv {
	 unsigned char occur;
	 unsigned char channel;
	 unsigned char volume;
   } tv_ins;
 
 } gw_ep_ins[S_SIZE];

  

  typedef struct {
  
  	int coor_uartfd;
	int socketfd_in[SOCKETFDLEN];
	int socketfd_out[SOCKETFDLEN];
	int socketfd_in_login[SOCKETFDLEN];
	int gateway_num;
	struct gateway {
		struct sockaddr_in serv_addr;
		int sock_fd;
		unsigned int state :1,
			online:1,
			inuse :1,
			sync  :1,
			unuse :28;
		} gw_array[S_SIZE];
	unsigned char uart_input_buf[UARTBUFSIZE];
	unsigned char uart_output_buf[UARTBUFSIZE];
	char socket_input_buf[SOCKETBUFSIZE];
	char socket_output_buf[SOCKETBUFSIZE];

	int uart_buf_size;
	int send_sensor_data;
	int uart_write_flag;
	int led_view;
	 pthread_mutex_t mutex;
	 pthread_mutex_t socket_mutex;
  }global_data_base;

struct sk_cmd_pkg {
  long no;
  long para_1;
  long para_2;
  unsigned char led_screen[48];
  long para_3;
  long para_4;
} sk_cmd_pkg_ins;

enum sk_cmd_id {

  CMD_ID_GET_HALL_STATE = 1      , // 01
  CMD_ID_SET_TV_VC  , // 02
  CMD_ID_GET_TEMP_HUMI_VAL       , // 03
  CMD_ID_GET_SHAKE_STATE         , // 04
  CMD_ID_GET_REED_STATE          , // 05
  CMD_ID_GET_ACCELERATE          , // 06
  CMD_ID_GET_SMOKE_STATE         , // 07
  CMD_ID_GET_DOPPLER_STATE       , // 08
  CMD_ID_GET_SET_LEDS_MOTOR      , // 09
  CMD_ID_GET_SET_PWM_LEDOLD        , // 10调光灯
  CMD_ID_GET_ENDPOINT_TYPE       , // 11
  CMD_ID_GET_ENDPOINT_INFO       , // 12
  CMD_ID_GET_SET_RFID_VAL        , // 13
  CMD_ID_GET_ENDPOINT_STATE = 16 , // 16
 
  CMD_ID_GET_SOUND_STATE         , // 17 (0b)
  CMD_ID_GET_PYRINF_STATE        , // 18 (0c)
  CMD_ID_GET_SUPWAVE_VAL         , // 19 (0d)
 // CMD_ID_SET_TV_VOLU_CHAN        , // 20 (20) 
  CMD_ID_GET_TOUCH_STATE         , // 21 (0f)
  CMD_ID_SET_TV_VOLU_CHAN = 22		  , // 22 (16)电视

  CMD_ID_SET_LED_SCREEM= 31,  //(1F)Led屏
  CMD_ID_SET_LED_ALARM ,	   //LED报警灯

  CMD_ID_SET_CHUANGLIAN_CHAN = 34      ,//(22)窗帘
  CMD_ID_SET_LED1,                   //智能灯1
  CMD_ID_SET_HUM,                           //加湿器
   CMD_ID_SET_TAIDENG,                 //台灯
   CMD_ID_SET_FENGSHAN,            //风扇
  CMD_ID_GET_VOICE_STATE , //(27)语音
  CMD_ID_SET_LEDHEAD1,        //智能灯头1
  CMD_ID_SET_LEDHEAD2,        //智能灯头2
  CMD_ID_SET_LEDHEAD3,        //智能灯头3
  CMD_ID_SET_LEDHEAD4,        //智能灯头4
  CMD_ID_SET_LED2,                    //智能灯头2
  CMD_ID_GET_SET_PWM_LED1,  //(2D)调光灯1
  CMD_ID_GET_SET_PWM_LED2, //(2E)调光灯2
  CMD_ID_GET_SET_PWM_LED3 = 48, //(30)调光灯3

    /*liqiang add for aircondition 2015-1-20 16:12:59*/
  CMD_ID_SET_AIRCONDITION = 24         , //空调
};


enum sk_cmd_id_gw {

    CMD_ID_SET_LED_1_ON = 1          , //  1
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
    
    CMD_ID_SET_TV_SWITCH =64 , //(40)电视开关
    CMD_ID_SET_CHANNEL_FORWARD   , // (41)频道加
    CMD_ID_SET_CHANNEL_BACKWARD  , // (42)频道减
    CMD_ID_SET_VOLUME_INCREASE   , // (43)音量加
    CMD_ID_SET_VOLUME_DECREASE   , // (44)音量减
    CMD_ID_SET_VOLUME_QUICK,

    CMD_ID_SET_AIR_FENGXIANG = 80       ,//(50)摇头
    CMD_ID_SET_AIR_FENGSU               ,//(51)风速
    CMD_ID_SET_AIR_MOSHI                ,//(52)模式
    CMD_ID_SET_AIR_WENDUJIA             ,//(53)调节温度+
    CMD_ID_SET_AIR_WENDUJIAN            ,//(54)调节温度-
    CMD_ID_SET_AIR_KAIGUAN              ,//(55)开关    

    CMD_ID_SET_FENGSHAN_01= 96          ,//(60) 风扇开
    CMD_ID_SET_FENGSHAN_02              ,//(61) 风扇关
    CMD_ID_SET_FENGSHAN_03              ,//(62) 风扇摇头
    CMD_ID_SET_FENGSHAN_04              ,//(63) 风扇风速
};

enum PWMLIGHT{
	    CMD_ID_SET_PWD_00		, // 0
	    CMD_ID_SET_PWD_01		, // 1
	    CMD_ID_SET_PWD_02		, // 2
	    CMD_ID_SET_PWD_03		, // 3
	    CMD_ID_SET_PWD_04 		, // 4
	    CMD_ID_SET_PWD_05 		, // 5
	    CMD_ID_SET_PWD_06            , // 6
	    CMD_ID_SET_PWD_07            , // 7
	    CMD_ID_SET_PWD_08            , // 8
	    CMD_ID_SET_PWD_09            , // 9
};

struct socket_cmd {
  unsigned short  cmd_len;  
  unsigned char   cmd_id;
  unsigned char   para_num;
};

typedef struct {
  unsigned char header_1;  // Header 1 (CC)
  unsigned char header_2;  // Header 2 (EE)
  unsigned char node_seq;  // Module Sequence (NO, default is 01)
  unsigned char node_id;   // Module ID (00 means gateway)
  unsigned char cmd;       // ParamH + ParamL
  unsigned char data[10];
  unsigned char tailer;    // Tailer
} UART_Format;

typedef enum {
  coor = 0,   // Coordinator (binding with gateway)
  hall,       // Hall
  pr,         // Photoresistor
  temp_hum,   // Temperature and Humidity
  shake,      // Shake
  reed,       // Reed
  acc,        // Accelerate
  smoke,      // Smoke
  doppler,    // Doppler
  motor,      // Motor
  led_pwm,    // LED Brightness Adjustment
  sound,      // Sound
  voltage,    // Voltage
  current,    // Current
  touch,      // Touch
  ultrasonic, // Ultrasonic Wave
  rfid_1356,  // 13.56M RFID Card
} Device_Type_List;

enum endpoint_event {
  COORDINATOR_ALIVE = 0            , //  0, Coordinator (binding with gateway)
  COORDINATOR_DEAD                 , //  1, Out of control
  HALL_NO_MAGNETIC_FIELD_DETECTED  , //  2
  HALL_MAGNETIC_FIELD_DETECTED     , //  3
  PHOTORESISTOR_AD_VALUE           , //  4
  TEMPERATURE_VALUE                , //  5
  HUMIDITY_VALUE                   , //  6
  SHAKE_NO_DETECTED                , //  7
  SHAKE_DETECTED                   , //  8
  REED_NO_MAGNETIC_FIELD_DETECTED  , //  9
  REED_MAGNETIC_FIELD_DETECTED     , // 10
  ACCELERATE_X_VALUE               , // 11
  ACCELERATE_Y_VALUE               , // 12
  ACCELERATE_Z_VALUE               , // 13
  SMOKE_NO_DETECTED                , // 14
  SMOKE_DETECTED                   , // 15
  DOPPLER_NO_ONE                   , // 16
  DOPPLER_HAS_ONE                  , // 17
  MOTOR_STOP                       , // 18
  MOTOR_TURN_LEFT                  , // 19
  MOTOR_TURN_RIGHT                 , // 20
  LEDS_X_STATES                    , // 21
  LED_PWM_VALUE                    , // 22
  SOUND_NO_DETECTED                , // 23
  SOUND_DETECTED                   , // 24
  VOLTAGE_VALUE                    , // 25
  CURRENT_VALUE                    , // 26
  TOUCH_NO_DETECTED                , // 27
  TOUCH_DETECTED                   , // 28
  ULTRASONIC_WAVE_NO_DETECTED      , // 29
  ULTRASONIC_WAVE_DETECTED         , // 30
  RFID_1356_GET_VALUE              , // 31
  RFID_1356_SET_VALUE              , // 32
};

int sk_cmd_parse(char *sk_cmd);
int write_coor_uart(int fd, char *uart_cmd, int cmd_len);
void  build_sk_package ( void *buffer,
				unsigned char nodeid,
				unsigned char cmd ,unsigned char data);
int sk_cmd_dispatch(char *client_cmd);

void sig_chld(int signo);
int OpenUart(void);
void str_cli(int sockfd);
int collection_endpoint_info(char *endpoint_info, int coor_id);
int read_coor_uart(char *sensor_data);
int get_local_time(char *str_local_time);






