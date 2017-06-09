String writefile(String namefile){
  namefile=namefile+".txt";
  File myFile1 = SD.open(namefile, O_WRITE | O_TRUNC);
  if (myFile1) {
    Serial.print("Writing to test.txt...");
    for (int k = 1; k < results.rawlen; k++) {
                 myFile1.print(results.rawbuf[k]);
                if (k < results.rawlen - 1) myFile1.print(",");
            }
            myFile1.print(",");
            myFile1.print(json_ts);
             myFile1.println("\n");
    myFile1.close();
    Serial.println("done.");
    return (" OK");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return (" Fail");
  }

}
String write_file_setting(){
  //namefile=namefile+".txt";
  File myFile1 = SD.open("Setting/setting.txt", O_WRITE | O_TRUNC);
  if (myFile1) {
    myFile1.print(time_begin_int);
   myFile1.println("\n");
       myFile1.print(time_end_int);
   myFile1.println("\n");
       myFile1.print(temp_set);
   myFile1.println("\n");
          myFile1.print(is_mon);
   myFile1.println("\n");
          myFile1.print(is_tue);
   myFile1.println("\n");
          myFile1.print(is_wed);
   myFile1.println("\n");
          myFile1.print(is_thu);
   myFile1.println("\n");
          myFile1.print(is_fri);
   myFile1.println("\n");
          myFile1.print(is_sat);
   myFile1.println("\n");
          myFile1.print(is_sun);
   myFile1.println("\n");
    myFile1.close();
    Serial.println("done.");
    return (" OK");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return (" Fail");
  }
}
boolean read_file_setting(){
   File myFile1 = SD.open("Setting/setting.txt");
  if (myFile1) {
    char char_tam[30];
    String giatritam="";
    Serial.println("test.txt:");
    int i=1;
    // read from the file until there's nothing else in it:
    //while (myFile1.available()) {
      giatritam=myFile1.readStringUntil('\n');
    giatritam.toCharArray(char_tam, sizeof(char_tam));
      time_begin_int=atoi(char_tam);
            giatritam=myFile1.readStringUntil('\n');
      giatritam=myFile1.readStringUntil('\n');
    giatritam.toCharArray(char_tam, sizeof(char_tam));
      time_end_int=atoi(char_tam);
                  giatritam=myFile1.readStringUntil('\n');
      temp_set=giatritam.toInt();
            giatritam=myFile1.readStringUntil('\n');
      is_mon=giatritam.toInt();
            giatritam=myFile1.readStringUntil('\n');
      is_tue=giatritam.toInt();
            giatritam=myFile1.readStringUntil('\n');
      is_wed=giatritam.toInt();
                  giatritam=myFile1.readStringUntil('\n');
      is_thu=giatritam.toInt();
                  giatritam=myFile1.readStringUntil('\n');
      is_fri=giatritam.toInt();
                  giatritam=myFile1.readStringUntil('\n');
      is_sat=giatritam.toInt();
                  giatritam=myFile1.readStringUntil('\n');
      is_sun=giatritam.toInt();
      i++;      
   // }  
    Serial.println(time_begin_int);
     Serial.println(time_end_int);
      Serial.println(temp_set);
       Serial.println(is_mon);
     Serial.println(is_tue);
     Serial.println(is_wed);
     Serial.println(is_thu);
     Serial.println(is_fri);
     Serial.println(is_sat);
      Serial.println(is_sun);
    // close the file:
    myFile1.close();
    return i ;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return 0;
  }
}



int readfile(String namefile){
  namefile=namefile+".txt";
  // re-open the file for reading:
  File myFile1 = SD.open(namefile);
  if (myFile1) {
    String ir_code_str="";
    Serial.println("test.txt:");
    int i=1;
    // read from the file until there's nothing else in it:
    //while (myFile1.available()) {
      ir_code_str=myFile1.readStringUntil('\n');
      i++;      
   // }
   Serial.println(ir_code_str);
   parseStringRAW(ir_code_str);   
    
    // close the file:
    myFile1.close();
    return i ;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return 0;
  }
 }

 void listfile(){
  myFile = SD.open("/");

  printDirectory(myFile, 0);
 }
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
