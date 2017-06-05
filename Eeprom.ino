int waitConnected(void) {
  byte wait = 0;
    while (wait<10 ) {
    digitalWrite(status_led, LOW);
    if (WiFi.status() == WL_CONNECTED) {
      //Serial.println("");
      Serial.println(F("WiFi connected"));
      return (1);
    }
    wait++;
    delay(300);
    digitalWrite(status_led, HIGH);
    delay(300);
   // Serial.println(wait);
    }
  return(0);
}
/*int waitConnected1(void) {
  int i=0;
  while (true ) {
    digitalWrite(status_led, LOW);
    if (WiFi.status() == WL_CONNECTED) {
      return (1);
    }
    delay(400);
    digitalWrite(status_led, HIGH);
    delay(400);
    i++;
    if (i>70){ESP.restart();return (0);}
  }
  return (0);
}*/
void printIP(void) {
  Serial.println(WiFiConf.module_id);
}
void ketnoimang() {
  WiFi.begin(WiFiConf.sta_ssid, WiFiConf.sta_pwd);
  parseBytes(WiFiConf.sta_ip, '.', ip1, 4, 10);
  parseBytes(WiFiConf.sta_gateway, '.', gateway, 4, 10);
  parseBytes(WiFiConf.sta_subnet, '.', subnet, 4, 10);
  WiFi.config(ip1, gateway, subnet);
}
void printWiFiConf(void) {
  Serial.println(WiFiConf.sta_ssid);
 // Serial.println(WiFiConf.sta_pwd);
//  Serial.println(WiFiConf.sta_ip);
 // Serial.println(WiFiConf.sta_gateway);

}
bool loadWiFiConf() {
  Serial.println(F("loading WiFiConf"));
  if (EEPROM.read(WIFI_CONF_START + 0) == wifi_conf_format[0] &&
      EEPROM.read(WIFI_CONF_START + 1) == wifi_conf_format[1] &&
      EEPROM.read(WIFI_CONF_START + 2) == wifi_conf_format[2] &&
      EEPROM.read(WIFI_CONF_START + 3) == wifi_conf_format[3])
  {
    for (unsigned int t = 0; t < sizeof(WiFiConf); t++) {
      *((char*)&WiFiConf + t) = EEPROM.read(WIFI_CONF_START + t);
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
  
  uint8_t macAddr[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(macAddr);
  sprintf(dst, "%s%02x%02x", NAME_PREF, macAddr[WL_MAC_ADDR_LENGTH - 2], macAddr[WL_MAC_ADDR_LENGTH - 1]);
}
void resetModuleId(void) {
  uint8_t macAddr[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(macAddr);
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
    Serial.println(WiFi.SSID(i));
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

