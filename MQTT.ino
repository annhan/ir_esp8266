const int BUFFER_SIZE = 1200;
const char* send_cmd = "SendIR";
const char* lean_cmd = "LeanIR";
const char* get_cmd = "Get_infor";
const char* setsche1_cmd = "Setsche1_infor";
const char* getsche1_cmd = "Getsche1_infor";
const char* setsche2_cmd = "Setsche2_infor";
const char* getsche2_cmd = "Getsche2_infor";
const char* setsche3_cmd = "Setsche3_infor";
const char* getsche3_cmd = "Getsche3_infor";
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  DEBUG_PRINTLN(message);
    if (!processJson(message)) {
    return;
  }
}
boolean reconnect() {
  Serial.println("Reconnecting : ");
  if (statusmang==1){
        if (WiFiConf.sta_mqtt_user[0]!='x'){  
                DEBUG_PRINTLN("Co User");
                if (clientmqtt.connect("arduinoClient",WiFiConf.sta_mqtt_user, WiFiConf.sta_mqtt_pass)) {
                 // if (clientmqtt.connect("arduinoClient")) {
                  clientmqtt.publish(WiFiConf.sta_mqtt_topic,"Reconnect");
                  clientmqtt.subscribe(WiFiConf.sta_mqtt_topic);
                }
        }
        else
        {        DEBUG_PRINTLN("Khong User");
                if (clientmqtt.connect("arduinoClient")) {
                  clientmqtt.publish(WiFiConf.sta_mqtt_topic,"Reconnect");
                  clientmqtt.subscribe(WiFiConf.sta_mqtt_topic);
                }
        }
  }
  return clientmqtt.connected();
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {DEBUG_PRINTLN("parseObject() failed");return false;} 
  if (root.containsKey("command")) {
    if (strcmp(root["command"], send_cmd) == 0)
    { String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      Serial.println(remote);
      Serial.println(button);
      type = type + "/" + remote + "/" + button + ".txt";
    int chieudai = read_file_setting(type , 3 );
    }
    else if (strcmp(root["command"], lean_cmd) == 0)
    { IR_leaning_MQTT=true;
      String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      json_ts =root["para"]["fre"].as<String>();
      Serial.println(remote);
      Serial.println(button);
      noiluu_MQTT = type + "/" + remote + "/" + button + ".txt";
    }
     else if (strcmp(root["command"], setsche1_cmd) == 0)
    { 
      String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      time_begin = root["para"]["timeB"].as<String>();
      time_end = root["para"]["timeE"].as<String>();
      String tempt = root["para"]["Temp"].as<String>();
      HG1.time_begin_int=conver_time_string_to_int(time_begin);
      HG1.time_end_int=conver_time_string_to_int(time_end);
      HG1.temp_set= tempt.toInt();
      String sche=root["para"]["sche"][0].as<String>();
      HG1.is_sun=sche.toInt();
      sche=root["para"]["sche"][1].as<String>();
      HG1.is_mon=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG1.is_tue=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG1.is_wed=sche.toInt();
      sche=root["para"]["sche"][4].as<String>();
      HG1.is_thu=sche.toInt();
      sche=root["para"]["sche"][5].as<String>();
      HG1.is_fri=sche.toInt();
      sche=root["para"]["sche"][6].as<String>();
      HG1.is_sat=sche.toInt();
      write_file_setting("Setting/setting.txt",1);
    }
    else if (strcmp(root["command"], getsche1_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 100, "{\"ip\":\"%X.%X.%X.%X\",\"TB\":%d,\"TE\":%d,\"temp\":%d.%02d,\"sche\":[%d,%d,%d,%d,%d,%d,%d]}",ip[0],ip[1],ip[2],ip[3],HG1.time_begin_int ,HG1.time_end_int ,(int)HG1.temp_set, (int)(HG1.temp_set * 10.0) % 10,HG1.is_sun,HG1.is_mon,HG1.is_tue,HG1.is_wed,HG1.is_thu,HG1.is_fri,HG1.is_sat); //%ld
          DEBUG_PRINTLN(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
    else if (strcmp(root["command"], setsche2_cmd) == 0)
    { 
      String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      time_begin = root["para"]["timeB"].as<String>();
      time_end = root["para"]["timeE"].as<String>();
      String tempt = root["para"]["Temp"].as<String>();
      HG2.time_begin_int=conver_time_string_to_int(time_begin);
      HG2.time_end_int=conver_time_string_to_int(time_end);
      HG2.temp_set= tempt.toInt();
      String sche=root["para"]["sche"][0].as<String>();
      HG2.is_sun=sche.toInt();
      sche=root["para"]["sche"][1].as<String>();
      HG2.is_mon=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG2.is_tue=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG2.is_wed=sche.toInt();
      sche=root["para"]["sche"][4].as<String>();
      HG2.is_thu=sche.toInt();
      sche=root["para"]["sche"][5].as<String>();
      HG2.is_fri=sche.toInt();
      sche=root["para"]["sche"][6].as<String>();
      HG2.is_sat=sche.toInt();
      write_file_setting("Setting/setting.txt",1);
    }
    else if (strcmp(root["command"], getsche2_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 100, "{\"ip\":\"%X.%X.%X.%X\",\"TB\":%d,\"TE\":%d,\"temp\":%d.%02d,\"sche\":[%d,%d,%d,%d,%d,%d,%d]}",ip[0],ip[1],ip[2],ip[3],HG2.time_begin_int ,HG2.time_end_int ,(int)HG2.temp_set, (int)(HG2.temp_set * 10.0) % 10,HG2.is_sun,HG1.is_mon,HG1.is_tue,HG2.is_wed,HG2.is_thu,HG2.is_fri,HG2.is_sat); //%ld
          DEBUG_PRINTLN(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
    else if (strcmp(root["command"], setsche3_cmd) == 0)
    { 
      String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      time_begin = root["para"]["timeB"].as<String>();
      time_end = root["para"]["timeE"].as<String>();
      String tempt = root["para"]["Temp"].as<String>();
      HG3.time_begin_int=conver_time_string_to_int(time_begin);
      HG3.time_end_int=conver_time_string_to_int(time_end);
      HG3.temp_set= tempt.toInt();
      String sche=root["para"]["sche"][0].as<String>();
      HG3.is_sun=sche.toInt();
      sche=root["para"]["sche"][1].as<String>();
      HG3.is_mon=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG3.is_tue=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      HG3.is_wed=sche.toInt();
      sche=root["para"]["sche"][4].as<String>();
      HG3.is_thu=sche.toInt();
      sche=root["para"]["sche"][5].as<String>();
      HG3.is_fri=sche.toInt();
      sche=root["para"]["sche"][6].as<String>();
      HG3.is_sat=sche.toInt();
      write_file_setting("Setting/setting.txt",1);
    }
    else if (strcmp(root["command"], getsche3_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 100, "{\"ip\":\"%X.%X.%X.%X\",\"TB\":%d,\"TE\":%d,\"temp\":%d.%02d,\"sche\":[%d,%d,%d,%d,%d,%d,%d]}",ip[0],ip[1],ip[2],ip[3],HG3.time_begin_int ,HG3.time_end_int ,(int)HG3.temp_set, (int)(HG3.temp_set * 10.0) % 10,HG3.is_sun,HG3.is_mon,HG3.is_tue,HG3.is_wed,HG3.is_thu,HG3.is_fri,HG3.is_sat); //%ld
          DEBUG_PRINTLN(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
    else if (strcmp(root["command"], get_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 75, "{\"ip\":\"%X.%X.%X.%X\",\"T\":%d.%02d,\"H\":%d.%02d,\"M\":%d}",ip[0],ip[1],ip[2],ip[3], (int)nhietdo, (int)(nhietdo * 10.0) % 10,(int)doam, (int)(doam * 10.0) % 10,1); //%ld
          DEBUG_PRINTLN(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
  }
  return true;
}
