
String urlDecodeir(String param) {
  param.replace("+"," ");
  param.replace("%2C",",");
//param.replace("%21","!");
//param.replace("%23","#");
//param.replace("%24","$");
//param.replace("%26","&");
//param.replace("%27","'");
//param.replace("%28","(");
//param.replace("%29",")");
//param.replace("%2A","*");
//param.replace("%2B","+");
//param.replace("%2F","/");
//param.replace("%3A",":");
//param.replace("%3B",";");
//param.replace("%3D","=");
//param.replace("%3F","?");
//param.replace("%40","@");
//param.replace("%5B","[");
//param.replace("%5D","]");

return param;
}
void parseStringRAW(String str) {
  int16_t index;
  uint16_t count;

  // Find out how many items there are in the string.
  index = -1;
  count = 1;
  do {
    index = str.indexOf(',', index + 1);
    count++;
  } while (index != -1);

  // Now we know how many there are, allocate the memory to store them all.
  code_array = reinterpret_cast<uint16_t*>(malloc(count * sizeof(uint16_t)));
  // Check we malloc'ed successfully.
  if (code_array == NULL) {  // malloc failed, so give up.
    Serial.printf("\nCan't allocate %d bytes. (%d bytes free)\n",
                  count * sizeof(uint16_t), ESP.getFreeHeap());
    Serial.println("Giving up & forcing a reboot.");
    ESP.restart();  // Reboot.
    delay(500);  // Wait for the restart to happen.
    return;  // Should never get here, but just in case.
  }

  // Now convert the strings to integers and place them in code_array.
  count = 0;
  uint16_t start_from = 0;
  do {
    index = str.indexOf(',', start_from);
    code_array[count] = str.substring(start_from, index).toInt();
    start_from = index + 1;
    count++;
  } while (index != -1);
  for (int i=0;i<count-1;i++){
    if (code_array[i]<1310){
    code_array[i]=code_array[i]* USECPERTICK;  
    }
    else code_array[i]=65534;
   // Serial.print(code_array[i]);
    //Serial.print(",");
  }//
 // Serial.println("");
  irsend.sendRaw(code_array, count - 1, code_array[count - 1] / (1000));                                                
  free(code_array);  // Free up the memory allocated.

}
void parseStringGC(String str) {
  int16_t index;
  uint16_t count;

  // Find out how many items there are in the string.
  index = -1;
  count = 1;
  do {
    index = str.indexOf(',', index + 1);
    count++;
  } while (index != -1);

  // Now we know how many there are, allocate the memory to store them all.
  code_array = reinterpret_cast<uint16_t*>(malloc(count * sizeof(uint16_t)));
  // Check we malloc'ed successfully.
  if (code_array == NULL) {  // malloc failed, so give up.
   // Serial.printf("\nCan't allocate %d bytes. (%d bytes free)\n",count * sizeof(uint16_t), ESP.getFreeHeap());
   // Serial.println("Giving up & forcing a reboot.");
    ESP.restart();  // Reboot.
    delay(500);  // Wait for the restart to happen.
    return;  // Should never get here, but just in case.
  }

  // Now convert the strings to integers and place them in code_array.
  count = 0;
  uint16_t start_from = 0;
  do {
    index = str.indexOf(',', start_from);
    code_array[count] = str.substring(start_from, index).toInt();
    start_from = index + 1;
    count++;
  } while (index != -1);

  irsend.sendGC(code_array, count);  // All done. Send it.
  free(code_array);  // Free up the memory allocated.
  /*
        long nextIndex;
        long codeLength = 1;
        long currentIndex = 0;
        nextIndex = str.indexOf(',');
        for (int i = 1;  i < chieudai_ir;  i++) {irSignal[i - 1] = 0x00;}
        while (nextIndex != -1) {
          irSignal[codeLength - 1] = (unsigned int) (str.substring(currentIndex, nextIndex).toInt());
          codeLength++;
          currentIndex = nextIndex + 1;
          nextIndex = str.indexOf(',', currentIndex);
        }
        irSignal[codeLength - 1] = (unsigned int) (str.substring(currentIndex, nextIndex).toInt());
        irsend.sendGC(irSignal, codeLength);*/
}

