#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
}
#endif
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "sscanf.h"
#include <EEPROM.h>
#include "DHT.h"
#include "variable_http.h"
#include "KhaiBao.h"
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

#include <IRrecv.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <ir_Mitsubishi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <SD.h>
// Port 1883
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
  #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

File myFile;

//#define notFibaro true


const int chipSelect = 16;

IPAddress ip10;
IPAddress ip ;
IPAddress gateway10;
IPAddress subnet10;
IPAddress DNS(8, 8, 8, 8);
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
IPAddress timeServerIP; // time.nist.gov NTP server address
unsigned int localPort = 2390;      // local port to listen for UDP packets
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

DHT dht(DHTPin, DHTTYPE);

unsigned long EEPROMReadlong(int address);
void update_fota();
void dumpInfo(decode_results *results);
void parseStringRAW(String str);
void parseStringGC(String str);
void getHC();
void printIP(void);
int waitConnected(void);
//int waitConnected1(void);
void ketnoimang() ;
void scanWiFi(void);
void saveWiFiConf(void);
void resetModuleId(void);
bool loadWiFiConf();
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
void setupWeb(void);
void setupWiFiConf(void);
void dumpCode (decode_results *results);
void conver_time();
void Tach_TCP(String str);
void nhan_TCP();

ESP8266HTTPUpdateServer httpUpdater;

WiFiClient client;
WiFiClient client1;
//############################################
PubSubClient clientmqtt(client1);
//#############################################
uint16_t CAPTURE_BUFFER_SIZE = 1400;
// Nr. of milli-Seconds of no-more-data before we consider a message ended.
// NOTE: Don't exceed MAX_TIMEOUT_MS. Typically 130ms.
#define TIMEOUT 100U  // Suits most messages, while not swallowing repeats.
IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT);
IRsend irsend(Send_PIN);
IRDaikinESP dakinir(Send_PIN);
IRMitsubishiAC mitsubir(Send_PIN);

/////////////////////////////////////////////////////////////////////////

ESP8266WebServer server(4999);
WiFiServer serverTCP(4998);
decode_results  results;

 
/* SETUP

*/
  struct Hengiostruct HG1;        // Declare QuyenSach1 of type Book
  struct Hengiostruct HG2;
  struct Hengiostruct HG3; 
void setup() {
  //ESP.eraseConfig();
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, LOW);
  Serial.begin(115200);
  EEPROM.begin(1024);
  delay(10);
  DEBUG_PRINTLN("Startup");
 
  pinMode(MotionPin, INPUT);
  irsend.begin();
  if (!loadWiFiConf()){
    resetModuleId();
    saveWiFiConf();
  }
 // motion_time = EEPROMReadlong(1000);
  conver_time();
  scanWiFi();
  DEBUG_PRINTLN("A");
  hoclenh = 0;
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WiFiConf.module_id, wifi_password);
  ketnoimang();
  statusmang = waitConnected();
  if (WiFi.status() == WL_CONNECTED) {
    update_fota();
    DEBUG_PRINTLN("Get HC");
    getHC();
    DEBUG_PRINTLN("Connect");
    WiFi.softAPdisconnect(true);
   // WiFi.softAP(WiFiConf.module_id, wifi_password, 6, 1);
  }
  printIP();
  httpUpdater.setup(&server, update_path, update_username, update_password);
  setupWeb();
  setupWiFiConf();
  DEBUG_PRINTLN("Server begin");
  server.begin();
  //server.setNoDelay(true);
  DEBUG_PRINTLN("MDNS");
  MDNS.begin("mIR");
  DEBUG_PRINTLN("TCP Server");
  serverTCP.begin();
  serverTCP.setNoDelay(true);
  DEBUG_PRINTLN("Daikin");
  dakinir.begin();
  DEBUG_PRINTLN("Mit");
  mitsubir.begin();
  DEBUG_PRINTLN("DHT");
  dht.begin();
  DEBUG_PRINTLN("Disable");
  digitalWrite(status_led, LOW);
  irrecv.disableIRIn();
  DEBUG_PRINTLN("Local IP");
  ip = WiFi.localIP();
  timeled = timedelay = millis();
  ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  _motion_status = 1;
  WiFi.macAddress(macAddr);
  MDNS.addService("http", "tcp", 4999);
  if (!SD.begin(chipSelect)) {
    DEBUG_PRINTLN("Card failed, or not present");
  }
  DEBUG_PRINTLN("card initialized.");
  user_using();
  HG1.state_status = state_no;
  HG2.state_status = state_no;
  HG3.state_status = state_no;
