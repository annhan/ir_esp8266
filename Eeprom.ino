int waitConnected(void) {
  byte wait = 0;
    while (wait<20 ) {
    digitalWrite(status_led, LOW);
    if (WiFi.status() == WL_CONNECTED) {
      //DEBUG_PRINTLN("");
      DEBUG_PRINTLN("WiFi connected");
      return (1);
    }
    wait++;
    delay(300);
    digitalWrite(status_led, HIGH);
    delay(300);
   // DEBUG_PRINTLN(wait);
    }
  return(0);
}
void printIP(void) {
  DEBUG_PRINTLN(WiFiConf.module_id);
  DEBUG_PRINTLN(WiFiConf.sta_ip);
  DEBUG_PRINTLN(WiFi.localIP());
  
}
void ketnoimang() {
  
  WiFi.hostname("mIR");
  boolean kq=scanWiFireturn();
  if (kq){
      WiFi.begin(WiFiConf.sta_ssid, WiFiConf.sta_pwd);
      if (atoi(WiFiConf.sta_DHCP) == 1){
          parseBytes1(WiFiConf.sta_ip, '.', 1, 4, 10);
          parseBytes1(WiFiConf.sta_gateway, '.', 2, 4, 10);
          parseBytes1(WiFiConf.sta_subnet, '.', 3, 4, 10);
          WiFi.config(ip10,gateway10,subnet10,DNS);
      }
  }
}
void printWiFiConf(void) {
  //DEBUG_PRINTLN(WiFiConf.sta_ssid);
}
bool loadWiFiConf() {
  //DEBUG_PRINTLN(F("loading WiFiConf"));
  if (EEPROM.read(WIFI_CONF_START + 0) == wifi_conf_format[0] &&
      EEPROM.read(WIFI_CONF_START + 1) == wifi_conf_format[1] &&
      EEPROM.read(WIFI_CONF_START + 2) == wifi_conf_format[2] &&
      EEPROM.read(WIFI_CONF_START + 3) == wifi_conf_format[3])
  {
    for (unsigned int t = 0; t < sizeof(WiFiConf); t++) {
      *((char*)&WiFiConf + t) = EEPROM.read(WIFI_CONF_START + t); //& là địa chỉ  của biến Struc, *là data tức là gán data trong ô nhớ struc bằng eprom đọc dc (char*) là ép kiểu dữ liệu
    }
    printWiFiConf();
    return true;
  } else {
    return false;
  }
}
void saveWiFiConf(void) {
  for (unsigned int t = 0; t < sizeof(WiFiConf); t++) {
    EEPROM.write(WIFI_CONF_START + t, *((char*)&WiFiConf + t));
  }
  EEPROM.commit();
  printWiFiConf();
}
void setDefaultModuleId(char* dst) {
  sprintf(dst, "%s%02x%02x", NAME_PREF, macAddr[WL_MAC_ADDR_LENGTH - 2], macAddr[WL_MAC_ADDR_LENGTH - 1]);
}
void resetModuleId(void) {;
  setDefaultModuleId(WiFiConf.module_id);
}
void scanWiFi(void) {
  int founds = WiFi.scanNetworks();
  network_html = F("<ol>");
  network_html += FPSTR(_p_html);
  network_html +=F("<fieldset>");
  network_html +=F("<legend>Available Networks</legend>");
 // network_html = "Available Networks";
  for (int i = 0; i < founds; ++i)
  {
    // Print SSID and RSSI for each network found
    //DEBUG_PRINTLN(WiFi.SSID(i));
    network_html += F("<li>");
    network_html += WiFi.SSID(i);
    network_html += F(" (");
    network_html += WiFi.RSSI(i);
    network_html += F(")");
    network_html += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    network_html += F("</li>");
    
  }
  network_html +=F("</fieldset>");
  network_html += F("</ol>");
}
boolean scanWiFireturn(void) {
  int founds = WiFi.scanNetworks();
  char ten_wifi[32];
  for (int i = 0; i < founds; ++i)
  {  
    WiFi.SSID(i).toCharArray(ten_wifi, sizeof(ten_wifi));
   if (strstr(ten_wifi,WiFiConf.sta_ssid) != NULL){DEBUG_PRINTLN("Co wifi cung ten");return true;}   
    
  }
  return false;
}


void EEPROMWritelong(int address, unsigned long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);
      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
       EEPROM.commit();
      }

 unsigned long EEPROMReadlong(int address)
      {
      //Read the 4 bytes from the eeprom memory.
      unsigned long four = EEPROM.read(address);
      unsigned long three = EEPROM.read(address + 1);
      unsigned long two = EEPROM.read(address + 2);
      unsigned long one = EEPROM.read(address + 3);
      
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFF00) + ((two << 16) & 0xFF0000) + ((one << 24) & 0xFF000000);
      }     

void user_using(){
  DEBUG_PRINTLN(WiFiConf.sta_ML);
  if (String(WiFiConf.sta_ML) == "0") duongdan_ML="User";
  else if (String(WiFiConf.sta_ML) == "1") duongdan_ML="Carrier";
  else if (String(WiFiConf.sta_ML) == "2") duongdan_ML="Daikin";
  else if (String(WiFiConf.sta_ML) =="3") duongdan_ML="Electrolux";
  else if (String(WiFiConf.sta_ML)=="4") duongdan_ML="Hitachi";
  else if (String(WiFiConf.sta_ML) =="5") duongdan_ML="LG";
  else if (String(WiFiConf.sta_ML)=="6") duongdan_ML="Misu";
  else if (String(WiFiConf.sta_ML)=="7") duongdan_ML="Panasonic";
  else if (String(WiFiConf.sta_ML)=="8") duongdan_ML="Reetech";
  else if (String(WiFiConf.sta_ML)=="9") duongdan_ML="Samsung";
  else if (String(WiFiConf.sta_ML)=="10") duongdan_ML="Sanyo";
  else if (String(WiFiConf.sta_ML)=="11") duongdan_ML="Sharp";
  else if (String(WiFiConf.sta_ML)=="12") duongdan_ML="Toshiba";

  if (String(WiFiConf.sta_TV)=="0") duongdan_TV="User";
  else if (String(WiFiConf.sta_TV)=="1") duongdan_TV="LG";
  else if (String(WiFiConf.sta_TV)=="2") duongdan_TV="Samsung";
  else if (String(WiFiConf.sta_TV)=="3") duongdan_TV="Sony";
  else if (String(WiFiConf.sta_TV)=="4") duongdan_TV="Toshiba";
  
}

