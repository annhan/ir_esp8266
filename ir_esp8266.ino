#include "DHT.h"
#include "variable_http.h"
#include "KhaiBao.h"
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <ir_Mitsubishi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <SD.h>
File myFile;


const int chipSelect = 16;


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
//void delay_comang(int thoigian) ;
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
IRrecv irrecv(RECV_PIN);
IRsend irsend(Send_PIN);
IRDaikinESP dakinir(Send_PIN);
IRMitsubishiAC mitsubir(Send_PIN);

/////////////////////////////////////////////////////////////////////////

ESP8266WebServer server(4999);
WiFiServer serverTCP(4998);
decode_results  results;
irparams_t save;

/* SETUP

*/
void setup() {
  //ESP.eraseConfig();
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, LOW);
  Serial.begin(115200);
  EEPROM.begin(1024);
  delay(10);
  Serial.println(F("Startup"));
  pinMode(MotionPin, INPUT);
  irsend.begin();
  if (!loadWiFiConf()) {
    resetModuleId();
    saveWiFiConf();
  }
  motion_time = EEPROMReadlong(1000);
  Serial.println(motion_time);
  conver_time();
  Serial.println(time_);
  scanWiFi();
  Serial.println("A");
  hoclenh = 0;
  WiFi.mode(WIFI_AP_STA);

  ketnoimang();
  // Serial.println("D");
  statusmang = waitConnected();
  // Serial.println("E");
  if (WiFi.status() == WL_CONNECTED) {
    update_fota();
    Serial.println("Connect");
    WiFi.softAP(WiFiConf.module_id, wifi_password, 6, 1);
  }
  else {
    //WiFi.disconnect();

    // delay(500);
    //WiFi.mode(WIFI_AP);
    Serial.println("Not connetted wifi");
    WiFi.softAP(WiFiConf.module_id, wifi_password);
  }

  printIP();
  httpUpdater.setup(&server, update_path, update_username, update_password);
  Serial.println(WiFi.localIP());
  setupWeb();
  setupWiFiConf();
  server.begin();
  MDNS.begin(WiFiConf.module_id);

  serverTCP.begin();
  dakinir.begin();
  mitsubir.begin();
  dht.begin();
  digitalWrite(status_led, HIGH); //irrecv.enableIRIn();}
  irrecv.disableIRIn();
  getHC();
  IPAddress ip = WiFi.localIP();
  timeled = timedelay = millis();
  ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  _motion_status = 1;
  MDNS.addService("http", "tcp", 4999);
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    return;
  }
  Serial.println(F("card initialized."));
  user_using();
  state_status = state_no;
  read_setting = read_file_setting("Setting/setting.txt", 1);
  read_setting_state = read_file_setting("Setting/state.txt", 2);
  udp.begin(localPort);


  weekday = 8;
}