void setupWiFiConf(void) {
  
  server.on(html_setup_wifi, []() {
    //IPAddress ip = WiFi.localIP();
    //String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    String content = FPSTR(header);content += FPSTR(begin_title);
     String   content1 = FPSTR(p_html);
     content += F("Connected Wifi: ");
    content1 += WiFiConf.sta_ssid;
    content1 += F("</br>IP Address: ");
    content1 += ipStr;
    content1 += F(" ( ");
    content1 += WiFiConf.module_id;
    content1 += F(".local )");
    content1 += FPSTR(_p_html);
    content1 += FPSTR(p_html);
     content1 += FPSTR(_p_html);
     content1 += FPSTR(get_html);
     content1 +=F("'set_wifi_conf'>");
    content1 += FPSTR(label_html);
    content1 += F("'ssid'>SSID  </label>");
    content1 += F("<input name='ssid'id='ssid' maxlength=32 value=") ;
    content1 += String(WiFiConf.sta_ssid) ;
    content1 +=  FPSTR(br_html);

    content1 += FPSTR(label_html);
    content1 += F("'pwd'>PASS  </label>");
    content1 += F("<input type='password' name='pwd' id='pwd' value=");
    content1 += String(WiFiConf.sta_pwd);
    content1 += FPSTR(br_html);
    content1 += FPSTR(label_html);
    content1 += F("'ip'>IP  </label>");
    content1 += F("<input name='ip' id='ip'value=");
    content1 += String(WiFiConf.sta_ip) ;
    content1 +=  FPSTR(br_html);
    
    content1 += FPSTR(label_html);
    content1 += F("'gateway' >GATEWAY  </label>");
    content1 += F("<input  name='gateway' id='gateway' value=") ;
    content1 += String(WiFiConf.sta_gateway) ;
    content1 += FPSTR(br_html);

    content1 += FPSTR(label_html);
    content1 += F("'subnet' >SUBNET  </label>");
    content1 += F("<input  name='subnet' id='subnet' value=");
    content1 +=  String(WiFiConf.sta_subnet);
    content1 += FPSTR(br_html);
    content += FPSTR(wifisetting_html);
    content += FPSTR(title_html);
    content += F("<h1>Wifi Setup</h1>");
          content += content1;
         content += F("<input type='submit' value='OK' onclick='return confirm(\"Change Settings?\");'></form>");

    content += FPSTR(_p_html);
    content += network_html;
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
server.on(html_setup_hc2, []() {
   // IPAddress ip = WiFi.localIP();
    String content = FPSTR(header);content += FPSTR(begin_title);
         String    content1 = ipStr;
    content1 += F(" ( ");
    content1 += WiFiConf.module_id;
    content1 += F(".local )");
    //content1 += FPSTR(_p_html);
    content1 += FPSTR(p_html);
    ///content1 += FPSTR(_p_html);
    content1 += FPSTR(get_html);
    content1 += F("'set_hc2_conf'>");
    
    content1 += FPSTR(label_html);
    content1 += F("'iphc2'>IP HC2:");

    content1 += F("</label><input name='iphc2'id='iphc2' maxlength=32 value=");
    content1 += String(WiFiConf.sta_iphc2);
    content1 += FPSTR(br_html);

    content1 +=FPSTR(label_html);
    content1 += F("'pwdhc2'>PASS HC2: </label> <input type='password' name='pwdhc2' id='pwdhc2' value=");
    content1 += String(WiFiConf.sta_passhc) ;
    content1 += FPSTR(br_html);

    content1 += FPSTR(label_html);
    content1 += F("'global1'>Global 1:");

    content1 += F("</label> <input name='global1' id='global1'value=");
    content1 += String(WiFiConf.sta_global1);
    content1 += FPSTR(br_html);

    content1 += FPSTR(label_html);
    content1 += F("'global2'>Global 2:&nbsp;&nbsp;&nbsp;&nbsp;</label> <input  name='global2' id='global2' value=");
    content1 +=  String(WiFiConf.sta_global2);
    content1 += FPSTR(br_html);
    
    content1 += FPSTR(label_html);
    content1 += F("'global3'>Global 3:&nbsp;&nbsp;&nbsp;&nbsp;</label> <input  name='global3' id='global3' value=");   
    content1 += String(WiFiConf.sta_global3) ;    
    content1 += FPSTR(br_html);  
    content += FPSTR(wifisetting_html);
    content += FPSTR(title_html);
    content += F("<h1>HC2 Setting</h1>");
    content += FPSTR(p_html);
    content += F("Wifi conecting : ");
    content += WiFiConf.sta_ssid;
    content += F("</br>IP address: ");  
    content += content1;
    content += F("<input type='submit' value='OK' onclick='return confirm(\"Change Setting ?\");'></form>");
    content += FPSTR(_p_html);
    content += FPSTR(get_html);
    content += F("'getHC'>");
    content += F("<input type='submit' value='Check'></form>");
    content += FPSTR(_p_html);
    content += F("<li>Information HC2");
    content += F("<li>Format User:password#  Ex: admin:admin123#");
    //}
     content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
server.on("/getHC", []() {
    String content = FPSTR(header);content += FPSTR(begin_title);
       content += F("Wait");
    getHC();
    server.send(200, F("text/html"), content);
  });
server.on(html_setup_SETHC2, []() {
    String new_IPHC = server.arg(F("iphc2"));
    String new_pwdhc = server.arg(F("pwdhc2"));
    String new_global1 = server.arg(F("global1"));
    String new_global2 = server.arg(F("global2"));
    String new_global3 = server.arg(F("global3"));
    String content = FPSTR(header);content += FPSTR(begin_title);
      content += FPSTR(wifisetting_html);
    content += FPSTR(title_html);
    content += F("<h1>Luu Wifi</h1>");
    if (new_IPHC.length() > 0) {
      new_IPHC.toCharArray(WiFiConf.sta_iphc2, sizeof(WiFiConf.sta_iphc2));
      new_pwdhc.toCharArray(WiFiConf.sta_passhc, sizeof(WiFiConf.sta_passhc));
      new_global1.toCharArray(WiFiConf.sta_global1, sizeof(WiFiConf.sta_global1));
      new_global2.toCharArray(WiFiConf.sta_global2, sizeof(WiFiConf.sta_global2));
      new_global3.toCharArray(WiFiConf.sta_global3, sizeof(WiFiConf.sta_global3));
      saveWiFiConf();
      content += FPSTR(p_html);
      content += F("Save '");
      content += WiFiConf.sta_ssid;
      content += F("' ...  device Rebooting !");
      content += FPSTR(_p_html);
      
    } else {
      content += FPSTR(p_html);
      content += F("Rejected empty SSID.");
      content += FPSTR(_p_html);
    }
    content += F("<body></html>");
    server.send(200, F("text/html"), content);
  });

  server.on(html_setup_switch, []() {
    if (hoclenh==0){hoclenh=1;irrecv.enableIRIn();server.send(200, F("text/html"), F("{\"enable\":\"1\"} <p><a href=\"/\"><button>BACK</button></a></p>"));}
    else {hoclenh=0;irrecv.disableIRIn();server.send(200, F("text/html"), F("{\"enable\":\"0\"} <p><a href=\"/\"><button>BACK</button></a></p>"));}
   Serial.println(hoclenh);
   
  });
  server.on(html_setup_leaningir, []() {

    String content =  F("<!DOCTYPE HTML>\r\n<html><head>");
    content = FPSTR(header);content += FPSTR(begin_title);    
    //content =  F("<meta http-equiv='refresh' content='5'><style>body {background-color:lightgrey}h1 {color:blue}p {color:black}</style><link rel=\"shortcut icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAEAAAAAAAAAAAAAAAEAAAAAAADLy8sAHYsbACCNGADK28UADIEHAHOubwAShQoA//7/AObz6ADn8+gA6fPoAOzz6ADGxsYAXa5eAL7gugAghxAAtbyyABFmCAD09vEAN5ctADuPMAA4ly0AxuG9AKvXqQA2lzYAJ4cZADuXNgAikCIAKIwoAHS1cwDs++YAAHgAAKrKpwAAewAAj8WQACCHEQDa6tgAtbyzAMTcwQA4jzEAOpEuAFCkSwAYZgkAa7JiAIK9ggBAmDoAQJY9AA6FDADQ5NAApMalAC2PKQCozagA9PXwAPT48AAvbScAF2YKAGywZgBtsGYA4+3iAMviywA/mDsARpgyAM/jzgBkkGEA0eXLAC6KJAA2dDMAotCgAEWbRAA5iiQAv9+6AKmr3AAQZQgA8vXxAPn48QA+mDMAsdayAJ7DngChxKEANnQ0ACiRKABlkGIAq7iqAAB3AAAAegAAjsSQAHm5eQDKycoAw9vBAGGxYgB/mqMAhJesAPr6+AC21qoA/v/+AJqamgBygMIASpxAAMjIyADv8PYAYLBgANrr1gCUx5EAZ61gAGitYADQy9EAKGpOACOPJAD7+/wAJI8kAPv9+QDo8OUA/v//AI24iQD///8AWaxVAC+MJABerFUAXaleAF6pXgB5tHUA2uvXAPX16wDPys8AxOLGANHL0gAnhB8Agr1+AJqh1AAOfAgAzOHMAIe+gQCLvoEA6O7pAHK0bQCrt6oAMI4iAC6QKAAtlCsAS5s/AF+rXAB4t3YAwdrBAMrIygDv+e8A3O3eAM/K0ADf4uoAdJdxAHWXcQAniBoAbqtuAP3+9QAMhgwAoc6fAHO0bgAvjiMAFYYMAI28jgBGn0MAyMfIAGOrXQB4uHoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAyPe4c/NhEqT5QlaaAAX3JyXiITc5oWDmYpS11ycgByXIw8ZXJycnJycnxFg3JiclZESXJycnJycnJyOkEXfSScCXIvG1BtbW1rnYVAPRB4m3JyGgRxMU1OnoFycimVLlhycllTjXJycpAccnJmQpYLcnJkHx8zcnIeAnJyRjcPenJyDVNUBnJjgGBHchZIFDAHIAEoHWxyk1tqWnJDNjmDcm+KdCxycm4FK5hydVE7MhJyl34hGAeff3JyeRVScndoB3JyjpkfTHJyNC1VknIKYWcScnKRA3JyCIuhcFdycgl2iYQ+SjUmhoiiXHIMcnJycoI4JyMZLngkcnJyDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\"/><meta charset='UTF-8'><title>");
    content += F("mHome - IR Learn");
    content += FPSTR(title_html);
    content += F("<h1>IR Learn</h1>");
    // content += "<p>LAN: ";
    String hienthi;
    for (int k = 0; k < chieudai; k++) {
      hienthi += String(irSignal[k]);
      if (k < chieudai - 1) {
        hienthi += ",";
      }
    }
    
    if (statusmang==0) {
      content +=FPSTR(notwifi_html);
      content +=F("<a href='/wifi_conf'>Wifi Setting</a>");
    }
    else{
      //content += FPSTR(_p_html);
      content += F("<form method='post' action='codeIR'>");
    
            content += FPSTR(label_html);
            content += F("'raw'>IR CODE");

    content += F(": </label><input name='raw'id='raw'style=\"width:70%;\" value=");
            content +=hienthi; //maxlength=32
            content += FPSTR(br_html); //maxlength=32
            content += FPSTR(label_html);
            content += F("'ts'>Frequency");

    content += F(": </label> <input name='ts' id='ts'  value=38000");
            content += FPSTR(br_html);
            
            content += F("<input type='submit' value='Check new  Code' onclick='return confirm(\"Are you ready?\");'></form>");
            content += FPSTR(_p_html);
            //content += FPSTR(_p_html);
            content += F("Attention :");
            content += FPSTR(_p_html);
            content += F("<li>The default frequency is 38Khz = 38000. Change the frequency to 30000,33000,36000,38000,40000,56000 if devices not work");
            content += F("<li>When completed the new code, you have to refresh the page for the latest code update.");
            content += FPSTR(p_html);
            content += F("Thank you for reading.");
          //break;     
    //}
    }
    content += FPSTR(end_html);
    server.send(200,F("text/html"), content);
  });
  ///////////////////////
  /*
   * POST DATA IR
   */
  server.on(html_setup_codeIR, HTTP_POST, []() {
    if (hoclenh == 1) { irrecv.disableIRIn(); }
    String data1=server.arg("raw");
    String ts1=server.arg("ts");
    json_ts=ts1;
    if (data1.length()>=1264){data1=urlDecodeir(data1);}
   String content = "";
    if (data1.length() < 12) {
      content += F("Not OK");
      Serial.println(" ");
    }
    else {
          data1+=",";
          data1+=ts1;
          parseStringRAW(data1);
          content += F("OK");
    }
    server.send(200, F("text/html"), content);
    if (hoclenh == 1) {irrecv.enableIRIn();}
  });
  server.on(html_setup_codeGC, []() {
          if (hoclenh == 1) {
                      irrecv.disableIRIn();
                    //  delay(500);    
                    }
    String data1 = server.arg("raw");
     parseStringGC(data1);
    String content = FPSTR(header);content += FPSTR(begin_title);
     content += WiFiConf.module_id;
    content += F(".local - set WiFi");
    content += FPSTR(title_html);
    content += F("<h1>OK</h1>");
    server.send(200, "text/html", content);
          if (hoclenh == 1) { //delay(200);    
                            irrecv.enableIRIn();  // Start the receiver
                          }
  });
   server.on(html_setup_getstatus, []() { 
    server.send(200,F("text/html"), "OK");
  });

  /*
   * Get code IR 
   */
   server.on(html_setup_getcode, []() {
    String hienthi;
    for (int k = 0; k < chieudai; k++) {
      hienthi += String(irSignal[k]);
      if (k < chieudai - 1)  hienthi += ",";
    }
    String json="{";
    json += "\"code\":\"" + hienthi ;
    json += "\",\"f\":" + json_ts;
    if (hoclenh==0) json += "\",\"enable\": 1";
    else json += "\",\"enable\": 0";
    json += "}";
    if (hienthi.length()>1 ){
      server.send(200,F("text/html"), json);
    }
    else server.send(200,F("text/html"), F("{\"code\":\"error\",\"f\":\"38000\"}"));
   // ESP.restart();
  });
  /*
   * Send Daikin
   */
  server.on(html_setup_senddaikin, []() {
    /*#define DAIKIN_COOL                0b011
#define DAIKIN_HEAT                0b100
#define DAIKIN_FAN                 0b110
#define DAIKIN_AUTO                0b000
#define DAIKIN_DRY                 0b010
#define DAIKIN_POWERFUL       0b00000010
#define DAIKIN_SILENT         0b00100000
*/
    String status = server.arg(F("status"));
    String temp= server.arg(F("temp"));
    String fan= server.arg(F("fan"));
    String mode1= server.arg(F("mode"));
    if (mode1=="Cool") dakinir.setMode(DAIKIN_COOL);
    else if (mode1=="fan") dakinir.setMode(DAIKIN_FAN);
    else if (mode1=="auto") dakinir.setMode(DAIKIN_AUTO);
    else if (mode1=="powerful") dakinir.setMode(DAIKIN_POWERFUL);
    else if (mode1=="dry") dakinir.setMode(DAIKIN_DRY);
    else if (mode1=="heat") dakinir.setMode(DAIKIN_HEAT);
    else if (mode1=="silent") dakinir.setMode(DAIKIN_SILENT);
    if (status =="OFF"){
      Serial.println("OFF Daikin");
    dakinir.off();
  dakinir.setFan(temp.toInt());
  dakinir.setTemp(25);
  dakinir.setSwingVertical(0);
  dakinir.setSwingHorizontal(0);
    }
    else {
   dakinir.on();
  dakinir.setFan(fan.toInt());
  dakinir.setTemp(temp.toInt());
  dakinir.setSwingVertical(0);
  dakinir.setSwingHorizontal(0);
    }
  // Now send the IR signal.
  dakinir.send();
    server.send(200,F("text/html"), "OK");
   // ESP.restart();
  });
  /*
   * Send MISUBISI
   */
  server.on(html_setup_sendmisu, []() {
    /*#define DAIKIN_COOL                0b011
#define MITSUBISHI_AC_AUTO        0x20U
#define MITSUBISHI_AC_COOL        0x18U
#define MITSUBISHI_AC_DRY         0x10U
#define MITSUBISHI_AC_HEAT        0x08U
#define MITSUBISHI_AC_POWER       0x20U
#define MITSUBISHI_AC_FAN_AUTO       0U
#define MITSUBISHI_AC_FAN_MAX        5U
#define MITSUBISHI_AC_FAN_SILENT     6U
#define MITSUBISHI_AC_MIN_TEMP      16U  // 16C
#define MITSUBISHI_AC_MAX_TEMP      31U  // 31C
#define MITSUBISHI_AC_VANE_AUTO      0U
#define MITSUBISHI_AC_VANE_AUTO_MOVE 7U
*/
    String status = server.arg(F("status"));
    String temp= server.arg(F("temp"));
    String fan= server.arg(F("fan"));
    String mode1= server.arg(F("mode"));
   /* switch (mode1)
    {
    case "Cool":
          mitsubir.setMode(MITSUBISHI_AC_COOL);
          break;
    case "fan": 
          mitsubir.setMode(MITSUBISHI_AC_FAN_AUTO);
          break;
    case "auto": 
          mitsubir.setMode(MITSUBISHI_AC_AUTO);
          break;
    case "powerful": 
          mitsubir.setMode(DAIKIN_POWERFUL);
          break;
    case "dry": 
          mitsubir.setMode(MITSUBISHI_AC_DRY);
          break;
    case "heat": 
          mitsubir.setMode(MITSUBISHI_AC_HEAT);
          break;
    case "silent": 
          mitsubir.setMode(DAIKIN_SILENT);
          break;
      
    }*/
    if (mode1=="Cool") mitsubir.setMode(MITSUBISHI_AC_COOL);
    else if (mode1=="fan") mitsubir.setMode(MITSUBISHI_AC_FAN_AUTO);
    else if (mode1=="auto") mitsubir.setMode(MITSUBISHI_AC_AUTO);
    else if (mode1=="powerful") mitsubir.setMode(DAIKIN_POWERFUL);
    else if (mode1=="dry") mitsubir.setMode(MITSUBISHI_AC_DRY);
    else if (mode1=="heat") mitsubir.setMode(MITSUBISHI_AC_HEAT);
    else if (mode1=="silent") mitsubir.setMode(DAIKIN_SILENT);
    if (status =="OFF"){
       Serial.println("OFF Misu");
    mitsubir.off();
  mitsubir.setFan(temp.toInt());
  mitsubir.setTemp(25);
  mitsubir.setVane(MITSUBISHI_AC_VANE_AUTO);
    }
    else {
   mitsubir.on();
  mitsubir.setFan(fan.toInt());
  mitsubir.setTemp(temp.toInt());
  mitsubir.setVane(MITSUBISHI_AC_VANE_AUTO);
    }
  // Now send the IR signal.
  mitsubir.send();
    server.send(200,F("text/html"), "OK");
   // ESP.restart();
  });
  //////////////
  server.on(html_setup_setwifi, []() {
    
    String new_ssid = server.arg(F("ssid"));
    String new_pwd = server.arg(F("pwd"));
    String new_ip = server.arg(F("ip"));
    String new_gateway = server.arg(F("gateway"));
    String new_subnet = server.arg(F("subnet"));
    // String content =  "OK";
    if (new_ssid.length() > 0) {
      new_ssid.toCharArray(WiFiConf.sta_ssid, sizeof(WiFiConf.sta_ssid));
      new_pwd.toCharArray(WiFiConf.sta_pwd, sizeof(WiFiConf.sta_pwd));
      new_ip.toCharArray(WiFiConf.sta_ip, sizeof(WiFiConf.sta_ip));
      new_gateway.toCharArray(WiFiConf.sta_gateway, sizeof(WiFiConf.sta_gateway));
      new_subnet.toCharArray(WiFiConf.sta_subnet, sizeof(WiFiConf.sta_subnet));
      saveWiFiConf();
    } else {
     // content += F("<p>Rejected empty SSID. </p>");
     // content += F("<body></html>");
    //  Serial.println("Rejected empty SSID.");
    }
    server.send(200,F("text/html"), F("OK"));
    digitalWrite(status_led, LOW);
    ESP.restart();
  });
  server.on(html_setup_reboot, HTTP_GET, []() {
    String content = FPSTR(header);content += FPSTR(begin_title);
     content += F("mHome - Reset");
    content += FPSTR(title_html);
    content += F("<h1>ThĂ´ng Tin :</h1>");
    content += FPSTR(p_html);
    content += FPSTR(support_html);
    content += FPSTR(_p_html);
    content += FPSTR(p_html);
    content += FPSTR(get_html);
    content += F("'set_Reset'>");
    content += F("<input type='submit' value='Reboot' onclick='return confirm(\"Tiáº¿p Tá»¥c ?\");'></form>");
    content += FPSTR(_p_html);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
 server.on(html_setup_reset, HTTP_GET, []() {
    String content = FPSTR(header);content += FPSTR(begin_title);
    content += F("mHome - Reset");
    content += FPSTR(title_html);
    content += F("<h1>ThĂ´ng Tin :</h1>");
    content += FPSTR(p_html);
    content += FPSTR(support_html);
    content += FPSTR(_p_html);
    content += FPSTR(p_html);
    content += FPSTR(get_html);
    content += F("'set_Reset1'>");
    content += F("<input type='submit' value='Reset' onclick='return confirm(\"Tiếp tục ?\");'></form>");
    content += FPSTR(_p_html);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_SETreset1, HTTP_GET, []() {
        String new_IPHC = F("192.168.1.10");
    String new_pwdhc =  F("admin:admin#");
    String new_global1 = F("temp1");
    String new_global2 =  F("temp2");
    String new_global3 =  F("temp3");
    String new_ssid =  F("mhome");
    String new_pwd =  F("mhome");
    String new_ip =  F("192.168.1.220");
    String new_gateway =  F("192.168.1.1");
    String new_subnet =  F("255.255.255.0");
      new_ssid.toCharArray(WiFiConf.sta_ssid, sizeof(WiFiConf.sta_ssid));
      new_pwd.toCharArray(WiFiConf.sta_pwd, sizeof(WiFiConf.sta_pwd));
      new_ip.toCharArray(WiFiConf.sta_ip, sizeof(WiFiConf.sta_ip));
      new_gateway.toCharArray(WiFiConf.sta_gateway, sizeof(WiFiConf.sta_gateway));
      new_subnet.toCharArray(WiFiConf.sta_subnet, sizeof(WiFiConf.sta_subnet));
      new_IPHC.toCharArray(WiFiConf.sta_iphc2, sizeof(WiFiConf.sta_iphc2));
      new_pwdhc.toCharArray(WiFiConf.sta_passhc, sizeof(WiFiConf.sta_passhc));
      new_global1.toCharArray(WiFiConf.sta_global1, sizeof(WiFiConf.sta_global1));
      new_global2.toCharArray(WiFiConf.sta_global2, sizeof(WiFiConf.sta_global2));
      new_global3.toCharArray(WiFiConf.sta_global3, sizeof(WiFiConf.sta_global3));
      String lan="0";
      lan.toCharArray(WiFiConf.sta_language, sizeof(WiFiConf.sta_language));
      resetModuleId();
      saveWiFiConf();
      digitalWrite(status_led, LOW);
       ESP.restart();
  });
  server.on(html_setup_SETreset, HTTP_GET, []() {
    digitalWrite(status_led, LOW);
        ESP.restart();
  });
 server.on(html_setup_GETinfor, HTTP_GET, []() {
     String content = F("{\"T:\""); content += String(nhietdo);
     content +=  F(",\"H\":");content += String(doam);
     content +=  F(",\"PIR\":\"");content += String(time_);
     content +=  F("\",\"PIR2\":\"");content += String(motion_time);
     content +=  F("\"}");

    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_module, []() {
    char defaultId[sizeof(WiFiConf.module_id)];
    setDefaultModuleId(defaultId);
    String content = FPSTR(header);content += FPSTR(begin_title);
     content += F("mHome-Local Wifi");
    content += FPSTR(title_html);
    content += F("<h1>Thay đổi tên Wifi</h1>");
    content += FPSTR(p_html);
    content += F("Module ID: ");
    content += WiFiConf.module_id;
    content += F("</br>IP: ");
    content += ipStr;
    content += FPSTR(_p_html);
    content += FPSTR(p_html);
    content += FPSTR(get_html);
    content += F("'set_module_id'><label for='module_id'>Tên Wifi: </label><input name='module_id' id='module_id' maxlength=32 value='");
    content += WiFiConf.module_id;
    content += F("'><input type='submit' onclick='return confirm(\"Tiếp Tục ?\");'></form>");
    content += F(" Nếu để trống tên wifi sẽ là : '");
    content += defaultId;
    content += F("'");
    content += FPSTR(_p_html);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
<<<<<<< HEAD
  /* server.on("/set_language", []() {
    String new_language = server.arg("language");
    String content = FPSTR(header);content += FPSTR(begin_title);
       content += F("mHome - Language");
    content += FPSTR(title_html);
    content += F("</head><body>");
    content += F("<h1>Save</h1>");
    content += FPSTR(p_html);
    content += F("OK '");
    if (new_language =="Vietnamese") {String lan="1";lan.toCharArray(WiFiConf.sta_language, sizeof(WiFiConf.sta_language));content += "VN";}
    else
      {String lan="0";lan.toCharArray(WiFiConf.sta_language, sizeof(WiFiConf.sta_language));content += "EN";}
      
      saveWiFiConf();
      content += F("' ... Device Reboot !");
      content += FPSTR(_p_html);
      content += F("<body></html>");
    server.send(200, F("text/html"), content);
    digitalWrite(status_led, HIGH );
    ESP.restart();
  });*/
  server.on("/set_module_id", []() {
    String new_id = server.arg("module_id");
    String content = FPSTR(header);content += FPSTR(begin_title);
   // content += F("<title>");
    //content += F("mHome ");
    //content += FPSTR(title_html);
=======
  server.on(html_setup_SETmodule, []() {
    String new_id = server.arg(F("module_id"));
    String content = "";//FPSTR(header);content += FPSTR(begin_title);

>>>>>>> ir_dev
    if (new_id.length() > 0) {
      new_id.toCharArray(WiFiConf.module_id, sizeof(WiFiConf.module_id));
    } else {
      resetModuleId();
    }
    saveWiFiConf();
   // content += FPSTR(p_html);
    content += WiFiConf.module_id;
   // content += F("' ... Device Rebooting.");
   // content += FPSTR(_p_html);
   // content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
    digitalWrite(status_led, LOW);
    ESP.restart();
  });
}

void setupWeb(void) {
  server.on("/", []() {
    String content = FPSTR(header);
content += FPSTR(begin_title);
        content += F("mHome - Main page");
    content += FPSTR(title_html);
    content += F("<h1>mHome - Wifi to IR Controller </h1>");
    content += FPSTR(p_html);
    content += F("<li>Nhiệt độ : ");
    content += String(nhietdo);
    content += F(" *C");
    content += F("<li>Độ Ẩm: ");
    content += String(doam);
    content += F(" %");
    content += F("<li>PIR : ");
    content += time_;
    content += F("  UTC +7 ");

        if (hoclenh==0){
      content += F("<li>Học Lệnh : <a href='/learning'>Disable");
      
    }
    else content += F("<li>Học Lệnh : <a href='/learning'>Enable");
    content += FPSTR(_p_html);
    content +=FPSTR(fieldset);

              content +=FPSTR(legend_html);
              content +=F("'/wifi_conf'>Wifi setting");
              content +=FPSTR(_legend_html);
              if (statusmang==0){ 
                  content += F("<li> ");
                  content += FPSTR(notwifi_html);
                  content += F("<li>IP: 192.168.4.1:4999 ");
                  content += F(" ( ");
                  content += WiFiConf.module_id;
                  content += F(" )");
              }
              else{
                  content += F("<li>Connected to : ");
                  content += WiFiConf.sta_ssid;
                  content += F("<li>IP: ");
                  content += ipStr;
                  content += F(" ( ");
                  content += WiFiConf.module_id;
                  content += F(" )");
               }
              content +=FPSTR(_fieldset);
              content +=FPSTR(fieldset);
              content +=FPSTR(legend_html);
              content +=F("'/IR'>IR Leaning");
              content +=FPSTR(_legend_html);
              content +=F("<li>Description: This section is for learning IR code from IR remotes.");
              content +=F("<li>Each successful learning code should be copied to use with Fibaro HC2 later");        
              content +=FPSTR(_fieldset);
              content +=FPSTR(fieldset);
                        content +=FPSTR(legend_html);
                        content +=F("'/hc2_conf'>HC2 Setting");
                        content +=FPSTR(_legend_html);
                        content +=F("<li>Description:This section is for setting communication between HC2 and Wifi IR controller");
                        content +=F("<li>Status : ");
                        content +=SerialHC2;
              content +=FPSTR(_fieldset);
                  content +=FPSTR(fieldset);
                        content +=FPSTR(legend_html);
                        content +=F("'/firmware'>Firmware Update");
                        content +=FPSTR(_legend_html);
                        content +=F("<li>Description: This section is for update firmware of wifi IR controller");
                        content +=F("<li>Status : ");
                        content +=AppVersion;
                        content +=timeVersion;
              content +=FPSTR(_fieldset);
                  content +=FPSTR(fieldset);
                        content +=FPSTR(legend_html);
                        content +=F("'/Reboot'>Reboot Wifi IR Controller");
                        content +=FPSTR(_legend_html);
              content +=FPSTR(_fieldset);
              content +=FPSTR(fieldset);
                      content +=FPSTR(legend_html);
                      content +=F("'/Reset1'>Reset");
                      content +=FPSTR(_legend_html);
              content +=FPSTR(_fieldset);
              content +=FPSTR(fieldset);
              content +=FPSTR(legend_html);
              content +=F("'/module_id'>Wifi Name");
              content +=FPSTR(_legend_html);
              content +=FPSTR(_fieldset);
              content +=FPSTR(fieldset);
              content +=F("<legend>Information</legend>");
              content += FPSTR(information);
    content +=FPSTR(_fieldset);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
    
  });
}
//*******************
// Hafm tach IP.Gateway
//*******************
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);  // Convert byte
    str = strchr(str, sep);               // Find next separator
    if (str == NULL || *str == '\0') {
      break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
}


