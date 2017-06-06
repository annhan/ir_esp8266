


/* TĂ�ch TCP
 *  
 */
void Tach_TCP(String str) {
  String strtokIndx; // this is used by strtok() as an index
  int16_t index;
    index = str.indexOf(',', 0);
    strtokIndx = str.substring(0, index);
    str=str.substring(index+1,str.length());
    if (strtokIndx=="sendir" ){
      parseStringGC(str);     
    }
    else{
      parseStringRAW(str);      
    }
    Serial.println(strtokIndx);
}
/* NHáº­n TCP
 *  
 */
void nhan_TCP(){
    client = serverTCP.available();
    if(client) {
      timeout = millis();
       while(!client.available()){
        delay(10);
            if (millis() - timeout > 1000 ) {
                    client.stop();
                    return;
            }
      }
          if (client.available()) {
            String ir_code_str = client.readStringUntil('\r');  // Exclusive of \r
            client.flush();
            client.write("OK");
            Tach_TCP(ir_code_str);
          }
    }
}




void dumpInfo(decode_results *results) {
  if (results->overflow) {
  //  Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }
  Serial.println(F(""));
  Serial.print(F("uint16_t rawData["));              // variable type
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print(F("] = {"));                   // Start declaration
  for (uint16_t i = 1; i < results->rawlen; i++) {
    Serial.print(results->rawbuf[i], DEC);
   if (i < results->rawlen - 1) Serial.print(F(","));  // ',' not needed on last one
  }
  Serial.print(F("};"));  //

}
void  dumpCode (decode_results *results)
{ 
 // chieudai = results->rawlen - 1;
 // for (int i = 1;  i < results->rawlen;  i++) {irSignal[i - 1] = results->rawbuf[i];}
  //for (int i = results->rawlen;  i < chieudai_ir;  i++) {irSignal[i - 1] = 0x00;}
}




void serialPrintUint64Hex(uint64_t value) {
  // Serial.print() can't handle printing long longs. (uint64_t)
  // So we have to print the top and bottom halves separately.
  if (value >> 32)
    Serial.print((uint32_t) (value >> 32), HEX);
  Serial.print((uint32_t) (value & 0xFFFFFFFF), HEX);
}
/*void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results->decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  } else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  serialPrintUint64Hex(results->value);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
 // Serial.print("Raw (");
 // Serial.print(count, DEC);
 // Serial.print("): ");

 // for (uint16_t i = 1; i < count; i++) {
  //  if (i % 100 == 0)
   //   yield();  // Preemptive yield every 100th entry to feed the WDT.
 //   if (i & 1) {
  //    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
  //  } else {
   //   Serial.write('-');
   //   Serial.print((uint32_t) results->rawbuf[i] * USECPERTICK, DEC);
   // }
   // Serial.print(" ");
  //}
 // Serial.println();
}*/