int hoc_ir(byte stt) {
  if (stt == 1) {
    if (irrecv.decode(&results, &save)) {
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
  switch (WiFi.status())
  {
    case WL_CONNECTED:
      // if (hoclenh == 1) {
      if (statusmang == 0) {
        statusmang = 1;
        _resetketnoi = 0;
        digitalWrite(status_led, HIGH);
      }
      else if (hoc_ir(hoclenh) == 1) {
        digitalWrite(status_led, LOW);
        _resetketnoi = 100;
        irrecv.resume();
      }
      else if (_resetketnoi == 2) {
        digitalWrite(status_led, HIGH);
        _resetketnoi = _resetketnoi - 1;
      }
      else if (_resetketnoi > 0) {
        _resetketnoi = _resetketnoi - 1;
      }
      nhan_TCP();
      if (demgiay % 10 == 0) {
        SetVariHC("NHAN", String(demgiay));
        demgiay++ ;
        float H = dht.readHumidity();
        float T = dht.readTemperature();
        if (isnan(H) || isnan(T)) {
          Serial.println(F("Failed to read from DHT sensor!"));
        }
        else {
          doam = H;
          nhietdo = T;
          Serial.print("H: ");
          Serial.print(doam);
          Serial.print(" %\t T: ");
          Serial.print(nhietdo);
          Serial.println(" *C ");
        }
      }
      else if (demgiay % 33 == 0) {
        send_udp();
        demgiay++;
      }
      else if (demgiay % 36 == 0) {
        demgiay = 1 ;
        gettime_udp();
        if (read_setting == 0) read_setting = read_file_setting("Setting/setting.txt", 1);
        if (read_setting_state == 0) read_setting_state = read_file_setting("Setting/state.txt", 2);
      }
      break ;
    default:
      if (statusmang != 0) {
        statusmang = 0;
        _resetketnoi = 0;
      }
      if  ((_resetketnoi % 5000) == 0)ketnoimang() ;
      if ( (unsigned long) (millis() - timeled) > 500 ) {
        if (denled == 0) denled = 1;
        else denled = 0;
        digitalWrite(status_led, denled );
        timeled = millis();
        Serial.println("DISCONNECT");
      }
      _resetketnoi = _resetketnoi + 1;
      if (_resetketnoi >= 65000) {
        Serial.println("A");
        digitalWrite(status_led, HIGH );
        //pinMode(status_led, INPUT);
        // delay(1000);
        // ESP.eraseConfig();
        ESP.reset();
      }
      break;
  }
  delay(2);
  if (millis() - timedelay > 1000 ) {
    demgiay++;
    timedelay = millis();
  }
  if (digitalRead(MotionPin) == 1) {
    if (_motion_status == 0) {

      if (thoigianthuc - motion_time > 5 ) {

        Serial.println(F("Status Motion : 1"));

        _motion_status = 1;
        motion_time = thoigianthuc;
        conver_time();
        Serial.println(time_);
        EEPROMWritelong(1000, motion_time);
      }
    }
  }
  else if (_motion_status == 1) {
    Serial.println(F("Status Motion : 0"));
    _motion_status = 0;
  }

  switch (weekday) {
    case 0 : if (is_sun) dung_ngay = true; else dung_ngay = false; break;
    case 1 : if (is_mon) dung_ngay = true; else dung_ngay = false; break;
    case 2 : if (is_tue) dung_ngay = true; else dung_ngay = false; break;
    case 3 : if (is_wed) dung_ngay = true; else dung_ngay = false; break;
    case 4 : if (is_thu) dung_ngay = true; else dung_ngay = false; break;
    case 5 : if (is_fri) dung_ngay = true; else dung_ngay = false; break;
    case 6 : if (is_sat) dung_ngay = true; else dung_ngay = false; break;
    default : dung_ngay = false; break;
  }
  //State machine for schedule
  /*
     Cách tính : lấy thời gian end trừ thời gian bắt đầu ra khoảng thời gian giữa 2 lần set
     biểu đồ sẽ chia cho 4 trong đó : 1 phần là tăng nhiệt độ khi ngủ, 2 phần duy trì và 1 phần  tăng nhiệt vào buổi sáng
     Trong phần tăng nhiệt độ thì sẽ chia làm 3 mốc : mốc begin cho nhiệt độ thấp nhất, mốc tiếp theo tăng 1 độ, mốc tiếp tăng 1 độ và mốc cuối cùng set đúng nhiệt độ cài đặt
     Phần duy trì nhiệt độ sẽ lấy mẫu sau 15 phút, nếu nhiệt độ thấp hơn cài đặt thì tăng 1 độ nếu nhiệt độ cao hơn cài đặt thì giảm 1 độ cho đến giai đoạn cuối
     Phần kết thúc : là khi trời sáng nhiệt độ phòng cần tăng lên hơn nhiệt độ cài đặt và sau đó đúng giờ thì tắt
  */
  switch (state_status) {
    case state_begin:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau / 4;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/18.txt";
        read_file_setting(file , 3 );
        state_status = state_update1;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update1:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau / 3;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/19.txt";
        read_file_setting(file , 3 );
        state_status = state_update2;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_update2:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau / 2;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/20.txt";
        read_file_setting(file , 3 );
        state_status = state_update3;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_update3:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(temp_set) + ".txt";
        read_file_setting(file , 3 );
        state_status = state_conti;
        write_file_setting("Setting/state.txt", 2);
      }
      //Tăng nhiệt độ cách 15 phút
      break;
    case state_conti:
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc + 900;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        if (float(temp_set) - nhietdo > 1) {
          String file = "ML/" + duongdan_ML + "/" + String(temp_set + 1) + ".txt";
          read_file_setting(file , 3 );
        }
        else {
          String file = "ML/" + duongdan_ML + "/" + String(temp_set - 1) + ".txt";
          read_file_setting(file , 3 );
        }
        if ((thoigianthuc - time_tam_cho_cac_buoc) > (khoang_time_cach_nhau * 6)) {
          state_status = state_update4;
          write_file_setting("Setting/state.txt", 2);
        }
      }
      //15 phút kiểm tra giu nhiet do đến nhỏ hơn thời gian kết thúc 1h thì chuyển trạng thái
      break;
    case state_update4:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau * 6;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(temp_set + 1) + ".txt";
        read_file_setting(file , 3 );
        state_status = state_update5;
        write_file_setting("Setting/state.txt", 2);
      }
      // tăng nhiệt độ lên cao khi trời sáng
      break;
    case state_update5:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau * 7;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/" + String(temp_set + 2) + ".txt";
        read_file_setting(file , 3 );
        state_status = state_wait;
        write_file_setting("Setting/state.txt", 2);
      }
      break;
    case state_wait:
      time_tam_cho_cac_buoc = time_begin_int + khoang_time_cach_nhau * 7;
      time_tam_cho_cac_buoc = time_tam_cho_cac_buoc % 86400L;
      if (thoigianthuc > time_tam_cho_cac_buoc) {
        String file = "ML/" + duongdan_ML + "/OFF.txt";
        read_file_setting(file , 3 );
        state_status = state_wait;
        write_file_setting("Setting/state.txt", 2);
      }
      // kết thúc điều khiển
      break;
    case state_no:
      if (!dung_ngay) {
        state_status = state_not_day;
        Serial.println("Không dung ngay dieu khien");
      }
      else if (thoigianthuc > time_begin_int) {
        //ON ML nhiệt độ thấp nhất
        if (time_begin_int > time_end_int) {
         
            khoang_time_cach_nhau = ( time_end_int + 86400) - time_begin_int ;
            khoang_time_cach_nhau = khoang_time_cach_nhau / 8 ;
            Serial.print("Khoảng thoi gian cach nhau ");
            Serial.println(khoang_time_cach_nhau);
            //ON ML
            String file = "ML/" + duongdan_ML + "/ON.txt" ;
            read_file_setting(file , 3 );
            state_status = state_begin;
          
        }
        else if (thoigianthuc < time_end_int) {
          khoang_time_cach_nhau = time_end_int - time_begin_int ;
          khoang_time_cach_nhau = khoang_time_cach_nhau / 8 ;
          Serial.print("Khoảng thoi gian cach nhau ");
          Serial.println(khoang_time_cach_nhau);
          //ON ML
          String file = "ML/" + duongdan_ML + "/ON.txt" ;
          read_file_setting(file , 3 );
          state_status = state_begin;
        }
      }
      break;
    case state_not_day:
      if (dung_ngay) {
        state_status = state_no;
        Serial.println("dung ngay dk");
      }
      // kết thúc điều khiển
      break;
  }
}

