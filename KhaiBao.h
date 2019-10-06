#define Send_PIN  15
#define RECV_PIN  2

#define status_led  0 //13 //0

//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define DHTPin 5
//#define MotionPin 4

uint8_t macAddr[6];
//## MQTT
boolean IR_leaning_MQTT=false;
String noiluu_MQTT="Amply/User/sassss.txt";
#define MQTT_MAX_PACKET_SIZE 512
long lastReconnectAttempt = 0;
//###
const int state_no = 0;
const int state_not_day = 1;
const int state_begin = 2;
const int state_update1 = 3;
const int state_update2 = 4;
const int state_update3 = 5;
const int state_conti = 6;
const int state_update4 = 7;
const int state_update5 = 8;
const int state_wait = 9;


String duongdan_ML="User";
String duongdan_TV="User";

byte _motion_status=0;
unsigned long motion_time=0;

float nhietdo=0;
float doam=0;

/* Cai dat update fota
 *  */
const char ApiKey[] = "BkxnF0xUl-";
const char ApiURL[] = "http://fota.vn";
const char AppVersion[] = "mIR-V0.4";  // Phien ban
const char timeVersion[] = " 9/4/2018-15:00";
/*
 * Wifi
 */
 
const char* wifi_password = "88888888";
const char* update_path = "/firmware";
const char* update_username = "mhome";
const char* update_password = "fibaro";
String ipStr="Nil";
String SerialHC2="Nil";

byte denled=0;

int status_failmang=0;
uint16_t _resetketnoi=0;
byte datasend = 0;
byte hoclenh = 0;
int truyenhc=0;
int statusmang=0;

String ts;
String json_ts="38000";


int demgiay=0;
unsigned long timeled=0;
unsigned long timeout = 0;
unsigned long timedelay = 0;
/////////////////////
#define WIFI_CONF_FORMAT {0, 0, 0, 1}
const uint8_t wifi_conf_format[] = WIFI_CONF_FORMAT;
#define NAME_PREF "mHome-"
String network_html;
#define WIFI_CONF_START 0
 
struct WiFiConfStruct {
  uint8_t format[4];
  char sta_ssid[32];
  char sta_pwd[64];
  char sta_ip[15];
  char sta_gateway[15];
  char sta_subnet[15];
  char sta_iphc2[20];
  char sta_passhc[100];
  char sta_global1[20];
  char sta_global2[20];
  char sta_global3[20];
  char sta_language[3];
  char module_id[32];
  char sta_ML[4];
  char sta_TV[4];
  char sta_MQ[4];
  char sta_Amply[4];
    char sta_mqtt_address[64];
  uint16_t sta_mqtt_port;
  char sta_mqtt_user[64];
  char sta_mqtt_pass[64];
  char sta_mqtt_topic[32];
  char sta_DHCP[4];
} WiFiConf = {
  WIFI_CONF_FORMAT,
  "aaaa",
  "",
  "192.168.1.240",
  "192.168.1.1",
  "255.255.255.0",
  "",
  "",
  "",
  "",
  "",
  "0",
  "",
  "0",
  "0",
  "0",
  "0",
  "mhome-nhamau.ddns.net",
  1883,
  "x",
  "x",
  "IN_MQTT",
  "1"
};


/*
 * Thoi gian thuc doc tu server
 */
boolean read_setting=0;
boolean read_setting_state=0;
boolean read_setting_motion=0;
/*
unsigned long time_tam_cho_cac_buoc = 600;
int temp_set=0;
int khoang_time_cach_nhau=300;
unsigned long time_begin_int=0;
unsigned long time_end_int=0;
boolean dung_ngay=false;
boolean is_mon = 0 ;
boolean is_tue = 0 ;
boolean is_wed = 0 ;
boolean is_thu = 0 ;
boolean is_fri = 0 ;
boolean is_sat = 0 ;
boolean is_sun = 0 ;
*/
struct Hengiostruct {
    int state_status=0;
    unsigned long time_tam_cho_cac_buoc = 600;
    int temp_set=0;
    int khoang_time_cach_nhau=300;
    unsigned long time_begin_int=0;
    unsigned long time_end_int=0;
    boolean dung_ngay=false;
    boolean is_mon = 0 ;
    boolean is_tue = 0 ;
    boolean is_wed = 0 ;
    boolean is_thu = 0 ;
    boolean is_fri = 0 ;
    boolean is_sat = 0 ;
    boolean is_sun = 0 ;
};

unsigned long thoigianthuc=0;
String time_ ;
int year;
int month;
int day;
int hour;
int minute;
int weekday;

String time_begin="";
String time_end="";


