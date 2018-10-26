// RAWSTICK la 50

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
#include <EmonLiteESP.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <ir_Mitsubishi.h>
#include <WiFiUdp.h>
//#include <SPI.h>
//#include <SD.h>
// Port 1883
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.print (x)
#else
  #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

#define CURRENT_PIN             A0

// If you are using a nude ESP8266 board it will be 1.0V, if using a NodeMCU there
// is a voltage divider in place, so use 3.3V instead.
#define REFERENCE_VOLTAGE       1.0

// Precision of the ADC measure in bits. Arduinos and ESP8266 use 10bits ADCs, but the
// ADS1115 is a 16bits ADC
#define ADC_BITS                10

// This is basically the volts per amper ratio of your current measurement sensor.
// If your sensor has a voltage output it will be written in the sensor enclosure,
// something like "30V 1A", otherwise it will depend on the burden resistor you are
// using.
#define CURRENT_RATIO           17

// This version of the library only calculate aparent power, so it asumes a fixes
// mains voltage
#define MAINS_VOLTAGE           230

// Number of samples each time you measure
#define SAMPLES_X_MEASUREMENT   1500

// Time between readings, this is not specific of the library but on this sketch
#define MEASUREMENT_INTERVAL    10000

EmonLiteESP power;
unsigned int currentCallback() {

    // If usingthe ADC GPIO in the ESP8266
    return analogRead(CURRENT_PIN);

}

void powerMonitorSetup() {
    power.initCurrent(currentCallback, ADC_BITS, REFERENCE_VOLTAGE, CURRENT_RATIO);
}
void powerMonitorLoop() {

    static unsigned long last_check = 0;

    if ((millis() - last_check) > MEASUREMENT_INTERVAL) {
        Serial.print(F("B-"));
        double currentt = power.getCurrent(SAMPLES_X_MEASUREMENT);
        current=int(currentt * MAINS_VOLTAGE);
        Serial.print(current);
        Serial.print(F("-"));
        if (current>WiFiConf.offset) {
          current=current-WiFiConf.offset;
        }
        else current=0;
        Serial.print(current);
        Serial.println(F("W"));
        last_check = millis();

    }
}


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
//#############################################
uint16_t CAPTURE_BUFFER_SIZE = 1400;
#define TIMEOUT 100U  // Suits most messages, while not swallowing repeats.
IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT,true);
IRsend irsend(Send_PIN);
IRDaikinESP dakinir(Send_PIN);
IRMitsubishiAC mitsubir(Send_PIN);

/////////////////////////////////////////////////////////////////////////

ESP8266WebServer server(4999);
WiFiServer serverTCP(4998);
decode_results  results;

void blink_led(int time_tt){
  digitalWrite(status_led,1 );
  delay(time_tt);
  digitalWrite(status_led, 0);
  delay(time_tt);
  digitalWrite(status_led, 1 );
    delay(time_tt);
  digitalWrite(status_led, 0 );
}
void setup() {
  //ESP.eraseConfig();
  pinMode(config_pin, INPUT);
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, LOW);
  Serial.begin(115200);
  EEPROM.begin(1024);
  if (digitalRead(config_pin)==0){
    delay(1500);
    if (digitalRead(config_pin)==0){
      static_config=1;
      blink_led(200);
    }
  }
  
  delay(10);
  DEBUG_PRINTLN("Startup");
  irsend.begin();
  if (!loadWiFiConf()){
    resetModuleId();
    saveWiFiConf();
  }
 // motion_time = EEPROMReadlong(1000);
  conver_time();
  scanWiFi();
  hoclenh = 0;
  pinMode(A0, INPUT);
  //emon1.current(A0, 9.1);
  if (static_config == 1){
      WiFi.setAutoConnect(false);
      WiFi.setAutoReconnect(false);
      WiFi.mode(WIFI_AP);
      WiFi.softAP("mIR-Config", wifi_password);
  }
  else {
      WiFi.setAutoConnect(true);
      WiFi.setAutoReconnect(true);
      WiFi.mode(WIFI_STA);
      //WiFi.softAP(WiFiConf.module_id, wifi_password);
      ketnoimang();
      statusmang = waitConnected();
      if (WiFi.status() == WL_CONNECTED) {
        update_fota();
        getHC();
        DEBUG_PRINTLN("Connect");
      }
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
  WiFi.macAddress(macAddr);
  MDNS.addService("http", "tcp", 4999);
  udp.begin(localPort);
  weekday = 8;
  powerMonitorSetup();
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
  wdt_reset(); 
  if (atoi(WiFiConf.sta_DHCP) == 1){ powerMonitorLoop(); }
  server.handleClient();
  //##################
  // MQTT ############
  if (static_config == 0){
      switch (WiFi.status())
      {
        case WL_CONNECTED:
          // if (hoclenh == 1) {
          
          if (statusmang == 0) {
           // WiFi.softAPdisconnect(true);
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
            //
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
           // WiFi.softAPdisconnect(false);
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
  }
  delay(2);
  if (millis() - timedelay > 1000 ) {
    demgiay++;
    timedelay = millis();
  }

}

