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
  hour = (motion_time  % 86400L) / 3600 ;
  hour = hour + 7 ;
  hour = hour % 24;
  time_ = hour;
  time_ += " : ";
  if ( ((motion_time % 3600) / 60) < 10 ) time_ += "0";
  minute = (motion_time  % 3600) / 60 ;
  time_ += minute;
  Serial.println(time_);
  scanWiFi();
  Serial.println("A");
  hoclenh = 0;
  WiFi.mode(WIFI_AP_STA);
 // Serial.println("B");
 // WiFi.setAutoReconnect ( true );
  //Serial.println("C");
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
    delay(500);
    WiFi.mode(WIFI_AP);
    Serial.println(WiFiConf.module_id);
    WiFi.softAP(WiFiConf.module_id, wifi_password);

  }

  printIP();
  httpUpdater.setup(&server, update_path, update_username, update_password);
  Serial.println(WiFi.localIP());
  setupWeb();
  setupWiFiConf();
  server.begin();
  MDNS.begin(WiFiConf.module_id);
  udp.begin(localPort);
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
        SetVariHC("NHAN",String(demgiay));
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
      }

      break ;
    default:
      if (statusmang != 0) {
        statusmang = 0;
        _resetketnoi = 0;
      }
      if  ((_resetketnoi % 5000) == 0)ketnoimang();
      if ( (unsigned long) (millis() - timeled) > 500 ) {
        if (denled == 0) denled = 1;
        else denled = 0;
        digitalWrite(status_led, denled );
        timeled = millis();
        Serial.println("DISCONNECT");
      }
      _resetketnoi = _resetketnoi + 1;
      if (_resetketnoi >= 20000) {
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
        hour = (motion_time  % 86400L) / 3600 ;
        hour = hour + 7 ;
        hour = hour % 24;
        time_ = hour;
        time_ += " : ";
        if ( ((motion_time % 3600) / 60) < 10 ) {
          time_ += "0";
        }
        minute = (motion_time  % 3600) / 60 ;
        time_ += minute;
        Serial.println(time_);
        EEPROMWritelong(1000, motion_time);
      }
    }
  }
  else if (_motion_status == 1) {
    Serial.println(F("Status Motion : 0"));
    _motion_status = 0;
  }
  
//String PostData = "{\r\n\"name\": \"\",\r\n\"value\":\"\",\r\n\"invokeScenes\":True\r\n}";
//Serial.println(PostData.length());
}

