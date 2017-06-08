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