#ifdef notFibaro
  read_setting = read_file_setting("Setting/setting.txt", 1);
  read_setting_state = read_file_setting("Setting/state.txt", 2);
  read_setting_motion = read_file_setting("Setting/Motion.txt", 4);
#endif
  udp.begin(localPort);
  weekday = 8;
  //################################
  if (WiFiConf.sta_mqtt_port > 0){
  snprintf (WiFiConf.sta_mqtt_topic, 32, "mIR/%02x%02x%02x%02x%02x%02x",macAddr[WL_MAC_ADDR_LENGTH - 5], macAddr[WL_MAC_ADDR_LENGTH - 6],macAddr[WL_MAC_ADDR_LENGTH - 4], macAddr[WL_MAC_ADDR_LENGTH - 3],macAddr[WL_MAC_ADDR_LENGTH - 2], macAddr[WL_MAC_ADDR_LENGTH - 1]);
  DEBUG_PRINTLN(WiFiConf.sta_mqtt_topic);
  clientmqtt.setServer(WiFiConf.sta_mqtt_address, WiFiConf.sta_mqtt_port);
  clientmqtt.setCallback(callback);
  lastReconnectAttempt = 0;
  }
  else DEBUG_PRINTLN("Not using MQTT");
}


int hoc_ir(byte stt) {
  if (stt == 1) {
    if (irrecv.decode(&results)) {
      serialPrintUint64Hex(results.value);
      //dump(&results);
      dumpInfo(&results);
      dumpCode(&results);
      return 1;
    }
  }
  return 0;
}
void loop() {
  server.handleClient();
  //##################
  // MQTT ############
  if (WiFiConf.sta_mqtt_port > 0){
        if (!clientmqtt.connected()) {
        long now = millis();
        if (now - lastReconnectAttempt > 5000) {
          lastReconnectAttempt = now;
          if (reconnect()) {
            lastReconnectAttempt = 0;
          }
        }
      } else {
        clientmqtt.loop();
      }
  }
  switch (WiFi.status())
  {
    case WL_CONNECTED:
      // if (hoclenh == 1) {

      if (statusmang == 0) {
        WiFi.softAPdisconnect(true);
        getHC();
        statusmang = 1;
        _resetketnoi = 0;
        digitalWrite(status_led, LOW);
      }
      else if (hoc_ir(hoclenh) == 1) {
        digitalWrite(status_led, HIGH);
        _resetketnoi = 100;
        irrecv.resume();
      }
      else if (_resetketnoi == 2) {
        digitalWrite(status_led, LOW);
        _resetketnoi = _resetketnoi - 1;
      }
      else if (_resetketnoi > 0) {
        _resetketnoi = _resetketnoi - 1;
      }
      nhan_TCP();
      if (demgiay % 10 == 0) {
        demgiay++ ;
        float H = dht.readHumidity();
        float T = dht.readTemperature();
        if (isnan(H) || isnan(T)) {DEBUG_PRINTLN("Failed to read from DHT sensor!");}
        else {
          doam = H;
          nhietdo = T;

        }
      }
      else if (demgiay % 33 == 0) {
        send_udp();
        demgiay++;
      }
      else if (demgiay % 36 == 0) {
        demgiay = 1 ;
        gettime_udp();
#ifdef notFibaro
        if (read_setting == 0) read_setting = read_file_setting("Setting/setting.txt", 1);
        if (read_setting_state == 0) read_setting_state=read_file_setting("Setting/state.txt", 2);  
        if (read_setting_motion == 0) read_setting_motion=read_file_setting("Setting/Motion.txt", 4);  
#endif
      }
      break ;
    default:
      if (statusmang != 0) {
        WiFi.softAPdisconnect(false);
        statusmang = 0;
        _resetketnoi = 0;
      }
      if  ((_resetketnoi % 5000) == 0)ketnoimang() ;
      if ( (unsigned long) (millis() - timeled) > 500 ) {
        if (denled == 0) denled = 1;
        else denled = 0;
        digitalWrite(status_led, denled );
        timeled = millis();
      }
      _resetketnoi = _resetketnoi + 1;
      if (_resetketnoi >= 65000) {
        digitalWrite(status_led, HIGH );
        ESP.reset();
      }
      break;
  }
  delay(2);
  if (millis() - timedelay > 1000 ) {
    demgiay++;
    timedelay = millis();
  }
#ifdef notFibaro
  if (digitalRead(MotionPin) == 1) {
    if (_motion_status == 0) {
      if (thoigianthuc - motion_time > 20 ) {
        DEBUG_PRINTLN("Status Motion : 1");
        _motion_status = 1;
        motion_time = thoigianthuc;
        conver_time();
        write_file_setting("Setting/Motion.txt", 4);
      }
    }
  }
  else if (_motion_status == 1) {
    DEBUG_PRINTLN("Status Motion : 0");
    _motion_status = 0;
  }
      switch (weekday) {
        case 0 : if (HG1.is_sun) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_sun) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_sun) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 1 : if (HG1.is_mon) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_mon) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_mon) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 2 : if (HG1.is_tue) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_tue) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_tue) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 3 : if (HG1.is_wed) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_wed) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_wed) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 4 : if (HG1.is_thu) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_thu) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_thu) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 5 : if (HG1.is_fri) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_fri) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_fri) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        case 6 : if (HG1.is_sat) HG1.dung_ngay = true; else HG1.dung_ngay = false; if (HG2.is_sat) HG2.dung_ngay = true; else HG2.dung_ngay = false; if (HG3.is_sat) HG3.dung_ngay = true; else HG3.dung_ngay = false; break;
        default : HG1.dung_ngay = false; HG2.dung_ngay = false; HG3.dung_ngay = false; break;
      }

  //State machine for schedule
  /*
     Cách tính : lấy thời gian end trừ thời gian bắt đầu ra khoảng thời gian giữa 2 lần set
     biểu đồ sẽ chia cho 4 trong đó : 1 phần là tăng nhiệt độ khi ngủ, 2 phần duy trì và 1 phần  tăng nhiệt vào buổi sáng
     Trong phần tăng nhiệt độ thì sẽ chia làm 3 mốc : mốc begin cho nhiệt độ thấp nhất, mốc tiếp theo tăng 1 độ, mốc tiếp tăng 1 độ và mốc cuối cùng set đúng nhiệt độ cài đặt
     Phần duy trì nhiệt độ sẽ lấy mẫu sau 15 phút, nếu nhiệt độ thấp hơn cài đặt thì tăng 1 độ nếu nhiệt độ cao hơn cài đặt thì giảm 1 độ cho đến giai đoạn cuối
     Phần kết thúc : là khi trời sáng nhiệt độ phòng cần tăng lên hơn nhiệt độ cài đặt và sau đó đúng giờ thì tắt
  */
  switch (HG1.state_status) {
    case state_begin:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau / 4;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/18.txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_update1;
        write_file_setting("Setting/state.txt", 3);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update1:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau / 3;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/19.txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_update2;
        write_file_setting("Setting/state.txt", 4);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update2:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau / 2;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/20.txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_update3;
        write_file_setting("Setting/state.txt", 5);
      }
      break;
    case state_update3:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG1.temp_set) + ".txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_conti;
        write_file_setting(F("Setting/state.txt"), 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_conti:
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc + 900;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        if (float(HG1.temp_set) - nhietdo > 1) {
          String file = "ML/" + duongdan_ML + "/" + String(HG1.temp_set + 1) + ".txt";
          read_file_setting(file , 3 );
        }
        else {
          String file = "ML/" + duongdan_ML + "/" + String(HG1.temp_set - 1) + ".txt";
          read_file_setting(file , 3 );
        }
        if ((thoigianthuc - HG1.time_tam_cho_cac_buoc) > (HG1.khoang_time_cach_nhau * 6)) {
          HG1.state_status = state_update4;
          write_file_setting("Setting/state.txt", 2);
        }
      }
      //15 phút kiểm tra giu nhiet do đến nhỏ hơn thời gian kết thúc 1h thì chuyển trạng thái
      break;
    case state_update4:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau * 6;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG1.temp_set + 1) + ".txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_update5;
        write_file_setting("Setting/state.txt", 2);
      }
      // tăng nhiệt độ lên cao khi trời sáng
      break;
    case state_update5:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau * 7;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG1.temp_set + 2) + ".txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_wait;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_wait:
      HG1.time_tam_cho_cac_buoc = HG1.time_begin_int + HG1.khoang_time_cach_nhau * 7;
      HG1.time_tam_cho_cac_buoc = HG1.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG1.time_tam_cho_cac_buoc) {
        if (thoigianthuc > HG1.time_end_int){
         DEBUG_PRINTLN("TAM 8");
        String file = "ML/" + duongdan_ML + "/OFF.txt";
        read_file_setting(file , 3 );
        HG1.state_status = state_no;
        write_file_setting("Setting/state.txt", 2);
      }
      }
      // kết thúc điều khiển
      break;
    case state_no:
      if (!HG1.dung_ngay) {
        HG1.state_status = state_not_day;
        DEBUG_PRINTLN("Không dung ngay dieu khien 2");
      }
        //ON ML nhiệt độ thấp nhất
      else if (HG1.time_begin_int > HG1.time_end_int) {
            if ((thoigianthuc < HG1.time_end_int)||(thoigianthuc > HG1.time_begin_int))  {
            HG1.khoang_time_cach_nhau = ( HG1.time_end_int + 86400) - HG1.time_begin_int ;
            HG1.khoang_time_cach_nhau = HG1.khoang_time_cach_nhau / 8 ;
            //Serial.print("Khoang thoi gian cach nhau ");
            DEBUG_PRINTLN(HG1.khoang_time_cach_nhau);
            //ON ML
            String file = "ML/" + duongdan_ML + "/ON.txt" ;
            read_file_setting(file , 3 );
            HG1.state_status = state_begin;
            write_file_setting("Setting/state.txt", 2);
            }
        }
      else if ((thoigianthuc < HG1.time_end_int)&&(thoigianthuc > HG1.time_begin_int)) {
          HG1.khoang_time_cach_nhau = HG1.time_end_int - HG1.time_begin_int ;
          HG1.khoang_time_cach_nhau = HG1.khoang_time_cach_nhau / 8 ;
          //Serial.print("Khoang thoi gian cach nhau ");
          DEBUG_PRINTLN(HG1.khoang_time_cach_nhau);
          //ON ML
          String file = "ML/" + duongdan_ML + "/ON.txt" ;
          read_file_setting(file , 3 );
          HG1.state_status = state_begin;
          write_file_setting("Setting/state.txt", 2);
        }
      
      break;
    case state_not_day:
      if (HG1.dung_ngay) {
        HG1.state_status = state_no;
        DEBUG_PRINTLN("dung ngay dk");
      }
      // kết thúc điều khiển
      break;
  }
 //LAN 2
  switch (HG2.state_status) {
    case state_begin:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau / 4;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
         DEBUG_PRINTLN("TAM 3");
        String file = "ML/" + duongdan_ML + "/18.txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_update1;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update1:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau / 3;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 4");
        String file = "ML/" + duongdan_ML + "/19.txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_update2;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update2:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau / 2;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 5");
        String file = "ML/" + duongdan_ML + "/20.txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_update3;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_update3:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 6");
        String file = "ML/" + duongdan_ML + "/" + String(HG2.temp_set) + ".txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_conti;
        write_file_setting(F("Setting/state.txt"), 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_conti:
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc + 900;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 5");
        if (float(HG2.temp_set) - nhietdo > 1) {
          String file = "ML/" + duongdan_ML + "/" + String(HG2.temp_set + 1) + ".txt";
          read_file_setting(file , 3 );
        }
        else {
          String file = "ML/" + duongdan_ML + "/" + String(HG2.temp_set - 1) + ".txt";
          read_file_setting(file , 3 );
        }
        if ((thoigianthuc - HG2.time_tam_cho_cac_buoc) > (HG2.khoang_time_cach_nhau * 6)) {
          HG2.state_status = state_update4;
          write_file_setting("Setting/state.txt", 2);
        }
      }
      //15 phút kiểm tra giu nhiet do đến nhỏ hơn thời gian kết thúc 1h thì chuyển trạng thái
      break;
    case state_update4:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau * 6;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 6");
        String file = "ML/" + duongdan_ML + "/" + String(HG2.temp_set + 1) + ".txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_update5;
        write_file_setting("Setting/state.txt", 2);
      }
      // tăng nhiệt độ lên cao khi trời sáng
      break;
    case state_update5:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau * 7;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        DEBUG_PRINTLN("TAM 7");
        String file = "ML/" + duongdan_ML + "/" + String(HG2.temp_set + 2) + ".txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_wait;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_wait:
      HG2.time_tam_cho_cac_buoc = HG2.time_begin_int + HG2.khoang_time_cach_nhau * 7;
      HG2.time_tam_cho_cac_buoc = HG2.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG2.time_tam_cho_cac_buoc) {
        if (thoigianthuc > HG2.time_end_int){
         DEBUG_PRINTLN("TAM 8");
        String file = "ML/" + duongdan_ML + "/OFF.txt";
        read_file_setting(file , 3 );
        HG2.state_status = state_no;
        write_file_setting("Setting/state.txt", 2);
      }
      }
      // kết thúc điều khiển
      break;
    case state_no:
      if (!HG2.dung_ngay) {
        HG2.state_status = state_not_day;
        DEBUG_PRINTLN("Không dung ngay dieu khien 2");
      }
        //ON ML nhiệt độ thấp nhất
      else if (HG2.time_begin_int > HG2.time_end_int) {
            if ((thoigianthuc < HG2.time_end_int)||(thoigianthuc > HG2.time_begin_int))  {
            HG2.khoang_time_cach_nhau = ( HG2.time_end_int + 86400) - HG2.time_begin_int ;
            HG2.khoang_time_cach_nhau = HG2.khoang_time_cach_nhau / 8 ;
            //Serial.print("Khoang thoi gian cach nhau ");
            DEBUG_PRINTLN(HG2.khoang_time_cach_nhau);
            //ON ML
            String file = "ML/" + duongdan_ML + "/ON.txt" ;
            read_file_setting(file , 3 );
            HG2.state_status = state_begin;
            write_file_setting("Setting/state.txt", 2);
            }
        }
        else if ((thoigianthuc < HG2.time_end_int)&&(thoigianthuc > HG2.time_begin_int)) {
          HG2.khoang_time_cach_nhau = HG2.time_end_int - HG2.time_begin_int ;
          HG2.khoang_time_cach_nhau = HG2.khoang_time_cach_nhau / 8 ;
          //Serial.print("Khoang thoi gian cach nhau ");
          DEBUG_PRINTLN(HG2.khoang_time_cach_nhau);
          //ON ML
          String file = "ML/" + duongdan_ML + "/ON.txt" ;
          read_file_setting(file , 3 );
          HG2.state_status = state_begin;
          write_file_setting("Setting/state.txt", 2);
        }
      
      break;
    case state_not_day:
      if (HG2.dung_ngay) {
        HG2.state_status = state_no;
        DEBUG_PRINTLN("dung ngay dk 2");
      }
      // kết thúc điều khiển
      break;
  }
