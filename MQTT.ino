const int BUFFER_SIZE = 1200;
const char* on_cmd = "ON";
const char* off_cmd = "OFF";


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
    // Once connected, publish an announcement...
    clientmqtt.publish(mqtt_topic,"hello world");
    // ... and resubscribe
    clientmqtt.subscribe(mqtt_topic);
  }
  return clientmqtt.connected();
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }
  const char* sensor = root["sensor"];
  long time = root["time"];
  long latitude = root["data"][0];
  double longitude = root["data"][1];

  // Print values.
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude);
  Serial.println(longitude, 6);
  if (root.containsKey("state")) {
    if (strcmp(root["state"], on_cmd) == 0) {
     
    }
    else if (strcmp(root["state"], off_cmd) == 0) {
      
    }
  }

  // If "flash" is included, treat RGB and brightness differently
  return true;
}
