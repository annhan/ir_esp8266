


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
    DEBUG_PRINTLN(strtokIndx);
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
    DEBUG_PRINTLN("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }
  DEBUG_PRINT(F("uint16_t rawData["));              // variable type
  DEBUG_PRINT(results->rawlen - 1);  // array size
 // DEBUG_PRINT(results->rawlen - 1,DEC);
  DEBUG_PRINT(F("] = {"));                   // Start declaration
  for (uint16_t i = 1; i < results->rawlen; i++) {
    DEBUG_PRINT(results->rawbuf[i]);
   if (i < results->rawlen - 1) DEBUG_PRINT(F(","));  // ',' not needed on last one
  }
  DEBUG_PRINTLN(F("};"));  //
 /*if (IR_leaning_MQTT){
  IR_leaning_MQTT=false;
  String data = write_file_setting(noiluu_MQTT , 3 );
  noiluu_MQTT="";
 }*/
}
void  dumpCode (decode_results *results)
{ 
 // chieudai = results->rawlen - 1;
 // for (int i = 1;  i < results->rawlen;  i++) {irSignal[i - 1] = results->rawbuf[i];}
  //for (int i = results->rawlen;  i < chieudai_ir;  i++) {irSignal[i - 1] = 0x00;}
}




void serialPrintUint64Hex(uint64_t value) {
  // DEBUG_PRINT() can't handle printing long longs. (uint64_t)
  // So we have to print the top and bottom halves separately.
}