// LAN 3
  switch (HG3.state_status) {
    case state_begin:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau / 4;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/18.txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_update1;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update1:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau / 3;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/19.txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_update2;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update2:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau / 2;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/20.txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_update3;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_update3:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG3.temp_set) + ".txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_conti;
        write_file_setting(F("Setting/state.txt"), 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_conti:
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc + 900;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        if (float(HG3.temp_set) - nhietdo > 1) {
          String file = "ML/" + duongdan_ML + "/" + String(HG3.temp_set + 1) + ".txt";
          read_file_setting(file , 3 );
        }
        else {
          String file = "ML/" + duongdan_ML + "/" + String(HG3.temp_set - 1) + ".txt";
          read_file_setting(file , 3 );
        }
        if ((thoigianthuc - HG3.time_tam_cho_cac_buoc) > (HG3.khoang_time_cach_nhau * 6)) {
          HG3.state_status = state_update4;
          write_file_setting("Setting/state.txt", 2);
        }
      }
      //15 phút kiểm tra giu nhiet do đến nhỏ hơn thời gian kết thúc 1h thì chuyển trạng thái
      break;
    case state_update4:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau * 6;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG3.temp_set + 1) + ".txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_update5;
        write_file_setting("Setting/state.txt", 2);
      }
      // tăng nhiệt độ lên cao khi trời sáng
      break;
    case state_update5:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau * 7;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(HG3.temp_set + 2) + ".txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_wait;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_wait:
      HG3.time_tam_cho_cac_buoc = HG3.time_begin_int + HG3.khoang_time_cach_nhau * 7;
      HG3.time_tam_cho_cac_buoc = HG3.time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > HG3.time_tam_cho_cac_buoc) {
        if (thoigianthuc > HG3.time_end_int){
        DEBUG_PRINTLN("TAM 8");
        String file = "ML/" + duongdan_ML + "/OFF.txt";
        read_file_setting(file , 3 );
        HG3.state_status = state_no;
        write_file_setting("Setting/state.txt", 2);
      }
      }
      // kết thúc điều khiển
      break;
    case state_no:
      if (!HG3.dung_ngay) {
        HG3.state_status = state_not_day;
        DEBUG_PRINTLN("Không dung ngay dieu khien 2");
      }
        //ON ML nhiệt độ thấp nhất
      else if (HG3.time_begin_int > HG3.time_end_int) {
            if ((thoigianthuc < HG3.time_end_int)||(thoigianthuc > HG3.time_begin_int))  {
            HG3.khoang_time_cach_nhau = ( HG3.time_end_int + 86400) - HG3.time_begin_int ;
            HG3.khoang_time_cach_nhau = HG3.khoang_time_cach_nhau / 8 ;
            //Serial.print("Khoang thoi gian cach nhau ");
            DEBUG_PRINTLN(HG3.khoang_time_cach_nhau);
            //ON ML
            String file = "ML/" + duongdan_ML + "/ON.txt" ;
            read_file_setting(file , 3 );
            HG3.state_status = state_begin;
            write_file_setting("Setting/state.txt", 2);
            }
        }
        else if ((thoigianthuc < HG3.time_end_int)&&(thoigianthuc > HG3.time_begin_int)) {
          HG3.khoang_time_cach_nhau = HG3.time_end_int - HG3.time_begin_int ;
          HG3.khoang_time_cach_nhau = HG3.khoang_time_cach_nhau / 8 ;
          //Serial.print("Khoang thoi gian cach nhau ");
          DEBUG_PRINTLN(HG3.khoang_time_cach_nhau);
          //ON ML
          String file = "ML/" + duongdan_ML + "/ON.txt" ;
          read_file_setting(file , 3 );
          HG3.state_status = state_begin;
          write_file_setting("Setting/state.txt", 2);
        }
      
      break;
    case state_not_day:
      if (HG3.dung_ngay) {
        HG3.state_status = state_no;
        DEBUG_PRINT("dung ngay dk 3");
      }
      // kết thúc điều khiển
      break;
  }
#endif
}

