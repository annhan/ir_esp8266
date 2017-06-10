unsigned long sendNTPpacket(IPAddress & address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void send_udp(){
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
 // Serial.print("IP : ");
  //Serial.println(timeServerIP);
}
byte gettime_udp()
{
  int cb = udp.parsePacket();
  if (!cb) {
        Serial.println(F("no packet yet"));
        return 0;
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = 0;
    secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    if  ((secsSince1900 > 2400000000) && (secsSince1900 < 6000000000)){
    thoigianthuc = secsSince1900 - seventyYears;
    unsigned long day = thoigianthuc / 86400L;
    weekday = (day+4) % 7;
    thoigianthuc=thoigianthuc+ 25200L ; //Cộng với 7h là 7*3600 cho đúng múi giờ +7
    thoigianthuc= thoigianthuc  % 86400L ;
    Serial.println(thoigianthuc);
    }
    return 1 ;
  }  
}


void conver_time(){
  hour = (motion_time  % 86400L) / 3600 ;
  //hour = hour + 7 ;
  hour = hour % 24;
  time_ = hour;
  time_ += " : ";
  if ( ((motion_time % 3600) / 60) < 10 ) time_ += "0";
  minute = (motion_time  % 3600) / 60 ;
  time_ += minute;
}


String conver_day(int day){
switch (day){
  case 0: return "Sunday"; break;
  case 1: return "Monday"; break;
  case 2: return "Tuesday"; break;
  case 3: return "Wednesday"; break;
  case 4: return "Thursday"; break;
  case 5: return "Friday"; break;
  case 6: return "Saturday"; break;
  default : return "Not" ; break;
}
}


unsigned long conver_time_string_to_int(String timeget){
   int hh, mm ;
    char time_begin_char[10];
    timeget.toCharArray(time_begin_char, sizeof(time_begin_char));
    sscanf(time_begin_char, "%d:%d", &hh, &mm);
    unsigned long showSecs = (hh * 3600) + (mm * 60);
    return showSecs;
}

String conver_time_int_to_string(unsigned long timeget){
    String trave="";
    int gio=timeget / 3600;
    if ( gio < 9)trave += "0" ;
    trave +=String(timeget / 3600) ;
    trave += ":";
    int phut = (timeget  % 3600) / 60 ;
    if (phut <9) trave += "0" ;
    trave += String((timeget  % 3600) / 60 );
    Serial.println(trave);
    return(trave);
}

