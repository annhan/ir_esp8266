const int BUFFER_SIZE = 1200;
const char* send_cmd = "SendIR";
const char* lean_cmd = "LeanIR";
const char* get_cmd = "Get_infor";


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);
    if (!processJson(message)) {
    return;
  }
}
boolean reconnect() {
  Serial.println("Reconnecting");
  //if (clientmqtt.connect("arduinoClient",mqtt_user, mqtt_pwd)) {
    if (clientmqtt.connect("arduinoClient")) {
    clientmqtt.publish(mqtt_topic,"hello world");
    clientmqtt.subscribe(mqtt_topic);
  }
  return clientmqtt.connected();
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {Serial.println("parseObject() failed");return false;} 
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
    else if (strcmp(root["command"], get_cmd) == 0) 
    {     char msg[75];  
          snprintf (msg, 75, "{\"ip\":\"%X.%X.%X.%X\",\"temp\":%d.%02d,\"hum\":%d.%02d}",ip[0],ip[1],ip[2],ip[3], (int)nhietdo, (int)(nhietdo * 10.0) % 10,(int)doam, (int)(doam * 10.0) % 10); //%ld
          Serial.println(msg);
          clientmqtt.publish(mqtt_topic, msg);
    }
  }
  return true;
}
