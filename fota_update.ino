/*
 * 
 * Check update phien ban tren FOTA
 * 
*/



void update_fota(){
    delay(5000);
  HTTPClient http;
  String url = F("http://fota.vn/api/device/");
 url +=  String(ApiKey); url += F("?id="); url += WiFi.macAddress();


  http.begin(url);
//  Serial.println("Register device: " + url);
 // Serial.println("Status code = " + String(http.GET()));

  //1. Check ota service
  char ota_url[128];
  sprintf(ota_url, "%s/api/fota/%s", ApiURL, ApiKey);
 // Serial.println("Checking update, url=" + String(ota_url));
  t_httpUpdate_return ret = ESPhttpUpdate.update(String(ota_url), String(AppVersion));

  switch (ret) {
    case HTTP_UPDATE_FAILED:
    //  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
     // Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
      break;

    case HTTP_UPDATE_OK:
     // Serial.println("HTTP_UPDATE_OK");
      ESP.restart();
      break;
  }
}

