
String write_file_setting(String namefile, int tt) {
  //namefile=namefile+".txt";
  File myFile1 = SD.open(namefile, O_WRITE | O_TRUNC);
  if (myFile1) {
    switch (tt){
    case 1:
        myFile1.print(HG1.time_begin_int);
        myFile1.println("");
        myFile1.print(HG1.time_end_int);
        myFile1.println("");
        myFile1.print(HG1.temp_set);
        myFile1.println("");
        myFile1.print(HG1.is_mon);
        myFile1.println("");
        myFile1.print(HG1.is_tue);
        myFile1.println("");
        myFile1.print(HG1.is_wed);
        myFile1.println("");
        myFile1.print(HG1.is_thu);
        myFile1.println("");
        myFile1.print(HG1.is_fri);
        myFile1.println("");
        myFile1.print(HG1.is_sat);
        myFile1.println("");
        myFile1.print(HG1.is_sun);
        myFile1.println("");
                myFile1.print(HG2.time_begin_int);
        myFile1.println("");
        myFile1.print(HG2.time_end_int);
        myFile1.println("");
        myFile1.print(HG2.temp_set);
        myFile1.println("");
        myFile1.print(HG2.is_mon);
        myFile1.println("");
        myFile1.print(HG2.is_tue);
        myFile1.println("");
        myFile1.print(HG2.is_wed);
        myFile1.println("");
        myFile1.print(HG2.is_thu);
        myFile1.println("");
        myFile1.print(HG2.is_fri);
        myFile1.println("");
        myFile1.print(HG2.is_sat);
        myFile1.println("");
        myFile1.print(HG2.is_sun);
        myFile1.println("");
                myFile1.print(HG3.time_begin_int);
        myFile1.println("");
        myFile1.print(HG3.time_end_int);
        myFile1.println("");
        myFile1.print(HG3.temp_set);
        myFile1.println("");
        myFile1.print(HG3.is_mon);
        myFile1.println("");
        myFile1.print(HG3.is_tue);
        myFile1.println("");
        myFile1.print(HG3.is_wed);
        myFile1.println("");
        myFile1.print(HG3.is_thu);
        myFile1.println("");
        myFile1.print(HG3.is_fri);
        myFile1.println("");
        myFile1.print(HG3.is_sat);
        myFile1.println("");
        myFile1.print(HG3.is_sun);
        myFile1.println("");
        break;
    case 2:
        myFile1.print(HG1.state_status);
        myFile1.println("");
        myFile1.print(HG1.khoang_time_cach_nhau);
        myFile1.println("");
        myFile1.print(HG1.time_tam_cho_cac_buoc);
        myFile1.println("");
        
                myFile1.print(HG2.state_status);
        myFile1.println("");
        myFile1.print(HG2.khoang_time_cach_nhau);
        myFile1.println("");
        myFile1.print(HG2.time_tam_cho_cac_buoc);
        myFile1.println("");
        
                myFile1.print(HG3.state_status);
        myFile1.println("");
        myFile1.print(HG3.khoang_time_cach_nhau);
        myFile1.println("");
        myFile1.print(HG3.time_tam_cho_cac_buoc);
        myFile1.println("");
        break;
    case 3:
        for (int k = 1; k < results.rawlen; k++) {
          myFile1.print(results.rawbuf[k]);
          if (k < results.rawlen - 1) myFile1.print(",");
        }
        myFile1.print(",");
        myFile1.print(json_ts);
        myFile1.println("\n");
        break;
    }
    myFile1.close();
    NHAN_Debug("done.");
    return (" OK");
  } else {
    // if the file didn't open, print an error:
    NHAN_Debug("error write test.txt");
    return (" Fail");
  }
}
boolean read_file_setting(String namefile, int tt) {
  File myFile1 = SD.open(namefile);
  if (myFile1) {
    char char_tam[30];
    String giatritam = "";
    int i = 1;
    switch (tt){
              case 1:
                  
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG1.time_begin_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG1.time_end_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.temp_set = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_mon = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_tue = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_wed = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_thu = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_fri = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_sat = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.is_sun = giatritam.toInt();

                                    giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG2.time_begin_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG2.time_end_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.temp_set = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_mon = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_tue = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_wed = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_thu = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_fri = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_sat = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.is_sun = giatritam.toInt();

                                    giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG3.time_begin_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG3.time_end_int = atoi(char_tam);
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.temp_set = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_mon = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_tue = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_wed = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_thu = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_fri = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_sat = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.is_sun = giatritam.toInt();
                  i++;
                  // }

                  break;
              case 2:
       
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.state_status = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG1.khoang_time_cach_nhau = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG1.time_tam_cho_cac_buoc = atoi(char_tam);

                  giatritam = myFile1.readStringUntil('\n');
                  HG2.state_status = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG2.khoang_time_cach_nhau = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG2. time_tam_cho_cac_buoc = atoi(char_tam);

                  giatritam = myFile1.readStringUntil('\n');
                  HG3.state_status = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  HG3.khoang_time_cach_nhau = giatritam.toInt();
                  giatritam = myFile1.readStringUntil('\n');
                  giatritam.toCharArray(char_tam, sizeof(char_tam));
                  HG3.time_tam_cho_cac_buoc = atoi(char_tam);
                  break;
              case 3:
                  String ir_code_str = "";
                  NHAN_Debug("test.txt:");
                  ir_code_str = myFile1.readStringUntil('\n');
                  i++;
                  Serial.println(ir_code_str);
                  parseStringRAW(ir_code_str);
                  break;

    } 
    myFile1.close();
    return 1 ;
  } else {
    // if the file didn't open, print an error:
    NHAN_Debug("error opening test.txt");
    return 0;
  }
}

