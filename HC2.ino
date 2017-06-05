const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/* 'Private' declarations */
 void a3_to_a4(unsigned char * a4, unsigned char * a3);
 void a4_to_a3(unsigned char * a3, unsigned char * a4);
 unsigned char b64_lookup(char c);

int base64_encode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];
  while(inputLen--) {
    a3[i++] = *(input++);
    if(i == 3) {
      a3_to_a4(a4, a3);

      for(i = 0; i < 4; i++) {
        output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);
      }

      i = 0;
    }
  }
  if(i) {
    for(j = i; j < 3; j++) {
      a3[j] = '\0';
    }
    a3_to_a4(a4, a3);
    for(j = 0; j < i + 1; j++) {
      output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);
    }
    while((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

int base64_decode(char * output, char * input, int inputLen) {
  int i = 0, j = 0;
  int decLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];
  while (inputLen--) {
    if(*input == '=') {
      break;
    }
    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i <4; i++) {
        a4[i] = b64_lookup(a4[i]);
      }
      a4_to_a3(a3,a4);
      for (i = 0; i < 3; i++) {
        output[decLen++] = a3[i];
      }
      i = 0;
    }
  }
  if (i) {
    for (j = i; j < 4; j++) {
      a4[j] = '\0';
    }
    for (j = 0; j <4; j++) {
      a4[j] = b64_lookup(a4[j]);
    }
    a4_to_a3(a3,a4);
    for (j = 0; j < i - 1; j++) {
      output[decLen++] = a3[j];
    }
  }
  output[decLen] = '\0';
  return decLen;
}

int base64_enc_len(int plainLen) {
  int n = plainLen;
  return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

int base64_dec_len(char * input, int inputLen) {
  int i = 0;
  int numEq = 0;
  for(i = inputLen - 1; input[i] == '='; i--) {
    numEq++;
  }

  return ((6 * inputLen) / 8) - numEq;
}

void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

void a4_to_a3(unsigned char * a3, unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

 unsigned char b64_lookup(char c) {
  if(c >='A' && c <='Z') return c - 'A';
  if(c >='a' && c <='z') return c - 71;
  if(c >='0' && c <='9') return c + 4;
  if(c == '+') return 62;
  if(c == '/') return 63;
  return -1;
}
/////////////////////////////////////////////////////////////////////////
void SetVariHC(String vari,String giatri) {
  String PostData = "{\r\n\"name\": \"\",\r\n\"value\":\"\",\r\n\"invokeScenes\":True\r\n}";
  int vitricat=0;
      for (byte tam=0;tam<sizeof(WiFiConf.sta_passhc);tam++){
            if (WiFiConf.sta_passhc[tam]=='#'){
                  vitricat=tam;
                  break;
            }
      }
      int encodedLen = base64_enc_len(vitricat-1);
      char encoded[encodedLen];
      base64_encode(encoded, WiFiConf.sta_passhc, vitricat);
     // Serial.println(encoded);
      String url=String("PUT /api/globalVariables/")+vari;
      url+= F(" HTTP/1.1");
      String url2="Host: "+String(WiFiConf.sta_iphc2);
      int chieudai=PostData.length()+vari.length()+giatri.length();
      WiFiClient client;
 if (client.connect(WiFiConf.sta_iphc2,80)) {
      client.println(url);
      //Serial.println("OK URL");     
      client.println(url2);
      String url1=F("Authorization: Basic ");
      url1+=String(encoded);
      client.println(url1);
      client.println(F("Content-Type: application/json"));
      client.print(F("Content-Length: "));
      client.println(chieudai);
      client.println();
      client.print(F("{\r\n\"name\": \""));
      client.print(vari);
      client.print(F("\",\r\n\"value\":\""));
      client.print(giatri);
      client.println(F("\",\r\n\"invokeScenes\":true\r\n}"));
      client.stop(); 
}
else{Serial.println(F("Can't connect HC2")); }
}
void getHC() {
  if (!client.connect(WiFiConf.sta_iphc2,80)) {  
     SerialHC2=F("connection failed");      
    return;
  }
  String url = F("/api/settings/info");
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: "+String(WiFiConf.sta_iphc2)+"\r\n" + 
               "Connection: close\r\n\r\n");
  timeout = millis();
  while (client.available() == 0) {
    delay(10);
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      
      SerialHC2=F("HC2 not connected.");
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    delay(5);
    String line = client.readStringUntil('\r');
     if (line.length()>70) {
      line.replace("{","");
      line.replace("}","");
      line.replace("\""," ");
      line.remove(76);
    SerialHC2=line;Serial.println(SerialHC2);}
    
  }
}

