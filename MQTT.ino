const int BUFFER_SIZE = 1200;
const char* send_cmd = "SendIR";
const char* lean_cmd = "LeanIR";
const char* get_cmd = "Get_infor";
const char* setsche_cmd = "Setsche_infor";
const char* getsche_cmd = "Getsche_infor";
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  NHAN_Debug(message);
    if (!processJson(message)) {
    return;
  }
}
boolean reconnect() {
  Serial.print("Reconnecting : ");
  if (statusmang==1){
        if (WiFiConf.sta_mqtt_user[0]!='x'){  
                NHAN_Debug("Co User");
                if (clientmqtt.connect("arduinoClient",WiFiConf.sta_mqtt_user, WiFiConf.sta_mqtt_pass)) {
                 // if (clientmqtt.connect("arduinoClient")) {
                  clientmqtt.publish(WiFiConf.sta_mqtt_topic,"Reconnect");
                  clientmqtt.subscribe(WiFiConf.sta_mqtt_topic);
                }
        }
        else
        {        NHAN_Debug("Khong User");
                if (clientmqtt.connect("arduinoClient")) {
                 // if (clientmqtt.connect("arduinoClient")) {
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
  if (!root.success()) {NHAN_Debug("parseObject() failed");return false;} 
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
     else if (strcmp(root["command"], setsche_cmd) == 0)
    { 
      String type=root["para"]["type"].as<String>();
      String remote=root["para"]["remote"].as<String>();
      String button=root["para"]["button"].as<String>();
      time_begin = root["para"]["timeB"].as<String>();
      time_end = root["para"]["timeE"].as<String>();
      String tempt = root["para"]["Temp"].as<String>();
      time_begin_int=conver_time_string_to_int(time_begin);
      time_end_int=conver_time_string_to_int(time_end);
      temp_set= tempt.toInt();
      String sche=root["para"]["sche"][0].as<String>();
      is_sun=sche.toInt();
      sche=root["para"]["sche"][1].as<String>();
      is_mon=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      is_tue=sche.toInt();
      sche=root["para"]["sche"][2].as<String>();
      is_wed=sche.toInt();
      sche=root["para"]["sche"][4].as<String>();
      is_thu=sche.toInt();
      sche=root["para"]["sche"][5].as<String>();
      is_fri=sche.toInt();
      sche=root["para"]["sche"][6].as<String>();
      is_sat=sche.toInt();
      write_file_setting("Setting/setting.txt",1);
    }
    else if (strcmp(root["command"], getsche_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 100, "{\"ip\":\"%X.%X.%X.%X\",\"TB\":%d,\"TE\":%d,\"temp\":%d.%02d,\"sche\":[%d,%d,%d,%d,%d,%d,%d]}",ip[0],ip[1],ip[2],ip[3],time_begin_int ,time_end_int ,(int)temp_set, (int)(temp_set * 10.0) % 10,is_sun,is_mon,is_tue,is_wed,is_thu,is_fri,is_sat); //%ld
          NHAN_Debug(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
    else if (strcmp(root["command"], get_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 75, "{\"ip\":\"%X.%X.%X.%X\",\"T\":%d.%02d,\"H\":%d.%02d,\"M\":%d}",ip[0],ip[1],ip[2],ip[3], (int)nhietdo, (int)(nhietdo * 10.0) % 10,(int)doam, (int)(doam * 10.0) % 10,1); //%ld
          NHAN_Debug(msg);
          clientmqtt.publish(WiFiConf.sta_mqtt_topic, msg);
    }
  }
  return true;
}
