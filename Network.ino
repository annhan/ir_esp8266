
String urlDecodeir(String param) {
  param.replace("+", " ");
  param.replace("%2C", ",");
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
  uint16_t *code_array = reinterpret_cast<uint16_t*>(malloc(count * sizeof(uint16_t)));
  // Check we malloc'ed successfully.
  if (code_array == NULL) {  // malloc failed, so give up.
    Serial.printf("\nCan't allocate %d bytes. (%d bytes free)\n",
                  count * sizeof(uint16_t), ESP.getFreeHeap());
    DEBUG_PRINTLN("Giving up & forcing a reboot.");
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
  for (int i = 0; i < count - 1; i++) {
    if (code_array[i] < 1310) {
      code_array[i] = code_array[i] * RAWTICK;
    }
    else code_array[i] = 65534;
    // Serial.print(code_array[i]);
    //Serial.print(",");
  }//
  // DEBUG_PRINTLN("");
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
  uint16_t *code_array = reinterpret_cast<uint16_t*>(malloc(count * sizeof(uint16_t)));
  // Check we malloc'ed successfully.
  if (code_array == NULL) {  // malloc failed, so give up.
    // Serial.printf("\nCan't allocate %d bytes. (%d bytes free)\n",count * sizeof(uint16_t), ESP.getFreeHeap());
    // DEBUG_PRINTLN("Giving up & forcing a reboot.");
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
    String content = FPSTR(header); content += FPSTR(begin_title);
    String   content1 = FPSTR(p_html);
    content += F("Connected Wifi: ");
    content1 = FPSTR(p_html);
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
    content1 += F("'set_wifi_conf'>");
    
    content1 += F("<div class=\"row\">");
    content1 += "<li><select name='button' class=\"dropbtn\" >";
    String id_check="";
    
    for (int i = 0; i < 2; i++) {
      switch (i) {
        case 0:  id_check = "DHCP";break;
        case 1:  id_check = "Static IP";break;
      }
      if (atoi(WiFiConf.sta_DHCP) != i)
      content1 += "<option value=\"" + String(i) + "\">" +  id_check + "</option>";
      else
      content1 += "<option value=\"" + String(i) + "\" selected>" +  id_check + "</option>";
    }
    content1 += "</select>";
    content1 += F("</div>");
    
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'ssid' class=\"req\">SSID : </label>");
    content1 += F("<input name='ssid'class=\"txt\" id='ssid' maxlength=32 value=") ;
    content1 += String(WiFiConf.sta_ssid) ;
    // content1 +=  FPSTR(br_html);
    content1 += F("></div>");


    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'pwd' class=\"req\">Password :  </label>");
    content1 += F("<input type='password' class=\"txt\" name='pwd' id='pwd' value=");
    content1 += String(WiFiConf.sta_pwd);


    content1 += F("></div>");

    content1 += F("<div class=\"row\">");
    //content1 += FPSTR(br_html);
    content1 += FPSTR(label1_html);
    content1 += F("'ip' class=\"req\">Ip : </label>");
    content1 += F("<input name='ip' class=\"txt\" id='ip' value=");
    content1 += String(WiFiConf.sta_ip) ;
    //  content1 +=  FPSTR(br_html);
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'gateway' class=\"req\" >Gateway :  </label>");
    content1 += F("<input  name='gateway' class=\"txt\" id='gateway' value=") ;
    content1 += String(WiFiConf.sta_gateway) ;
    //content1 += FPSTR(br_html);
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'subnet' class=\"req\">Subnet :</label>");
    content1 += F("<input  name='subnet' class=\"txt\" id='subnet' value=");
    content1 +=  String(WiFiConf.sta_subnet);
    // content1 += FPSTR(br_html);
    content1 += F("></div>");

    content += FPSTR(wifisetting_html);
    content += FPSTR(title_html);
    content += F("<h1>Wifi Setup</h1>");

    content += content1;

    content += F("<input type='submit' id=\"submitbtn\" value='OK' onclick='return confirm(\"Change Settings?\");'></form>");

    content += FPSTR(_p_html);
    content += network_html;
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_hc2, []() {
    // IPAddress ip = WiFi.localIP();
    String content = FPSTR(header); content += FPSTR(begin_title);
    String    content1 = ipStr;
    content1 = ipStr;
    content1 += F(" ( ");
    content1 += WiFiConf.module_id;
    content1 += F(".local )");
    content1 += FPSTR(p_html);
    content1 += FPSTR(get_html);
    content1 += F("'set_hc2_conf'>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'iphc2' class=\"req\">IP HC2:");
    content1 += F("</label><input name='iphc2' class=\"txt\" id='iphc2' maxlength=32 value=");
    content1 += String(WiFiConf.sta_iphc2);
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'pwdhc2' class=\"req\">PASS HC2: </label> <input type='password' class=\"txt\" name='pwdhc2' id='pwdhc2' value=");
    content1 += String(WiFiConf.sta_passhc) ;
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'global1' class=\"req\">Global 1:");

    content1 += F("</label> <input name='global1' class=\"txt\" id='global1'value=");
    content1 += String(WiFiConf.sta_global1);
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'global2' class=\"req\">Global 2:</label> <input  name='global2' class=\"txt\" id='global2' value=");
    content1 +=  String(WiFiConf.sta_global2);
    content1 += F("></div>");
    content1 += F("<div class=\"row\">");
    content1 += FPSTR(label1_html);
    content1 += F("'global3' class=\"req\">Global 3:</label> <input  name='global3'  class=\"txt\" id='global3' value=");
    content1 += String(WiFiConf.sta_global3) ;
    content1 += F("></div>");
    content += FPSTR(wifisetting_html);
    content += FPSTR(title_html);
    content += F("<h1>HC2 Setting</h1>");
    content += FPSTR(p_html);
    content += F("Wifi conecting : ");
    content += WiFiConf.sta_ssid;
    content += F("</br>IP address: ");
    content += content1;
    content += F("<input type='submit'  id=\"submitbtn\"  value='OK' onclick='return confirm(\"Change Setting ?\");'></form>");
    content += FPSTR(_p_html);
    content += FPSTR(get_html);
    content += F("'getHC'>");
    content += F("<input type='submit' id=\"submitbtn\" value='Check'></form>"); // class=\"button\"
    content += FPSTR(_p_html);
    content += F("<li>Information HC2");
    content += F("<li>Format User:password#  Ex: admin:admin123#");
    //}
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on("/getHC", []() {
    String content = FPSTR(header); content += FPSTR(begin_title);
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
    String content = FPSTR(header); content += FPSTR(begin_title);
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
    if (hoclenh == 0) {
      hoclenh = 1;
      irrecv.enableIRIn();
      server.send(200, F("text/html"), F("{\"enable\":\"1\"} <p><a href=\"/\"><button >BACK</button></a></p>"));
    }
    else {
      hoclenh = 0;
      irrecv.disableIRIn();
      server.send(200, F("text/html"), F("{\"enable\":\"0\"} <p><a href=\"/\"><button >BACK</button></a></p>"));
    }
    Serial.println(hoclenh);

  });
  server.on("/mqtt_conf", []() {
    String content = FPSTR(header); content += FPSTR(begin_title);
    String   content1 = F("<p>Wifi Connected: ");
    content1 += WiFiConf.sta_ssid;
    content1 += F("</br>IP Address: ");
    content1 += ipStr;
    content1 += F(" ( ");
    content1 += WiFiConf.module_id;
    content1 += F(".local");
    content1 += F(" )</p>");
    content1 += F("<p>");
     content1 += F("</p><form method='get' action='set_mqtt_conf'>");
    content1 += F("<li><label for='mqtt_server' class=\"req\">MQTT Server : </label><input name='mqtt_server' class=\"txt\" id='mqtt_server' maxlength=32 value=") ;
    content1 += String(WiFiConf.sta_mqtt_address) ;
    content1 +=  F(" ><br /><br />");

    content1 += F("<li><label for='mqtt_port' class=\"req\">Port : </label> <input class=\"txt\" name='mqtt_port' id='mqtt_port' value=");
    content1 += String(WiFiConf.sta_mqtt_port);
    content1 += F("><br /><br />");

    content1 += F("<li><label for='mqtt_user' class=\"req\">User : </label> <input name='mqtt_user' class=\"txt\" id='mqtt_user' value=");
    content1 += String(WiFiConf.sta_mqtt_user) ;
    content1 +=  F(" ><br /><br />");
    
    content1 += F("<li><label for='mqtt_pass' class=\"req\">Pass : </label> <input  name='mqtt_pass' class=\"txt\" id='mqtt_pass' value=") ;
    content1 += String(WiFiConf.sta_mqtt_pass) ;
    content1 += F("><br /><br />");
        content1 += F("<li><label for='mqtt_topic' class=\"req\">Topic : </label> <input  name='mqtt_topic' class=\"txt\" id='mqtt_topic' value=");
    content1 +=  String(WiFiConf.sta_mqtt_topic);
    content1 += F("><br /><br />");
    content += "mHome - Wifi Setup";
    content += F("</title></head><body>");
   /* if (WiFiConf.sta_language[0]=='1'){
    
    content += F("<h1>Cài Đặt MQTT</h1>");
    content += content1;
    content += F("<input type='submit' id=\"submitbtn\" value='OK' onclick='return confirm(\"Thay Đổi Cài Đặt?\");'></form>");
    }
    else
    {*/
    content += F("<h1>MQTT Setting</h1>");
    content += content1;
   content += F("<input type='submit' id=\"submitbtn\" value='OK' onclick='return confirm(\"Change Settings?\");'></form>");
    //}
    content += F(" </p>");
    content += F("</body></html>");
    server.send(200, F("text/html"), content);
  });
   server.on("/set_mqtt_conf", []() {
    String new_server = server.arg("mqtt_server");
    String new_port = server.arg("mqtt_port");
    String new_user = server.arg("mqtt_user");
    String new_pass = server.arg("mqtt_pass");
    String new_topic = server.arg("mqtt_topic");
    if (new_user==""){new_user="x";}
    if (new_pass==""){new_pass="x";}
      new_server.toCharArray(WiFiConf.sta_mqtt_address, sizeof(WiFiConf.sta_mqtt_address));
      int chieudai_mqtt=new_port.length();
      char tarray[chieudai_mqtt+1];      
      new_port.toCharArray(tarray, sizeof(tarray));
      WiFiConf.sta_mqtt_port = atoi(tarray);
      new_user.toCharArray(WiFiConf.sta_mqtt_user, sizeof(WiFiConf.sta_mqtt_user));
      new_pass.toCharArray(WiFiConf.sta_mqtt_pass, sizeof(WiFiConf.sta_mqtt_pass));
      new_topic.toCharArray(WiFiConf.sta_mqtt_topic, sizeof(WiFiConf.sta_mqtt_topic));

      saveWiFiConf();
      String content = F("OK...Reboot Device");
    server.send(200, "text/html", content);
    ESP.reset();
  });
  server.on(html_setup_leaningir, []() {

    String content =  F("<!DOCTYPE HTML>\r\n<html><head>");
    content = FPSTR(header); content += FPSTR(begin_title);
    //content =  F("<meta http-equiv='refresh' content='5'><style>body {background-color:lightgrey}h1 {color:blue}p {color:black}</style><link rel=\"shortcut icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAEAAAAAAAAAAAAAAAEAAAAAAADLy8sAHYsbACCNGADK28UADIEHAHOubwAShQoA//7/AObz6ADn8+gA6fPoAOzz6ADGxsYAXa5eAL7gugAghxAAtbyyABFmCAD09vEAN5ctADuPMAA4ly0AxuG9AKvXqQA2lzYAJ4cZADuXNgAikCIAKIwoAHS1cwDs++YAAHgAAKrKpwAAewAAj8WQACCHEQDa6tgAtbyzAMTcwQA4jzEAOpEuAFCkSwAYZgkAa7JiAIK9ggBAmDoAQJY9AA6FDADQ5NAApMalAC2PKQCozagA9PXwAPT48AAvbScAF2YKAGywZgBtsGYA4+3iAMviywA/mDsARpgyAM/jzgBkkGEA0eXLAC6KJAA2dDMAotCgAEWbRAA5iiQAv9+6AKmr3AAQZQgA8vXxAPn48QA+mDMAsdayAJ7DngChxKEANnQ0ACiRKABlkGIAq7iqAAB3AAAAegAAjsSQAHm5eQDKycoAw9vBAGGxYgB/mqMAhJesAPr6+AC21qoA/v/+AJqamgBygMIASpxAAMjIyADv8PYAYLBgANrr1gCUx5EAZ61gAGitYADQy9EAKGpOACOPJAD7+/wAJI8kAPv9+QDo8OUA/v//AI24iQD///8AWaxVAC+MJABerFUAXaleAF6pXgB5tHUA2uvXAPX16wDPys8AxOLGANHL0gAnhB8Agr1+AJqh1AAOfAgAzOHMAIe+gQCLvoEA6O7pAHK0bQCrt6oAMI4iAC6QKAAtlCsAS5s/AF+rXAB4t3YAwdrBAMrIygDv+e8A3O3eAM/K0ADf4uoAdJdxAHWXcQAniBoAbqtuAP3+9QAMhgwAoc6fAHO0bgAvjiMAFYYMAI28jgBGn0MAyMfIAGOrXQB4uHoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAyPe4c/NhEqT5QlaaAAX3JyXiITc5oWDmYpS11ycgByXIw8ZXJycnJycnxFg3JiclZESXJycnJycnJyOkEXfSScCXIvG1BtbW1rnYVAPRB4m3JyGgRxMU1OnoFycimVLlhycllTjXJycpAccnJmQpYLcnJkHx8zcnIeAnJyRjcPenJyDVNUBnJjgGBHchZIFDAHIAEoHWxyk1tqWnJDNjmDcm+KdCxycm4FK5hydVE7MhJyl34hGAeff3JyeRVScndoB3JyjpkfTHJyNC1VknIKYWcScnKRA3JyCIuhcFdycgl2iYQ+SjUmhoiiXHIMcnJycoI4JyMZLngkcnJyDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\"/><meta charset='UTF-8'><title>");
    content += F("mHome - IR Learn");
    content += FPSTR(title_html);
    content += F("<h1>IR Learn</h1>");

    // String hienthi;
    // for (int k = 0; k < chieudai; k++) {
    //  hienthi += String(irSignal[k]);
    //  if (k < chieudai - 1) {
    //      hienthi += ",";
    //  }
    // }
    if (statusmang == 0) {
      content += FPSTR(notwifi_html);
      content += F("<a href='/wifi_conf'>Wifi Setting</a>");
    }
    else {

      //content += FPSTR(_p_html);
      content += F("<form method='post' action='codeIR'>");

      content += F("<div class=\"row\">");
      content += FPSTR(label1_html);
      content += F("'raw' class=\"req\">IR CODE");

      content += F(": </label><input name='raw' class=\"txt\" id='raw'style=\"width:70%;\" value=");
      for (int k = 1; k < results.rawlen; k++) {
        content += String(results.rawbuf[k]);
        if (k < results.rawlen - 1) content += ",";
      }
      //content +=hienthi; //maxlength=32
      content += F("></div>");
      content += F("<div class=\"row\">");
      content += FPSTR(label1_html);
      content += F("'ts' class=\"req\">Frequency");

      content += F(": </label> <input name='ts'class=\"txt\" id='ts'  value=38000");
      content += F("></div>");


      content += F("<input type='submit' id=\"submitbtn\" value='Check' onclick='return confirm(\"Are you ready?\");'></form>");
      content += FPSTR(_p_html);
      //content += FPSTR(_p_html);
      content += F("Attention :");
      content += FPSTR(_p_html);
      content += F("<li>The default frequency is 38Khz = 38000. Change the frequency to 30000,33000,36000,38000,40000,56000 if devices not work");
      content += F("<li>When completed the new code, you have to refresh the page for the latest code update.");
      content += FPSTR(p_html);
      content += F("Thank you for reading.");

      content += F("<form method='get' action='Savecode_ML'>");
      content += F("<nav>");
      content += FPSTR(fieldset);
      content += FPSTR(legend_html);
      content += F(">Máy Lạnh");
      content += FPSTR(_legend_html);
      content += F("<div class=\"row\">");
      content += "<li><select name='button' class=\"dropbtn\">";
      content += F("<option value=\"ON\">ON</option>");
      content += F("<option value=\"OFF\">OFF</option>");
      content += F("<option value=\"16\">16 *C</option>");
      content += F("<option value=\"17\">17 *C</option>");
      content += F("<option value=\"18\">18 *C</option>");
      content += F("<option value=\"19\">19 *C</option>");
      content += F("<option value=\"20\">20 *C</option>");
      content += F("<option value=\"21\">21 *C</option>");
      content += F("<option value=\"22\">22 *C</option>");
      content += F("<option value=\"23\">23 *C</option>");
      content += F("<option value=\"24\">24 *C</option>");
      content += F("<option value=\"25\">25 *C</option>");
      content += F("<option value=\"26\">26 *C</option>");
      content += F("<option value=\"27\">27 *C</option>");
      content += F("<option value=\"28\">28 *C</option>");
      content += F("<option value=\"POWERFUL\">POWERFUL</option>");
      content += F("<option value=\"FAN1\">FAN1</option>");
      content += F("<option value=\"FAN2\">FAN2</option>");
      content += F("<option value=\"FAN3\">FAN3</option>");
      content += F("<option value=\"COOL\">COOL</option>");
      content += F("<option value=\"AUTO\">AUTO</option>");
      content += F("<option value=\"DRY\">DRY</option>");
      content += "</select>";
      content += F("</div>");
      content += F("<div class=\"row\">");
      content += F("<li><input type='submit' id=\"submitbtn\" value='Save ML' onclick='return confirm(\"Save?\");'>");
      content += F("</div>");
      content += FPSTR(_fieldset);
      content += F("</form>");
      content += F("</nav>");
      content += F("<nav>");
      content += F("<form method='get' action='Savecode_TV'>");
      content += FPSTR(fieldset);
      content += FPSTR(legend_html);
      content += F(">TV");
      content += FPSTR(_legend_html);
      content += F("<div class=\"row\">");
      content += "<li><select name='button1' class=\"dropbtn\">";
      content += F("<option value=\"ON\">ON</option>");
      content += F("<option value=\"OFF\">OFF</option>");
      content += F("<option value=\"HDMI\">HDMI</option>");
      content += F("<option value=\"NEXT\">NEXT</option>");
      content += F("<option value=\"PRE\">PRE</option>");
      content += F("<option value=\"UP\">UP</option>");
      content += F("<option value=\"DOWN\">DOWN</option>");
      content += F("<option value=\"VOLUMEUP\">Volume UP</option>");
      content += F("<option value=\"VOLUMEDW\">Volume DW</option>");
      content += F("<option value=\"CH_UP\">CH UP</option>");
      content += F("<option value=\"CH_DOWN\">CH DOWN</option>");
      content += F("<option value=\"SMART\">SMART</option>");
      content += F("<option value=\"RETURN\">RETURN</option>");
      content += F("<option value=\"OK\">OK</option>");
      content += F("<option value=\"1\">1</option>");
      content += F("<option value=\"2\">2</option>");
      content += F("<option value=\"3\">3</option>");
      content += F("<option value=\"4\">4</option>");
      content += F("<option value=\"5\">5</option>");
      content += F("<option value=\"6\">6</option>");
      content += F("<option value=\"7\">7</option>");
      content += F("<option value=\"8\">8</option>");
      content += F("<option value=\"9\">9</option>");
      content += F("<option value=\"0\">0</option>");
      content += "</select>";
      content += F("</div>");
      content += F("<div class=\"row\">");
      content += F("<li><input type='submit' id=\"submitbtn\" value='Save TV' onclick='return confirm(\"Save?\");'>");
      content += F("</div>");
      content += FPSTR(_fieldset);
      content += F("</form>");
      content += F("</nav>");

      content += F("<nav>");
      content += F("<form method='get' action='Savecode_Q'>");
      content += FPSTR(fieldset);
      content += FPSTR(legend_html);
      content += F(">Quạt");
      content += FPSTR(_legend_html);
      content += F("<div class=\"row\">");
      content += "<li><select name='button2' class=\"dropbtn\">";
      content += F("<option value=\"ON\">ON</option>");
      content += F("<option value=\"OFF\">OFF</option>");
      content += F("<option value=\"SPEED1\">SPEED 1</option>");
      content += F("<option value=\"SPEED2\">SPEED 2</option>");
      content += F("<option value=\"SPEED3\">SPEED 3</option>");
      content += F("<option value=\"SPEED4\">SPEED 4</option>");
      content += F("<option value=\"SPEED5\">SPEED 5</option>");
      content += F("<option value=\"SWIMUP\">SWIM UP</option>");
      content += F("<option value=\"SWIMLE\">SWIM LEFT</option>");
      content += "</select>";
      content += F("</div>");
      content += F("<div class=\"row\">");
      content += F("<li><input type='submit' id=\"submitbtn\" value='Save Quạt' onclick='return confirm(\"Save?\");'>");
      content += F("</div>");
      content += FPSTR(_fieldset);

      content += F("</form>");
      content += F("</nav>");

      content += F("<nav>");
      content += F("<form method='get' action='Savecode_Amply'>");
      content += FPSTR(fieldset);
      content += FPSTR(legend_html);
      content += F(">Amply");
      content += FPSTR(_legend_html);
      content += F("<div class=\"row\">");
      content += "<li><select name='button3' class=\"dropbtn\">";
      content += F("<option value=\"ON\">ON</option>");
      content += F("<option value=\"OFF\">OFF</option>");
      content += F("<option value=\"VOLUP\">VOL +</option>");
      content += F("<option value=\"VOLDW\">VOL -</option>");
      content += F("<option value=\"NEXT\">NEXT</option>");
      content += F("<option value=\"PAUSE\">PAUSE</option>");
      content += F("<option value=\"STOP\">STOP</option>");
      content += F("<option value=\"PRE\">PRE</option>");
      content += F("<option value=\"PLAY\">PLAY</option>");
      content += "</select>";
      content += F("</div>");
      content += F("<div class=\"row\">");
      content += F("<li><input type='submit' id=\"submitbtn\" value='Save Amply' onclick='return confirm(\"Save?\");'>");
      content += F("</div>");
      content += FPSTR(_fieldset);

      content += F("</form>");
      content += F("</nav>");
      //break;
      //}
    }
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });

  /*
     Save data to
  */

  server.on("/Savecode_ML", []() {
    String data1 = server.arg(F("button"));
    //  String maker=server.arg(F("maker"));
    data1 = "ML/User/" + data1 + ".txt";
    String data = write_file_setting(data1 , 3 );
    data1 = data1 + data ;
    server.send(200, F("text/html"), data1);
  });
  server.on("/Savecode_TV", []() {
    String data1 = server.arg(F("button1"));
    //  String maker=server.arg(F("maker"));
    data1 = "TV/User/" + data1 + ".txt";
    String data = write_file_setting(data1 , 3 );
    data1 = data1 + data ;
    server.send(200, F("text/html"), data1);
  });
  server.on("/Savecode_Q", []() {
    String data1 = server.arg(F("button2"));
    //  String maker=server.arg(F("maker"));
    data1 = "Quat/User/" + data1 + ".txt";
    String data = write_file_setting(data1 , 3 );
    data1 = data1 + data ;
    server.send(200, F("text/html"), data1);
  });
  server.on("/Savecode_Amply", []() {
    String data1 = server.arg(F("button3"));
    //  String maker=server.arg(F("maker"));
    data1 = "Amply/User/" + data1 + ".txt";
    String data = write_file_setting(data1 , 3 );
    data1 = data1 + data ;
    server.send(200, F("text/html"), data1);
  });
  server.on("/sendcode_SD", []() {
    String type = server.arg(F("type"));
    String maker = server.arg(F("maker"));
    String button = server.arg(F("button"));
    type = type + "/" + maker + "/" + button + ".txt";
    int chieudai = read_file_setting(type , 3 );
    Serial.println(chieudai);
    server.send(200, F("text/html"), "OK");
  });


  ////////////////////////////

  server.on("/remote_save", []() {

    String content =  F("<!DOCTYPE HTML>\r\n<html><head>");
    content = FPSTR(header); content += FPSTR(begin_title);
    //content =  F("<meta http-equiv='refresh' content='5'><style>body {background-color:lightgrey}h1 {color:blue}p {color:black}</style><link rel=\"shortcut icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAEAAAAAAAAAAAAAAAEAAAAAAADLy8sAHYsbACCNGADK28UADIEHAHOubwAShQoA//7/AObz6ADn8+gA6fPoAOzz6ADGxsYAXa5eAL7gugAghxAAtbyyABFmCAD09vEAN5ctADuPMAA4ly0AxuG9AKvXqQA2lzYAJ4cZADuXNgAikCIAKIwoAHS1cwDs++YAAHgAAKrKpwAAewAAj8WQACCHEQDa6tgAtbyzAMTcwQA4jzEAOpEuAFCkSwAYZgkAa7JiAIK9ggBAmDoAQJY9AA6FDADQ5NAApMalAC2PKQCozagA9PXwAPT48AAvbScAF2YKAGywZgBtsGYA4+3iAMviywA/mDsARpgyAM/jzgBkkGEA0eXLAC6KJAA2dDMAotCgAEWbRAA5iiQAv9+6AKmr3AAQZQgA8vXxAPn48QA+mDMAsdayAJ7DngChxKEANnQ0ACiRKABlkGIAq7iqAAB3AAAAegAAjsSQAHm5eQDKycoAw9vBAGGxYgB/mqMAhJesAPr6+AC21qoA/v/+AJqamgBygMIASpxAAMjIyADv8PYAYLBgANrr1gCUx5EAZ61gAGitYADQy9EAKGpOACOPJAD7+/wAJI8kAPv9+QDo8OUA/v//AI24iQD///8AWaxVAC+MJABerFUAXaleAF6pXgB5tHUA2uvXAPX16wDPys8AxOLGANHL0gAnhB8Agr1+AJqh1AAOfAgAzOHMAIe+gQCLvoEA6O7pAHK0bQCrt6oAMI4iAC6QKAAtlCsAS5s/AF+rXAB4t3YAwdrBAMrIygDv+e8A3O3eAM/K0ADf4uoAdJdxAHWXcQAniBoAbqtuAP3+9QAMhgwAoc6fAHO0bgAvjiMAFYYMAI28jgBGn0MAyMfIAGOrXQB4uHoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAyPe4c/NhEqT5QlaaAAX3JyXiITc5oWDmYpS11ycgByXIw8ZXJycnJycnxFg3JiclZESXJycnJycnJyOkEXfSScCXIvG1BtbW1rnYVAPRB4m3JyGgRxMU1OnoFycimVLlhycllTjXJycpAccnJmQpYLcnJkHx8zcnIeAnJyRjcPenJyDVNUBnJjgGBHchZIFDAHIAEoHWxyk1tqWnJDNjmDcm+KdCxycm4FK5hydVE7MhJyl34hGAeff3JyeRVScndoB3JyjpkfTHJyNC1VknIKYWcScnKRA3JyCIuhcFdycgl2iYQ+SjUmhoiiXHIMcnJycoI4JyMZLngkcnJyDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\"/><meta charset='UTF-8'><title>");
    content += F("mHome - IR Learn");
    content += FPSTR(title_html);
    content += F("<h1>Set Remote</h1>");
    content += F("<li>Remote ML : ");
    content += duongdan_ML;
    content += F("<li>Remote TV : ");
    content += duongdan_TV;
    String id_check="";
    /*
    content += F("<form method='get' action='set_remote_ML'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Máy Lạnh");
    content += FPSTR(_legend_html);
    content += F("<div class=\"row\">");
    content += "<li><select name='button' class=\"dropbtn\" >";
    
    
    for (int i = 0; i < 13; i++) {
      switch (i) {
        case 0:  id_check = "User";break;
        case 1:  id_check = "Carrier";break;
        case 2:  id_check = "Daikin";break;
        case 3:  id_check = "Electrolux";break;
        case 4:  id_check = "Hitachi";break;
        case 5:  id_check = "LG";break;
        case 6:  id_check = "Misu";break;
        case 7:  id_check = "Panasonic";break;
        case 8:  id_check = "Reetech";break;
        case 9:  id_check = "Samsung";break;
        case 10:  id_check = "Sanyo";break;
        case 11:  id_check = "Sharp";break;
        case 12:  id_check = "Toshiba";break;
      }
      if (atoi(WiFiConf.sta_ML) != i)
      content += "<option value=\"" + String(i) + "\">" +  id_check + "</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  id_check + "</option>";
    }
    content += "</select>";
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" value='Set ML' onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");

    
    content += F("</nav>");
*/

    content += F("<form method='get' action='set_schedule'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Schedule");
    content += FPSTR(_legend_html);


    content += F("<div class=\"row\">");
    content += "<label for='timebegin' class=\"req\">Begin: </label>";
    content += "<input type=\"time\" name=\"timebegin\" value=\"";
    content +=conver_time_int_to_string(HG1.time_begin_int);
    content += "\"/>";

    content += F("</div>");
    
    content += F("<div class=\"row\">");
    content += "<label for='timeend' class=\"req\">End: </label>";
    content += "<input type=\"time\" name=\"timeend\" value=\"";
    content +=conver_time_int_to_string(HG1.time_end_int);
    content += "\"/>";
    content += F("</div>");


    content += F("<div class=\"row\">");
    content += "<label for='button' class=\"req\">Temp : </label>";
    content += "<select name='tempset' class=\"dropbtn\" >";
    for (int i = 18; i < 26; i++) {
      if (i!=HG1.temp_set)
      content += "<option value=\"" + String(i) + "\">" +  String(i) + "*C</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  String(i) + "*C</option>";
    }
    content += "</select>";

    content += F("</div>");

    content += F("<div class=\"weekDays-selector\">");
    content += "<label  class=\"req\">Weekday : </label>";
    String thungay = "";
    for (int i = 0; i < 7; i++) {
      switch (i) {
        case 0:
              if (HG1.is_sun) thungay = "Sun\" checked";
              else thungay = "Sun\"";
              id_check = "Sun";
              break;
        case 1:
              if (HG1.is_mon) thungay = "Mon\" checked";
              else thungay = "Mon\"";
              id_check = "Mon"; 
              break;
        case 2: 
              if (HG1.is_tue) thungay = "Tue\" checked";
              else thungay = "Tue\""; 
              id_check = "Tue";
              break;
        case 3:
        if (HG1.is_wed) thungay = "Wed\" checked";
              else thungay = "Wed\"";
              id_check = "Wed";
               
              break;
        case 4: 
              if (HG1.is_thu) thungay = "Thu\" checked";
              else thungay = "Thu\""; 
              id_check = "Thu";
              break;
        case 5:
        if (HG1.is_fri) thungay = "Fri\" checked";
              else thungay = "Fri\""; 
              id_check = "Fri";
              break;
        case 6: 
              if (HG1.is_sat) thungay = "Sat\" checked";
              else thungay = "Sat\""; 
              id_check = "Sat";
              break;
      }
      content += "<input type=\"checkbox\" id=\"" + thungay + " name=\"" + id_check ;
      content += "\" value=\"OK\"/>";
      content += "<label for=\"" + id_check + "\">" + id_check ;
      content += "</label>";
    }
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");

    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on("/remote_save2", []() {

    String content =  F("<!DOCTYPE HTML>\r\n<html><head>");
    content = FPSTR(header); content += FPSTR(begin_title);
    //content =  F("<meta http-equiv='refresh' content='5'><style>body {background-color:lightgrey}h1 {color:blue}p {color:black}</style><link rel=\"shortcut icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAEAAAAAAAAAAAAAAAEAAAAAAADLy8sAHYsbACCNGADK28UADIEHAHOubwAShQoA//7/AObz6ADn8+gA6fPoAOzz6ADGxsYAXa5eAL7gugAghxAAtbyyABFmCAD09vEAN5ctADuPMAA4ly0AxuG9AKvXqQA2lzYAJ4cZADuXNgAikCIAKIwoAHS1cwDs++YAAHgAAKrKpwAAewAAj8WQACCHEQDa6tgAtbyzAMTcwQA4jzEAOpEuAFCkSwAYZgkAa7JiAIK9ggBAmDoAQJY9AA6FDADQ5NAApMalAC2PKQCozagA9PXwAPT48AAvbScAF2YKAGywZgBtsGYA4+3iAMviywA/mDsARpgyAM/jzgBkkGEA0eXLAC6KJAA2dDMAotCgAEWbRAA5iiQAv9+6AKmr3AAQZQgA8vXxAPn48QA+mDMAsdayAJ7DngChxKEANnQ0ACiRKABlkGIAq7iqAAB3AAAAegAAjsSQAHm5eQDKycoAw9vBAGGxYgB/mqMAhJesAPr6+AC21qoA/v/+AJqamgBygMIASpxAAMjIyADv8PYAYLBgANrr1gCUx5EAZ61gAGitYADQy9EAKGpOACOPJAD7+/wAJI8kAPv9+QDo8OUA/v//AI24iQD///8AWaxVAC+MJABerFUAXaleAF6pXgB5tHUA2uvXAPX16wDPys8AxOLGANHL0gAnhB8Agr1+AJqh1AAOfAgAzOHMAIe+gQCLvoEA6O7pAHK0bQCrt6oAMI4iAC6QKAAtlCsAS5s/AF+rXAB4t3YAwdrBAMrIygDv+e8A3O3eAM/K0ADf4uoAdJdxAHWXcQAniBoAbqtuAP3+9QAMhgwAoc6fAHO0bgAvjiMAFYYMAI28jgBGn0MAyMfIAGOrXQB4uHoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAyPe4c/NhEqT5QlaaAAX3JyXiITc5oWDmYpS11ycgByXIw8ZXJycnJycnxFg3JiclZESXJycnJycnJyOkEXfSScCXIvG1BtbW1rnYVAPRB4m3JyGgRxMU1OnoFycimVLlhycllTjXJycpAccnJmQpYLcnJkHx8zcnIeAnJyRjcPenJyDVNUBnJjgGBHchZIFDAHIAEoHWxyk1tqWnJDNjmDcm+KdCxycm4FK5hydVE7MhJyl34hGAeff3JyeRVScndoB3JyjpkfTHJyNC1VknIKYWcScnKRA3JyCIuhcFdycgl2iYQ+SjUmhoiiXHIMcnJycoI4JyMZLngkcnJyDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\"/><meta charset='UTF-8'><title>");
    content += F("mHome - IR Learn");
    content += FPSTR(title_html);
    content += F("<h1>Set Remote</h1>");
    content += F("<li>Remote ML : ");
    content += duongdan_ML;
    content += F("<li>Remote TV : ");
    content += duongdan_TV;
    String id_check="";
    String thungay="";
    /*
    content += F("<form method='get' action='set_remote_ML'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Máy Lạnh");
    content += FPSTR(_legend_html);
    content += F("<div class=\"row\">");
    content += "<li><select name='button' class=\"dropbtn\" >";
    
    
    for (int i = 0; i < 13; i++) {
      switch (i) {
        case 0:  id_check = "User";break;
        case 1:  id_check = "Carrier";break;
        case 2:  id_check = "Daikin";break;
        case 3:  id_check = "Electrolux";break;
        case 4:  id_check = "Hitachi";break;
        case 5:  id_check = "LG";break;
        case 6:  id_check = "Misu";break;
        case 7:  id_check = "Panasonic";break;
        case 8:  id_check = "Reetech";break;
        case 9:  id_check = "Samsung";break;
        case 10:  id_check = "Sanyo";break;
        case 11:  id_check = "Sharp";break;
        case 12:  id_check = "Toshiba";break;
      }
      if (atoi(WiFiConf.sta_ML) != i)
      content += "<option value=\"" + String(i) + "\">" +  id_check + "</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  id_check + "</option>";
    }
    content += "</select>";
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" value='Set ML' onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");

    
    content += F("</nav>");
*/
    content += F("<form method='get' action='set_schedule2'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Schedule 3");
    content += FPSTR(_legend_html);


    content += F("<div class=\"row\">");
    content += "<label for='timebegin' class=\"req\">Begin: </label>";
    content += "<input type=\"time\" name=\"timebegin\" value=\"";
    content +=conver_time_int_to_string(HG3.time_begin_int);
    content += "\"/>";

    content += F("</div>");
    
    content += F("<div class=\"row\">");
    content += "<label for='timeend' class=\"req\">End: </label>";
    content += "<input type=\"time\" name=\"timeend\" value=\"";
    content +=conver_time_int_to_string(HG3.time_end_int);
    content += "\"/>";
    content += F("</div>");


    content += F("<div class=\"row\">");
    content += "<label for='button' class=\"req\">Temp : </label>";
    content += "<select name='tempset' class=\"dropbtn\" >";
    for (int i = 18; i < 26; i++) {
      if (i!=HG3.temp_set)
      content += "<option value=\"" + String(i) + "\">" +  String(i) + "*C</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  String(i) + "*C</option>";
    }
    content += "</select>";

    content += F("</div>");

    content += F("<div class=\"weekDays-selector\">");
    content += "<label  class=\"req\">Weekday : </label>";
    for (int i = 0; i < 7; i++) {
      switch (i) {
        case 0:
              if (HG3.is_sun) thungay = "Sun\" checked";
              else thungay = "Sun\"";
              id_check = "Sun";
              break;
        case 1:
              if (HG3.is_mon) thungay = "Mon\" checked";
              else thungay = "Mon\"";
              id_check = "Mon"; 
              break;
        case 2: 
              if (HG3.is_tue) thungay = "Tue\" checked";
              else thungay = "Tue\""; 
              id_check = "Tue";
              break;
        case 3:
        if (HG3.is_wed) thungay = "Wed\" checked";
              else thungay = "Wed\"";
              id_check = "Wed";
               
              break;
        case 4: 
              if (HG3.is_thu) thungay = "Thu\" checked";
              else thungay = "Thu\""; 
              id_check = "Thu";
              break;
        case 5:
        if (HG3.is_fri) thungay = "Fri\" checked";
              else thungay = "Fri\""; 
              id_check = "Fri";
              break;
        case 6: 
              if (HG3.is_sat) thungay = "Sat\" checked";
              else thungay = "Sat\""; 
              id_check = "Sat";
              break;
      }
      content += "<input type=\"checkbox\" id=\"" + thungay + " name=\"" + id_check ;
      content += "\" value=\"OK\"/>";
      content += "<label for=\"" + id_check + "\">" + id_check ;
      content += "</label>";
    }
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");


    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on("/remote_save1", []() {

    String content =  F("<!DOCTYPE HTML>\r\n<html><head>");
    content = FPSTR(header); content += FPSTR(begin_title);
    //content =  F("<meta http-equiv='refresh' content='5'><style>body {background-color:lightgrey}h1 {color:blue}p {color:black}</style><link rel=\"shortcut icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAAAAEACABoBQAAFgAAACgAAAAQAAAAIAAAAAEACAAAAAAAAAEAAAAAAAAAAAAAAAEAAAAAAADLy8sAHYsbACCNGADK28UADIEHAHOubwAShQoA//7/AObz6ADn8+gA6fPoAOzz6ADGxsYAXa5eAL7gugAghxAAtbyyABFmCAD09vEAN5ctADuPMAA4ly0AxuG9AKvXqQA2lzYAJ4cZADuXNgAikCIAKIwoAHS1cwDs++YAAHgAAKrKpwAAewAAj8WQACCHEQDa6tgAtbyzAMTcwQA4jzEAOpEuAFCkSwAYZgkAa7JiAIK9ggBAmDoAQJY9AA6FDADQ5NAApMalAC2PKQCozagA9PXwAPT48AAvbScAF2YKAGywZgBtsGYA4+3iAMviywA/mDsARpgyAM/jzgBkkGEA0eXLAC6KJAA2dDMAotCgAEWbRAA5iiQAv9+6AKmr3AAQZQgA8vXxAPn48QA+mDMAsdayAJ7DngChxKEANnQ0ACiRKABlkGIAq7iqAAB3AAAAegAAjsSQAHm5eQDKycoAw9vBAGGxYgB/mqMAhJesAPr6+AC21qoA/v/+AJqamgBygMIASpxAAMjIyADv8PYAYLBgANrr1gCUx5EAZ61gAGitYADQy9EAKGpOACOPJAD7+/wAJI8kAPv9+QDo8OUA/v//AI24iQD///8AWaxVAC+MJABerFUAXaleAF6pXgB5tHUA2uvXAPX16wDPys8AxOLGANHL0gAnhB8Agr1+AJqh1AAOfAgAzOHMAIe+gQCLvoEA6O7pAHK0bQCrt6oAMI4iAC6QKAAtlCsAS5s/AF+rXAB4t3YAwdrBAMrIygDv+e8A3O3eAM/K0ADf4uoAdJdxAHWXcQAniBoAbqtuAP3+9QAMhgwAoc6fAHO0bgAvjiMAFYYMAI28jgBGn0MAyMfIAGOrXQB4uHoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAyPe4c/NhEqT5QlaaAAX3JyXiITc5oWDmYpS11ycgByXIw8ZXJycnJycnxFg3JiclZESXJycnJycnJyOkEXfSScCXIvG1BtbW1rnYVAPRB4m3JyGgRxMU1OnoFycimVLlhycllTjXJycpAccnJmQpYLcnJkHx8zcnIeAnJyRjcPenJyDVNUBnJjgGBHchZIFDAHIAEoHWxyk1tqWnJDNjmDcm+KdCxycm4FK5hydVE7MhJyl34hGAeff3JyeRVScndoB3JyjpkfTHJyNC1VknIKYWcScnKRA3JyCIuhcFdycgl2iYQ+SjUmhoiiXHIMcnJycoI4JyMZLngkcnJyDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\"/><meta charset='UTF-8'><title>");
    content += F("mHome - IR Learn");
    content += FPSTR(title_html);
    content += F("<h1>Set Remote</h1>");
    content += F("<li>Remote ML : ");
    content += duongdan_ML;
    content += F("<li>Remote TV : ");
    content += duongdan_TV;
    String id_check="";
    String thungay="";
    /*
    content += F("<form method='get' action='set_remote_ML'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Máy Lạnh");
    content += FPSTR(_legend_html);
    content += F("<div class=\"row\">");
    content += "<li><select name='button' class=\"dropbtn\" >";
    
    
    for (int i = 0; i < 13; i++) {
      switch (i) {
        case 0:  id_check = "User";break;
        case 1:  id_check = "Carrier";break;
        case 2:  id_check = "Daikin";break;
        case 3:  id_check = "Electrolux";break;
        case 4:  id_check = "Hitachi";break;
        case 5:  id_check = "LG";break;
        case 6:  id_check = "Misu";break;
        case 7:  id_check = "Panasonic";break;
        case 8:  id_check = "Reetech";break;
        case 9:  id_check = "Samsung";break;
        case 10:  id_check = "Sanyo";break;
        case 11:  id_check = "Sharp";break;
        case 12:  id_check = "Toshiba";break;
      }
      if (atoi(WiFiConf.sta_ML) != i)
      content += "<option value=\"" + String(i) + "\">" +  id_check + "</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  id_check + "</option>";
    }
    content += "</select>";
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" value='Set ML' onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");

    
    content += F("</nav>");
*/

    
//################################ Sche 2

content += F("<form method='get' action='set_schedule1'>");
    content += F("<nav>");
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F(">Schedule");
    content += FPSTR(_legend_html);


    content += F("<div class=\"row\">");
    content += "<label for='timebegin' class=\"req\">Begin: </label>";
    content += "<input type=\"time\" name=\"timebegin\" value=\"";
    content +=conver_time_int_to_string(HG2.time_begin_int);
    content += "\"/>";

    content += F("</div>");
    
    content += F("<div class=\"row\">");
    content += "<label for='timeend' class=\"req\">End: </label>";
    content += "<input type=\"time\" name=\"timeend\" value=\"";
    content +=conver_time_int_to_string(HG2.time_end_int);
    content += "\"/>";
    content += F("</div>");


    content += F("<div class=\"row\">");
    content += "<label for='button' class=\"req\">Temp : </label>";
    content += "<select name='tempset' class=\"dropbtn\" >";
    for (int i = 18; i < 26; i++) {
      if (i!=HG2.temp_set)
      content += "<option value=\"" + String(i) + "\">" +  String(i) + "*C</option>";
      else
      content += "<option value=\"" + String(i) + "\" selected>" +  String(i) + "*C</option>";
    }
    content += "</select>";

    content += F("</div>");

    content += F("<div class=\"weekDays-selector\">");
    content += "<label  class=\"req\">Weekday : </label>";

    for (int i = 0; i < 7; i++) {
      switch (i) {
        case 0:
              if (HG2.is_sun) thungay = "Sun\" checked";
              else thungay = "Sun\"";
              id_check = "Sun";
              break;
        case 1:
              if (HG2.is_mon) thungay = "Mon\" checked";
              else thungay = "Mon\"";
              id_check = "Mon"; 
              break;
        case 2: 
              if (HG2.is_tue) thungay = "Tue\" checked";
              else thungay = "Tue\""; 
              id_check = "Tue";
              break;
        case 3:
        if (HG2.is_wed) thungay = "Wed\" checked";
              else thungay = "Wed\"";
              id_check = "Wed";
               
              break;
        case 4: 
              if (HG2.is_thu) thungay = "Thu\" checked";
              else thungay = "Thu\""; 
              id_check = "Thu";
              break;
        case 5:
        if (HG2.is_fri) thungay = "Fri\" checked";
              else thungay = "Fri\""; 
              id_check = "Fri";
              break;
        case 6: 
              if (HG2.is_sat) thungay = "Sat\" checked";
              else thungay = "Sat\""; 
              id_check = "Sat";
              break;
      }
      content += "<input type=\"checkbox\" id=\"" + thungay + " name=\"" + id_check ;
      content += "\" value=\"OK\"/>";
      content += "<label for=\"" + id_check + "\">" + id_check ;
      content += "</label>";
    }
    content += F("</div>");
    content += F("<div class=\"row\">");
    content += F("<li><input type='submit' id=\"submitbtn\" onclick='return confirm(\"Save?\");'>");
    content += F("</div>");
    content += FPSTR(_fieldset);
    content += F("</form>");


    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on("/set_schedule",  []() {
    time_begin = server.arg(F("timebegin"));
    time_end = server.arg(F("timeend"));
    String tempt = server.arg(F("tempset"));
    HG1.time_begin_int=conver_time_string_to_int(time_begin);
    HG1.time_end_int=conver_time_string_to_int(time_end);
    HG1.temp_set= tempt.toInt();
    if (server.hasArg("Sun")){DEBUG_PRINTLN("OK Sunday");HG1.is_sun=1;}
    else HG1.is_sun=0;
    if (server.hasArg("Mon")){DEBUG_PRINTLN("OK Monday");HG1.is_mon=1;}
    else HG1.is_mon=0;
    if (server.hasArg("Tue")){DEBUG_PRINTLN("OK Tueday");HG1.is_tue=1;}
    else HG1.is_tue=0;
    if (server.hasArg("Wed")){DEBUG_PRINTLN("OK Webday");HG1.is_wed=1;}
    else HG1.is_wed=0;
    if (server.hasArg("Thu")){DEBUG_PRINTLN("OK Thuday");HG1.is_thu=1;}
    else HG1.is_thu=0;
    if (server.hasArg("Fri")){DEBUG_PRINTLN("OK Friday");HG1.is_fri=1;}
    else HG1.is_fri=0;
    if (server.hasArg("Sat")){DEBUG_PRINTLN("OK Satday");HG1.is_sat=1;}
    else HG1.is_sat=0;
    write_file_setting("Setting/setting.txt",1);
    server.send(200, F("text/html"), duongdan_ML);

  });
    server.on("/set_schedule1",  []() {
    time_begin = server.arg(F("timebegin"));
    time_end = server.arg(F("timeend"));
    String tempt = server.arg(F("tempset"));
    HG2.time_begin_int=conver_time_string_to_int(time_begin);
    HG2.time_end_int=conver_time_string_to_int(time_end);
    HG2.temp_set= tempt.toInt();
    if (server.hasArg("Sun")){DEBUG_PRINTLN("OK Sunday");HG2.is_sun=1;}
    else HG2.is_sun=0;
    if (server.hasArg("Mon")){DEBUG_PRINTLN("OK Monday");HG2.is_mon=1;}
    else HG2.is_mon=0;
    if (server.hasArg("Tue")){DEBUG_PRINTLN("OK Tueday");HG2.is_tue=1;}
    else HG2.is_tue=0;
    if (server.hasArg("Wed")){DEBUG_PRINTLN("OK Webday");HG2.is_wed=1;}
    else HG2.is_wed=0;
    if (server.hasArg("Thu")){DEBUG_PRINTLN("OK Thuday");HG2.is_thu=1;}
    else HG2.is_thu=0;
    if (server.hasArg("Fri")){DEBUG_PRINTLN("OK Friday");HG2.is_fri=1;}
    else HG2.is_fri=0;
    if (server.hasArg("Sat")){DEBUG_PRINTLN("OK Satday");HG2.is_sat=1;}
    else HG2.is_sat=0;
    write_file_setting("Setting/setting.txt",1);
    server.send(200, F("text/html"), duongdan_ML);

  });
    server.on("/set_schedule2",  []() {
    time_begin = server.arg(F("timebegin"));
    time_end = server.arg(F("timeend"));
    String tempt = server.arg(F("tempset"));
    HG3.time_begin_int=conver_time_string_to_int(time_begin);
    HG3.time_end_int=conver_time_string_to_int(time_end);
    HG3.temp_set= tempt.toInt();
    if (server.hasArg("Sun")){DEBUG_PRINTLN("OK Sunday");HG3.is_sun=1;}
    else HG3.is_sun=0;
    if (server.hasArg("Mon")){DEBUG_PRINTLN("OK Monday");HG3.is_mon=1;}
    else HG3.is_mon=0;
    if (server.hasArg("Tue")){DEBUG_PRINTLN("OK Tueday");HG3.is_tue=1;}
    else HG3.is_tue=0;
    if (server.hasArg("Wed")){DEBUG_PRINTLN("OK Webday");HG3.is_wed=1;}
    else HG3.is_wed=0;
    if (server.hasArg("Thu")){DEBUG_PRINTLN("OK Thuday");HG3.is_thu=1;}
    else HG3.is_thu=0;
    if (server.hasArg("Fri")){DEBUG_PRINTLN("OK Friday");HG3.is_fri=1;}
    else HG3.is_fri=0;
    if (server.hasArg("Sat")){DEBUG_PRINTLN("OK Satday");HG3.is_sat=1;}
    else HG3.is_sat=0;
    write_file_setting("Setting/setting.txt",1);
    server.send(200, F("text/html"), duongdan_ML);

  });
  server.on("/set_remote_ML",  []() {
    String data1 = server.arg(F("button"));
    data1.toCharArray(WiFiConf.sta_ML, sizeof(WiFiConf.sta_ML));
    user_using();
    saveWiFiConf();
    server.send(200, F("text/html"), duongdan_ML);

  });
  /*server.on("/set_remote_TV",  []() {
    String data1 = server.arg(F("button"));
    data1.toCharArray(WiFiConf.sta_TV, sizeof(WiFiConf.sta_TV));
    user_using();
    saveWiFiConf();
    server.send(200, F("text/html"), duongdan_TV);

  });*/
  ///////////////////////
  /*
     POST DATA IR
  */
  server.on(html_setup_codeIR, HTTP_POST, []() {
    if (hoclenh == 1) {
      irrecv.disableIRIn();
    }
    String data1 = server.arg(F("raw"));
    String ts1 = server.arg(F("ts"));
    json_ts = ts1;
    if (data1.length() >= 1264) {
      data1 = urlDecodeir(data1);
    }
    String content = "";
    if (data1.length() < 12) {
      content += F("Not OK");
      DEBUG_PRINTLN(" ");
    }
    else {
      data1 += ",";
      data1 += ts1;
      parseStringRAW(data1);
      content += F("OK");
    }
    server.send(200, F("text/html"), content);
    if (hoclenh == 1) {
      irrecv.enableIRIn();
    }
  });
  server.on(html_setup_codeGC, []() {
    if (hoclenh == 1) {
      irrecv.disableIRIn();
      //  delay(500);
    }
    String data1 = server.arg("raw");
    parseStringGC(data1);
    String content = FPSTR(header); content += FPSTR(begin_title);
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
    server.send(200, F("text/html"), status_html_ok);
  });

  /*
     Get code IR
  */
  server.on(html_setup_getcode, []() {
    // String hienthi;
    //for (int k = 0; k < chieudai; k++) {
    //  hienthi += String(irSignal[k]);
    //  if (k < chieudai - 1)  hienthi += ",";
    // }
    String json = "{";
    //json += "\"code\":\"" + hienthi ;
    json += "\"code\":\"" ;
    for (int k = 1; k < results.rawlen; k++) {
      json += String(results.rawbuf[k]);
      if (k < results.rawlen - 1) {
        json += ",";
      }
    }
    json += "\",\"f\":" + json_ts;
    if (hoclenh == 0) json += "\",\"enable\": 1";
    else json += "\",\"enable\": 0";
    json += "}";
    if (json.length() > 50 ) {
      server.send(200, F("text/html"), json);
    }
    else server.send(200, F("text/html"), F("{\"code\":\"error\",\"f\":\"38000\"}"));
    // ESP.restart();
  });
  /*
     Send Daikin
  */
  server.on(html_setup_senddaikin, HTTP_GET, []() {
    /*#define DAIKIN_COOL                0b011
      #define DAIKIN_HEAT                0b100
      #define DAIKIN_FAN                 0b110
      #define DAIKIN_AUTO                0b000
      #define DAIKIN_DRY                 0b010
      #define DAIKIN_POWERFUL       0b00000010
      #define DAIKIN_SILENT         0b00100000
    */
   /* String status = server.arg(F("status"));
    String temp = server.arg(F("temp"));
    String fan = server.arg(F("fan"));
    String mode1 = server.arg(F("mode"));
    if (mode1 == "Cool") dakinir.setMode(DAIKIN_COOL);
    else if (mode1 == "fan") dakinir.setMode(DAIKIN_FAN);
    else if (mode1 == "auto") dakinir.setMode(DAIKIN_AUTO);
    else if (mode1 == "powerful") dakinir.setMode(DAIKIN_POWERFUL);
    else if (mode1 == "dry") dakinir.setMode(DAIKIN_DRY);
    else if (mode1 == "heat") dakinir.setMode(DAIKIN_HEAT);
    else if (mode1 == "silent") dakinir.setMode(DAIKIN_SILENT);
    if (status == "OFF") {
      DEBUG_PRINTLN("OFF Daikin");
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
    dakinir.send()*/
    server.send(200, F("text/html"), status_html_ok);
    // ESP.restart();
  });
  

  server.on(html_setup_sendmisu, HTTP_GET,  []() {
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
    String temp = server.arg(F("temp"));
    String fan = server.arg(F("fan"));
    String mode1 = server.arg(F("mode"));
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
    /*if (mode1 == "Cool") mitsubir.setMode(MITSUBISHI_AC_COOL);
    else if (mode1 == "fan") mitsubir.setMode(MITSUBISHI_AC_FAN_AUTO);
    else if (mode1 == "auto") mitsubir.setMode(MITSUBISHI_AC_AUTO);
    else if (mode1 == "powerful") mitsubir.setMode(DAIKIN_BYTE_POWERFUL);
    else if (mode1 == "dry") mitsubir.setMode(MITSUBISHI_AC_DRY);
    else if (mode1 == "heat") mitsubir.setMode(MITSUBISHI_AC_HEAT);
    else if (mode1 == "silent") mitsubir.setMode(DAIKIN_SILENT);
    if (status == "OFF") {
      DEBUG_PRINTLN("OFF Misu");
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
    mitsubir.send();*/
    server.send(200, F("text/html"), status_html_ok);
    // ESP.restart();
  });
  //////////////
  server.on(html_setup_setwifi, HTTP_GET, []() {
    String data1 = server.arg(F("button"));
    data1.toCharArray(WiFiConf.sta_DHCP, sizeof(WiFiConf.sta_DHCP));
    DEBUG_PRINTLN(WiFiConf.sta_DHCP);
    String new_ssid = server.arg(F("ssid"));
    String new_pwd = server.arg(F("pwd"));
    String new_ip = server.arg(F("ip"));
    String new_gateway = server.arg(F("gateway"));
    String new_subnet = server.arg(F("subnet"));

    String content =  status_html_ok;


    if (new_ssid.length() > 0) {
      new_ssid.toCharArray(WiFiConf.sta_ssid, sizeof(WiFiConf.sta_ssid));
      new_pwd.toCharArray(WiFiConf.sta_pwd, sizeof(WiFiConf.sta_pwd));
      new_ip.toCharArray(WiFiConf.sta_ip, sizeof(WiFiConf.sta_ip));
      new_gateway.toCharArray(WiFiConf.sta_gateway, sizeof(WiFiConf.sta_gateway));
      new_subnet.toCharArray(WiFiConf.sta_subnet, sizeof(WiFiConf.sta_subnet));
      saveWiFiConf();

    } else {

      content = "NOT OK";
    }
    server.send(200, F("text/html"), content);

    delay(500);
    digitalWrite(status_led, LOW);
    ESP.restart();
  });
  server.on(html_setup_reboot, HTTP_GET, []() {
    String content = FPSTR(header); content += FPSTR(begin_title);
    content += F("mHome - Reset");
    content += FPSTR(title_html);
    content += F("<h1>Thông Tin :</h1>");
    content += FPSTR(p_html);
    content += FPSTR(support_html);
    content += FPSTR(_p_html);
    content += FPSTR(p_html);
    content += FPSTR(get_html);
    content += F("'set_Reset'>");
    content += F("<input type='submit' id=\"submitbtn\" value='Reboot' onclick='return confirm(\"Tiáº¿p Tá»¥c ?\");'></form>");
    content += FPSTR(_p_html);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_reset, HTTP_GET, []() {
    String content = FPSTR(header); content += FPSTR(begin_title);
    content += F("mHome - Reset");
    content += FPSTR(title_html);
    content += F("<h1>Thông Tin :</h1>");
    content += FPSTR(p_html);
    content += FPSTR(support_html);
    content += FPSTR(_p_html);
    content += FPSTR(p_html);
    content += FPSTR(get_html);
    content += F("'set_Reset1'>");
    content += F("<input type='submit' id=\"submitbtn\" value='Reset' onclick='return confirm(\"Tiếp tục ?\");'></form>");
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
    String lan = "0";
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
    String content = F("{\"T\":"); content += String(nhietdo);
    content +=  F(",\"H\":"); content += String(doam);
    content +=  F(",\"PIR\":\""); content += String(time_);
    content +=  F("\",\"PIR2\":\""); content += String(motion_time);
    content +=  F("\"}");

    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_module, []() {
    char defaultId[sizeof(WiFiConf.module_id)];
    setDefaultModuleId(defaultId);
    String content = FPSTR(header); content += FPSTR(begin_title);
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
    content += F("'set_module_id'>");

    content += F("<div class=\"row\">");
    content += F("<label for='module_id' class=\"req\" >Tên Wifi: </label><input name='module_id' class=\"txt\" id='module_id' maxlength=32 value=");
    content += WiFiConf.module_id;
    content += F("></div>");
    content += F("<div class=\"row\">");
    content += F("<input type='submit' id=\"submitbtn\" value='OK' onclick='return confirm(\"Tiếp Tục ?\");'></div></form>");
    content += F(" Nếu để trống tên wifi sẽ là : '");
    content += defaultId;
    content += F("'");
    content += FPSTR(_p_html);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);
  });
  server.on(html_setup_SETmodule, []() {
    String new_id = server.arg(F("module_id"));
    String content = "";//FPSTR(header);content += FPSTR(begin_title);

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
    content += FPSTR(fieldset);

    content += F("<li>Today : ");
    content += conver_day(weekday);
    content += F("<li>Nhiệt độ : ");
    content += String(nhietdo);
    content += F(" *C");
    content += F("<li>Độ Ẩm: ");
    content += String(doam);
    content += F(" %");
    content += F("<li>PIR : ");
    content += time_;
    content += F("  UTC +7 ");

    if (hoclenh == 0) {
      content += F("<li>Học Lệnh : <a href='/learning'>Disable</a>");

    }
    else content += F("<li>Học Lệnh : <a href='/learning'>Enable</a>");
    content += FPSTR(_fieldset);
    content += FPSTR(_p_html);
    content += FPSTR(fieldset);

    content += FPSTR(legend_html);
    content += F("'/wifi_conf'>Wifi setting");
    content += FPSTR(_legend_html);
    if (statusmang == 0) {
      content += F("<li> ");
      content += FPSTR(notwifi_html);
      content += F("<li>IP: 192.168.4.1:4999 ");
      content += F(" ( ");
      content += WiFiConf.module_id;
      content += F(" )");
    }
    else {
      content += F("<li>Connected to : ");
      content += WiFiConf.sta_ssid;
      content += F("<li>IP: ");
      content += ipStr;
      content += F(" ( ");
      content += WiFiConf.module_id;
      content += F(" )");
    }
    content += FPSTR(_fieldset);
    
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/IR'>IR Leaning");
    content += FPSTR(_legend_html);
    content += F("<li>Description: This section is for learning IR code from IR remotes.");
    content += F("<li>Each successful learning code should be copied to use with Fibaro HC2 later");
    content += FPSTR(_fieldset);
        content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/mqtt_conf'>MQTT Setting");
    content += FPSTR(_legend_html);
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/hc2_conf'>HC2 Setting");
    content += FPSTR(_legend_html);
    content += F("<li>Description:This section is for setting communication between HC2 and Wifi IR controller");
    content += F("<li>Status : ");
    content += SerialHC2;
    content += FPSTR(_fieldset);

    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/remote_save'>Setting Schema1");
    content += FPSTR(_legend_html);
        content += FPSTR(legend_html);
    content += F("'/remote_save1'>Setting Schema2");
    content += FPSTR(_legend_html);
        content += FPSTR(legend_html);
    content += F("'/remote_save2'>Setting Schema3");
    content += FPSTR(_legend_html);
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/firmware'>Firmware Update");
    content += FPSTR(_legend_html);
    content += F("<li>Description: This section is for update firmware of wifi IR controller");
    content += F("<li>Status : ");
    content += AppVersion;
    content += timeVersion;
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/Reboot'>Reboot Wifi IR Controller");
    content += FPSTR(_legend_html);
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/Reset1'>Reset");
    content += FPSTR(_legend_html);
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += FPSTR(legend_html);
    content += F("'/module_id'>Wifi Name");
    content += FPSTR(_legend_html);
    content += FPSTR(_fieldset);
    content += FPSTR(fieldset);
    content += F("<legend>Information</legend>");
    content += FPSTR(information);
    content += FPSTR(_fieldset);
    content += FPSTR(end_html);
    server.send(200, F("text/html"), content);

  });
}
//*******************
// Hafm tach IP.Gateway
//*******************
/*
  byte ip1[4];
  byte gateway[4];
  byte subnet[4];
  void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);  
    DEBUG_PRINTLN(bytes[i]);
    str = strchr(str, sep);               
    if (str == NULL || *str == '\0') {
      break;                            
    }
    str++;                               
  }
}*/
void parseBytes1(const char* str, char sep, int address, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    if (address == 1) ip10[i] = strtoul(str, NULL, base);  // Convert byte  ip10
    else if (address == 2) gateway10[i] = strtoul(str, NULL, base);  // Convert byte
    else if (address == 3) subnet10[i] = strtoul(str, NULL, base);  // Convert byte
   // DEBUG_PRINTLN(bytes[i]);
    str = strchr(str, sep);               // Find next separator
    if (str == NULL || *str == '\0') {
      break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
}

