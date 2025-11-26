String vSendData() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%d,%d,%d",  
  // Arus1, Arus2, Arus3, Arus4, 
  // adc0, adc1, adc2, adc3,
  // SetPointInput1, SetPointInput2, SetPointInput3, SetPointInput4, 
  Watt1, Watt2, Watt3, Watt4, 
  kecepatan1, kecepatan2, kecepatan3, kecepatan4, 
  out_pid1,out_pid2,out_pid3,
  Heater_st_1, Heater_st_2, Heater_st_3,
  Fan_Power_1, Fan_Power_2, Fan_Power_3,
  setpoint1, setpoint2, setpoint3,
  WCSstate1, WCSstate2, WCSstate3
  );
  SData = String(data);
  // Serial.print("WebSet = ");
  // Serial.println(SData);

  return SData;
}

String vSendDataMAC() {
  char dataMAC[1000];  // buffer besar biar cukup

  sprintf(dataMAC,
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam1
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam2
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam3
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam4
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam5
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam6
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDalam7
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorUjung1
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorUjung2
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // heater1
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // heater2
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // heater3
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDinding1
    "%02X:%02X:%02X:%02X:%02X:%02X,"   // sensorDinding2
    "%02X:%02X:%02X:%02X:%02X:%02X",   // sensorDinding3
    sensorDalam1[0], sensorDalam1[1], sensorDalam1[2], sensorDalam1[3], sensorDalam1[4], sensorDalam1[5],
    sensorDalam2[0], sensorDalam2[1], sensorDalam2[2], sensorDalam2[3], sensorDalam2[4], sensorDalam2[5],
    sensorDalam3[0], sensorDalam3[1], sensorDalam3[2], sensorDalam3[3], sensorDalam3[4], sensorDalam3[5],
    sensorDalam4[0], sensorDalam4[1], sensorDalam4[2], sensorDalam4[3], sensorDalam4[4], sensorDalam4[5],
    sensorDalam5[0], sensorDalam5[1], sensorDalam5[2], sensorDalam5[3], sensorDalam5[4], sensorDalam5[5],
    sensorDalam6[0], sensorDalam6[1], sensorDalam6[2], sensorDalam6[3], sensorDalam6[4], sensorDalam6[5],
    sensorDalam7[0], sensorDalam7[1], sensorDalam7[2], sensorDalam7[3], sensorDalam7[4], sensorDalam7[5],
    sensorUjung1[0], sensorUjung1[1], sensorUjung1[2], sensorUjung1[3], sensorUjung1[4], sensorUjung1[5],
    sensorUjung2[0], sensorUjung2[1], sensorUjung2[2], sensorUjung2[3], sensorUjung2[4], sensorUjung2[5],
    heater1[0], heater1[1], heater1[2], heater1[3], heater1[4], heater1[5],
    heater2[0], heater2[1], heater2[2], heater2[3], heater2[4], heater2[5],
    heater3[0], heater3[1], heater3[2], heater3[3], heater3[4], heater3[5],
    sensorDinding1[0], sensorDinding1[1], sensorDinding1[2], sensorDinding1[3], sensorDinding1[4], sensorDinding1[5],
    sensorDinding2[0], sensorDinding2[1], sensorDinding2[2], sensorDinding2[3], sensorDinding2[4], sensorDinding2[5],
    sensorDinding3[0], sensorDinding3[1], sensorDinding3[2], sensorDinding3[3], sensorDinding3[4], sensorDinding3[5]
  );

  String SDataMAC = String(dataMAC);

  // Serial.print("WebSet = ");
  // Serial.println(SDataMAC);

  return SDataMAC;
}


String vSendDataHeat() {
  char data[1000];  // Sesuaikan ukuran buffer jika perlu

  DindSuhu1 = SuhuDS1;
  DindSuhu2 = SuhuDS2;
  DindSuhu3 = SuhuDS3;

  sprintf(data, "%d,%.5f,%d,%d,%d,%.5f,%d,%d,%d,%.5f,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.2f,%.2f,%.2f,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%d,%d,%d,%.5f,%.5f,%.5f,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f", 
  Suhu_Tunnel_1, out_pid1, Heater_st_1, Fan_Power_1, 
  Suhu_Tunnel_2, out_pid2, Heater_st_2, Fan_Power_2, 
  Suhu_Tunnel_3, out_pid3, Heater_st_3, Fan_Power_3, 
  CalM_DinD1, CalB_DinD1, 
  CalM_DinD2, CalB_DinD2, 
  CalM_DinD3, CalB_DinD3, 
  DindSuhu1, DindSuhu2, DindSuhu3, 
  WCSstate1, WCSstate2, WCSstate3,
  setpoint1, setpoint2, setpoint3,
  kp1, kp2, kp3,
  ki1, ki2, ki3,
  kd1, kd2, kd3,
  Power_Tunnel_1, Power_Tunnel_2, Power_Tunnel_3,
  out_pid1, out_pid2, out_pid3,
  Max_PID_Tunnel_1,Max_PID_Tunnel_2,Max_PID_Tunnel_3,
  calM_Wcs1,calB_Wcs1,
  calM_Wcs2,calB_Wcs2,
  calM_Wcs3,calB_Wcs3,
  kpFan1,kiFan1,kdFan1,
  kpFan2,kiFan2,kdFan2
  );
  SDataHeat = String(data);
  // Serial.print("SDataHeat = ");
  // Serial.println(SDataHeat);

  return SDataHeat;
}

void stringToMacArray(const String &macStr, uint8_t *macArr) {
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", 
            &values[0], &values[1], &values[2], 
            &values[3], &values[4], &values[5]) == 6) {
    for (int i = 0; i < 6; ++i) {
      macArr[i] = (uint8_t) values[i];
    }
  }
}

void vAsyncWebServer() {

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", index_html);
  // });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", controling_html);
  // });


  server.serveStatic("/", LittleFS, "/").setDefaultFile("user.html");
  

  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/controling.html", "text/html"); });

  server.on("/MAC", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/mac.html", "text/html"); });

  server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });


  // server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  // server.serveStatic("/data.js", LittleFS, "/data.js");
  // server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  // server.serveStatic("/accessibility.js", LittleFS, "/accessibility.js");
  // server.serveStatic("/export-data.js", LittleFS, "/export-data.js");
  // server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");
  
  // server.on("/SuhuData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/SuhuData.js", "text/javascript");
  // });
  // server.on("/HumidData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/HumidData.js", "text/javascript");
  // });
  // server.on("/WindData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/WindData.js", "text/javascript");
  // });
  // server.on("/TekananData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/TekananData.js", "text/javascript");
  // });
  // server.on("/SuhuPipaData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/SuhuPipaData.js", "text/javascript");
  // });
  // server.on("/SuhuDindingData.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/SuhuDindingData.js", "text/javascript");
  // });
  // server.on("/OutPID.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/OutPID.js", "text/javascript");
  // });
  // server.on("/GabunganSuhu.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SD, "/GabunganSuhu.js", "text/javascript");
  // });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){

      String Dsuhu1 = String(SuhuDS1);
      String Dsuhu2 = String(SuhuDS2);
      String Dsuhu3 = String(SuhuDS3);

      String SBME1 = String(BME1);
      String SBME2 = String(BME2);
      String SBME3 = String(BME3);
      


      // String json = "{";
      // json += "\"suhu1\":" + String(suhu1, 2)+ ",";
      // json += "\"suhu2\":" + String(suhu2, 2)+ ",";
      // json += "\"suhu3\":" + String(suhu3, 2)+ ",";
      // json += "\"suhu4\":" + String(suhu4, 2)+ ",";
      // json += "\"suhu5\":" + String(suhu5, 2)+ ",";
      // json += "\"suhu6\":" + String(suhu6, 2)+ ",";
      // json += "\"suhu7\":" + String(suhu7, 2)+ ",";

      // json += "\"kec1\":" + String(kec1, 2)+ ",";
      // json += "\"kec2\":" + String(kec2, 2)+ ",";
      // json += "\"kec3\":" + String(kec3, 2)+ ",";
      // json += "\"kec4\":" + String(kec4, 2)+ ",";
      // json += "\"kec5\":" + String(kec5, 2)+ ",";
      // json += "\"kec6\":" + String(kec6, 2)+ ",";
      // json += "\"kec7\":" + String(kec7, 2);

      String json = "{";
      json += "\"time\":" + String(timeUpdate) + "000" + ",";
      json += "\"suhu1\":" + safeValue(suhu1) + ",";
      json += "\"suhu2\":" + safeValue(suhu2) + ",";
      json += "\"suhu3\":" + safeValue(suhu3) + ",";
      json += "\"suhu4\":" + safeValue(suhu4) + ",";
      json += "\"suhu5\":" + safeValue(suhu5) + ",";
      json += "\"suhu6\":" + safeValue(suhu6) + ",";
      json += "\"suhu7\":" + safeValue(suhu7) + ",";

      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"Psuhu1\":" + safeValue(SBME1)+ ",";
      json += "\"Psuhu2\":" + safeValue(SBME2)+ ",";
      json += "\"Psuhu3\":" + safeValue(SBME3)+ ",";

      json += "\"Humid1\":" + safeValue(Humid1)+ ",";
      json += "\"Humid2\":" + safeValue(Humid2)+ ",";
      json += "\"Humid3\":" + safeValue(Humid3)+ ",";
      json += "\"Humid4\":" + safeValue(Humid4)+ ",";
      json += "\"Humid5\":" + safeValue(Humid5)+ ",";
      json += "\"Humid6\":" + safeValue(Humid6)+ ",";
      json += "\"Humid7\":" + safeValue(Humid7)+ ",";
      json += "\"UHumid1\":" + safeValue(UHumid1)+ ",";
      json += "\"UHumid2\":" + safeValue(UHumid2)+ ",";

      json += "\"kec1\":" + safeValue(kec1)+ ",";
      json += "\"kec2\":" + safeValue(kec2)+ ",";
      json += "\"kec3\":" + safeValue(kec3)+ ",";
      json += "\"kec4\":" + safeValue(kec4)+ ",";
      json += "\"kec5\":" + safeValue(kec5)+ ",";
      json += "\"kec6\":" + safeValue(kec6)+ ",";
      json += "\"kec7\":" + safeValue(kec7)+ ",";

      json += "\"Usuhu1\":" + safeValue(Usuhu1)+ ",";
      json += "\"Usuhu2\":" + safeValue(Usuhu2)+ ",";
      json += "\"Ukec1\":" + safeValue(Ukec1)+ ",";
      json += "\"Ukec2\":" + safeValue(Ukec2)+ ",";
      json += "\"USDP1\":" + safeValue(USDP1)+ ",";
      json += "\"USDP2\":" + safeValue(USDP2);

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

      // for (int j = 0; j < 22; j++) {
      //   data[j] = "";  // Clear for next cycle
      // }
      reset_arr = 0;
      Serial.println(xPortGetCoreID());
    });

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){

      // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
      // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
      // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;

      String Dsuhu1 = String(SuhuDS1);
      String Dsuhu2 = String(SuhuDS2);
      String Dsuhu3 = String(SuhuDS3);

      String json = "{";
      
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"BME1\":" + safeValue(String(BME1))+ ",";
      json += "\"BME2\":" + safeValue(String(BME2))+ ",";
      json += "\"BME3\":" + safeValue(String(BME3));

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

    });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("In_Watt_Max1")) {
      PWM1 = request->getParam("In_Watt_Max1")->value().toInt();
      Serial.print("PWM1 = ");
      Serial.println(PWM1);
      servo1.write(PWM1);
      kecepatan1 = PWM1;
      preferences.putInt("PWM1", PWM1);

    }

    if (request->hasParam("In_Watt_Max2")) {
      PWM2 = request->getParam("In_Watt_Max2")->value().toInt();
      Serial.print("PWM2 = ");
      Serial.println(PWM2);
      servo2.write(PWM2);
      kecepatan2 = PWM2;
      preferences.putInt("PWM2", PWM2);
    }

    if (request->hasParam("In_Watt_Max3")) {
      PWM3 = request->getParam("In_Watt_Max3")->value().toInt();
      Serial.print("PWM3 = ");
      Serial.println(PWM3);
      servo3.write(PWM3);
      kecepatan3 = PWM3;
    }

    if (request->hasParam("In_Watt_Max4")) {
      PWM4 = request->getParam("In_Watt_Max4")->value().toInt();
      Serial.print("PWM4 = ");
      Serial.println(PWM4);
      servo4.write(PWM4);
      kecepatan4 = PWM4;
    }

    if (request->hasParam("In_Rref")) {
      Safety = request->getParam("In_Rref")->value().toInt();
      Serial.print("Safety :");
      Serial.println(Safety);
      // if(Safety == 1){
      //   servo1.write(30);
      //   servo2.write(30);
      //   servo3.write(30);
      //   servo4.write(30);
      // }
      // else{
      //   servo1.write(0);
      //   servo2.write(0);
      //   servo3.write(0);
      //   servo4.write(0);
      // }
    }


    if (request->hasParam("In_Lock_1")) {
      Heater_st_1 = request->getParam("In_Lock_1")->value().toInt();
      Serial.print("Heater_st_1 :");
      Serial.println(Heater_st_1);
      
      if(Heater_st_1 == 1){
        String Perintah = "KIRIM HEATER 1 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 1 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power1")) {
      Fan_Power_1 = request->getParam("In_Fan_Power1")->value().toInt();
      Serial.print("Fan_Power_1 :");
      Serial.println(Fan_Power_1);
      if(Fan_Power_1 == 1){
        String Perintah = "KIRIM HEATER 1 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 1 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel1")) {
      Power_Tunnel_1 = request->getParam("In_Power_Tunnel1")->value().toInt();
      Serial.print("Power_Tunnel_1 :");
      Serial.println(Power_Tunnel_1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 Power_Tunnel = %d", Power_Tunnel_1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_PID_Tunnel1")) {
      Max_PID_Tunnel_1 = request->getParam("In_PID_Tunnel1")->value().toInt();
      Serial.print("Max_PID_Tunnel_1 :");
      Serial.println(Max_PID_Tunnel_1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 Max_PID_Tunnel = %d", Max_PID_Tunnel_1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    if (request->hasParam("In_Suhu_Tunnel1")) {
      Suhu_Tunnel_1 = request->getParam("In_Suhu_Tunnel1")->value().toInt();
      Serial.print("Suhu_Tunnel_1 :");
      Serial.println(Suhu_Tunnel_1);
      preferences.putInt("Suhu_Tunnel", Suhu_Tunnel_1);
    }

    if (request->hasParam("In_sp1")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_sp1")->value().toFloat();
      Serial.print("setPointTemp1 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 Set_Point = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kp1")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kp1")->value().toFloat();
      Serial.print("KP1 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 NILAI_KP = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_ki1")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_ki1")->value().toFloat();
      Serial.print("KI1 :");
      Serial.println(setPointTemp1, 5);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 NILAI_KI = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kd1")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kd1")->value().toFloat();
      Serial.print("KD1 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 NILAI_KD = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Lock_2")) {
      Heater_st_2 = request->getParam("In_Lock_2")->value().toInt();
      Serial.print("Heater_st_2 :");
      Serial.println(Heater_st_2);
      
      if(Heater_st_2 == 1){
        String Perintah = "KIRIM HEATER 2 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 2 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power2")) {
      Fan_Power_2 = request->getParam("In_Fan_Power2")->value().toInt();
      Serial.print("Fan_Power_2 :");
      Serial.println(Fan_Power_2);
      if(Fan_Power_2 == 1){
        String Perintah = "KIRIM HEATER 2 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 2 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel2")) {
      Power_Tunnel_2 = request->getParam("In_Power_Tunnel2")->value().toInt();
      Serial.print("Power_Tunnel_2 :");
      Serial.println(Power_Tunnel_2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 Power_Tunnel = %d", Power_Tunnel_2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_PID_Tunnel2")) {
      Max_PID_Tunnel_2 = request->getParam("In_PID_Tunnel2")->value().toInt();
      Serial.print("Max_PID_Tunnel_2 :");
      Serial.println(Max_PID_Tunnel_2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 Max_PID_Tunnel = %d", Max_PID_Tunnel_2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    if (request->hasParam("In_Suhu_Tunnel2")) {
      Suhu_Tunnel_2 = request->getParam("In_Suhu_Tunnel2")->value().toInt();
      Serial.print("Suhu_Tunnel_2 :");
      Serial.println(Suhu_Tunnel_2);
      preferences.putInt("Suhu_Tunnel_2", Suhu_Tunnel_2);
    }

    if (request->hasParam("In_sp2")) {
      float setPointTemp2;
      setPointTemp2 = request->getParam("In_sp2")->value().toFloat();
      Serial.print("setPointTemp2 :");
      Serial.println(setPointTemp2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 Set_Point = %.5f", setPointTemp2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kp2")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kp2")->value().toFloat();
      Serial.print("KP2 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 NILAI_KP = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_ki2")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_ki2")->value().toFloat();
      Serial.print("KI2 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 NILAI_KI = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kd2")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kd2")->value().toFloat();
      Serial.print("KD2 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 NILAI_KD = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Lock_3")) {
      Heater_st_3 = request->getParam("In_Lock_3")->value().toInt();
      Serial.print("Heater_st_3 :");
      Serial.println(Heater_st_3);
      
      if(Heater_st_3 == 1){
        String Perintah = "KIRIM HEATER 3 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 3 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power3")) {
      Fan_Power_3 = request->getParam("In_Fan_Power3")->value().toInt();
      Serial.print("Fan_Power_3 :");
      Serial.println(Fan_Power_3);
      if(Fan_Power_3 == 1){
        String Perintah = "KIRIM HEATER 3 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 3 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel3")) {
      Power_Tunnel_3 = request->getParam("In_Power_Tunnel3")->value().toInt();
      Serial.print("Power_Tunnel_3 :");
      Serial.println(Power_Tunnel_3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 Power_Tunnel = %d", Power_Tunnel_3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_PID_Tunnel3")) {
      Max_PID_Tunnel_3 = request->getParam("In_PID_Tunnel3")->value().toInt();
      Serial.print("Max_PID_Tunnel_3 :");
      Serial.println(Max_PID_Tunnel_3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 Max_PID_Tunnel = %d", Max_PID_Tunnel_3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    if (request->hasParam("In_Suhu_Tunnel3")) {
      Suhu_Tunnel_3 = request->getParam("In_Suhu_Tunnel3")->value().toInt();
      Serial.print("Suhu_Tunnel_3 :");
      Serial.println(Suhu_Tunnel_3);
      preferences.putInt("Suhu_Tunnel_3", Suhu_Tunnel_3);
    }

    if (request->hasParam("In_sp3")) {
      float setPointTemp3;
      setPointTemp3 = request->getParam("In_sp3")->value().toFloat();
      Serial.print("setPointTemp3 :");
      Serial.println(setPointTemp3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 Set_Point = %.5f", setPointTemp3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kp3")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kp3")->value().toFloat();
      Serial.print("KP3 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 NILAI_KP = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_ki3")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_ki3")->value().toFloat();
      Serial.print("KI3 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 NILAI_KI = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_kd3")) {
      float setPointTemp1;
      setPointTemp1 = request->getParam("In_kd3")->value().toFloat();
      Serial.print("KD3 :");
      Serial.println(setPointTemp1);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 NILAI_KD = %.5f", setPointTemp1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (request->hasParam("In_CalM_DinD1")) {
      CalM_DinD1 = request->getParam("In_CalM_DinD1")->value().toFloat();
      Serial.print("CalM_DinD1 = ");
      Serial.println(CalM_DinD1);
      // preferences.putFloat("CalM_DinD1", CalM_DinD1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 NILAI_CalM = %.5f", CalM_DinD1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }
    if (request->hasParam("In_CalB_DinD1")) {
      CalB_DinD1 = request->getParam("In_CalB_DinD1")->value().toFloat();
      Serial.print("CalB_DinD1 = ");
      Serial.println(CalB_DinD1);
      // preferences.putFloat("CalB_DinD1", CalB_DinD1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 NILAI_CalB = %.5f", CalB_DinD1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    if (request->hasParam("In_CalM_DinD2")) {
      CalM_DinD2 = request->getParam("In_CalM_DinD2")->value().toFloat();
      Serial.print("CalM_DinD2 = ");
      Serial.println(CalM_DinD2);
      // preferences.putFloat("CalM_DinD2", CalM_DinD2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 NILAI_CalM = %.5f", CalM_DinD2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }
    if (request->hasParam("In_CalB_DinD2")) {
      CalB_DinD2 = request->getParam("In_CalB_DinD2")->value().toFloat();
      Serial.print("CalB_DinD2 = ");
      Serial.println(CalB_DinD2);
      // preferences.putFloat("CalB_DinD2", CalB_DinD2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 NILAI_CalB = %.5f", CalB_DinD2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    if (request->hasParam("In_CalM_DinD3")) {
      CalM_DinD3 = request->getParam("In_CalM_DinD3")->value().toFloat();
      Serial.print("CalM_DinD3 = ");
      Serial.println(CalM_DinD3);
      // preferences.putFloat("CalM_DinD3", CalM_DinD3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 NILAI_CalM = %.5f", CalM_DinD3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }
    if (request->hasParam("In_CalB_DinD3")) {
      CalB_DinD3 = request->getParam("In_CalB_DinD3")->value().toFloat();
      Serial.print("CalB_DinD3 = ");
      Serial.println(CalB_DinD3);
      // preferences.putFloat("CalB_DinD3", CalB_DinD3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 NILAI_CalB = %.5f", CalB_DinD3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
    }

    ////////////////////////////////////////////////////////////////////
    
    if (request->hasParam("In_CalM_Wcs1")) {
      calM_Wcs1 = request->getParam("In_CalM_Wcs1")->value().toFloat();
      Serial.print("calM_Wcs1 = ");
      Serial.println(calM_Wcs1);
      preferences.putFloat("calM_Wcs1", calM_Wcs1);
    }
    if (request->hasParam("In_CalB_Wcs1")) {
      calB_Wcs1 = request->getParam("In_CalB_Wcs1")->value().toFloat();
      Serial.print("calB_Wcs1 = ");
      Serial.println(calB_Wcs1);
      preferences.putFloat("calB_Wcs1", calB_Wcs1);
    }

    if (request->hasParam("In_kpFan1")) {
      kpFan1 = request->getParam("In_kpFan1")->value().toFloat();
      Serial.print("kpFan1 = ");
      Serial.println(kpFan1);
      preferences.putFloat("kpFan1", kpFan1);
    }

    if (request->hasParam("In_kiFan1")) {
      kiFan1 = request->getParam("In_kiFan1")->value().toFloat();
      Serial.print("kiFan1 = ");
      Serial.println(kiFan1);
      preferences.putFloat("kiFan1", kiFan1);
    }

    if (request->hasParam("In_kdFan1")) {
      kdFan1 = request->getParam("In_kdFan1")->value().toFloat();
      Serial.print("kdFan1 = ");
      Serial.println(kdFan1);
      preferences.putFloat("kdFan1", kdFan1);
    }


    if (request->hasParam("In_CalM_Wcs2")) {
      calM_Wcs2 = request->getParam("In_CalM_Wcs2")->value().toFloat();
      Serial.print("calM_Wcs2 = ");
      Serial.println(calM_Wcs2);
      preferences.putFloat("calM_Wcs2", calM_Wcs2);
    }
    if (request->hasParam("In_CalB_Wcs2")) {
      calB_Wcs2 = request->getParam("In_CalB_Wcs2")->value().toFloat();
      Serial.print("calB_Wcs2 = ");
      Serial.println(calB_Wcs2);
      preferences.putFloat("calB_Wcs2", calB_Wcs2);
    }

    if (request->hasParam("In_kpFan2")) {
      kpFan2 = request->getParam("In_kpFan2")->value().toFloat();
      Serial.print("kpFan2 = ");
      Serial.println(kpFan2);
      preferences.putFloat("kpFan2", kpFan2);
    }

    if (request->hasParam("In_kiFan2")) {
      kiFan2 = request->getParam("In_kiFan2")->value().toFloat();
      Serial.print("kiFan2 = ");
      Serial.println(kiFan2);
      preferences.putFloat("kiFan2", kiFan2);
    }

    if (request->hasParam("In_kdFan2")) {
      kdFan2 = request->getParam("In_kdFan2")->value().toFloat();
      Serial.print("kdFan2 = ");
      Serial.println(kdFan2);
      preferences.putFloat("kdFan2", kdFan2);
    }

    if (request->hasParam("In_CalM_Wcs3")) {
      calM_Wcs3 = request->getParam("In_CalM_Wcs3")->value().toFloat();
      Serial.print("calM_Wcs3 = ");
      Serial.println(calM_Wcs3);
      preferences.putFloat("calM_Wcs3", calM_Wcs3);
    }
    if (request->hasParam("In_CalB_Wcs3")) {
      calB_Wcs3 = request->getParam("In_CalB_Wcs3")->value().toFloat();
      Serial.print("calB_Wcs3 = ");
      Serial.println(calB_Wcs3);
      preferences.putFloat("calB_Wcs3", calB_Wcs3);
    }

    if (request->hasParam("In_CalM_Wcs4")) {
      calM_Wcs4 = request->getParam("In_CalM_Wcs4")->value().toFloat();
      Serial.print("calM_Wcs4 = ");
      Serial.println(calM_Wcs4);
      preferences.putFloat("calM_Wcs4", calM_Wcs4);
    }
    if (request->hasParam("In_CalB_Wcs4")) {
      calB_Wcs4 = request->getParam("In_CalB_Wcs4")->value().toFloat();
      Serial.print("calB_Wcs4 = ");
      Serial.println(calB_Wcs4);
      preferences.putFloat("calB_Wcs4", calB_Wcs4);
    }
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (request->hasParam("mac_tunnel1")) {
      String macStr = request->getParam("mac_tunnel1")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam1);

      // tampilkan hasil
      Serial.print("sensorDalam1[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam1[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm1", &sensorDalam1, sizeof(sensorDalam1));
    }

    if (request->hasParam("mac_tunnel2")) {
      String macStr = request->getParam("mac_tunnel2")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam2);

      // tampilkan hasil
      Serial.print("sensorDalam2[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam2[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm2", &sensorDalam2, sizeof(sensorDalam2));
    }

    if (request->hasParam("mac_tunnel3")) {
      String macStr = request->getParam("mac_tunnel3")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam3);

      // tampilkan hasil
      Serial.print("sensorDalam3[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam3[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm3", &sensorDalam3, sizeof(sensorDalam3));
    }

    if (request->hasParam("mac_tunnel4")) {
      String macStr = request->getParam("mac_tunnel4")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam4);

      // tampilkan hasil
      Serial.print("sensorDalam4[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam4[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm4", &sensorDalam4, sizeof(sensorDalam4));
    }

    if (request->hasParam("mac_tunnel5")) {
      String macStr = request->getParam("mac_tunnel5")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam5);

      // tampilkan hasil
      Serial.print("sensorDalam5[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam5[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm5", &sensorDalam5, sizeof(sensorDalam5));
    }

    if (request->hasParam("mac_tunnel6")) {
      String macStr = request->getParam("mac_tunnel6")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam6);

      // tampilkan hasil
      Serial.print("sensorDalam6[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam6[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm6", &sensorDalam6, sizeof(sensorDalam6));
    }

        if (request->hasParam("mac_tunnel7")) {
      String macStr = request->getParam("mac_tunnel7")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDalam7);

      // tampilkan hasil
      Serial.print("sensorDalam7[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDalam7[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensDlm7", &sensorDalam7, sizeof(sensorDalam7));
    }

    if (request->hasParam("mac_pipe1")) {
      String macStr = request->getParam("mac_pipe1")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorUjung1);

      // tampilkan hasil
      Serial.print("sensorUjung1[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorUjung1[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensUjn1", &sensorUjung1, sizeof(sensorUjung1));
    }

    if (request->hasParam("mac_pipe2")) {
      String macStr = request->getParam("mac_pipe2")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorUjung2);

      // tampilkan hasil
      Serial.print("sensorUjung2[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorUjung2[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("SensUjn2", &sensorUjung2, sizeof(sensorUjung2));
    }

    if (request->hasParam("mac_heater1")) {
      String macStr = request->getParam("mac_heater1")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, heater1);

      // tampilkan hasil
      Serial.print("heater1[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", heater1[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Heater1", &heater1, sizeof(heater1));
    }

    if (request->hasParam("mac_heater2")) {
      String macStr = request->getParam("mac_heater2")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, heater2);

      // tampilkan hasil
      Serial.print("heater2[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", heater2[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Heater2", &heater2, sizeof(heater2));
    }

    if (request->hasParam("mac_heater3")) {
      String macStr = request->getParam("mac_heater3")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, heater3);

      // tampilkan hasil
      Serial.print("heater3[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", heater3[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Heater3", &heater3, sizeof(heater3));
    }

    if (request->hasParam("mac_wall1")) {
      String macStr = request->getParam("mac_wall1")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDinding1);

      // tampilkan hasil
      Serial.print("sensorDinding1[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDinding1[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Dindng1", &sensorDinding1, sizeof(sensorDinding1));
    }

    if (request->hasParam("mac_wall2")) {
      String macStr = request->getParam("mac_wall2")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDinding2);

      // tampilkan hasil
      Serial.print("sensorDinding2[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDinding2[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Dindng2", &sensorDinding2, sizeof(sensorDinding2));
    }

    if (request->hasParam("mac_wall3")) {
      String macStr = request->getParam("mac_wall3")->value();
      Serial.print("MAC input: ");
      Serial.println(macStr);

      // konversi ke array uint8_t
      stringToMacArray(macStr, sensorDinding3);

      // tampilkan hasil
      Serial.print("sensorDinding3[] = { ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("0x%02X", sensorDinding3[i]);
        if (i < 5) Serial.print(", ");
      }
      Serial.println(" };");
      preferences.putBytes("Dindng3", &sensorDinding3, sizeof(sensorDinding3));
    }
    
    request->send(204);
  });

  server.on("/dataSet", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendData().c_str());
  });

  server.on("/dataSetHeat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendDataHeat().c_str());
  });

  server.on("/dataMAC", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendDataMAC().c_str());
  });

  server.on("/updateServer", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("SEDANG PROSES OTA");
    Serial.println("updateFirmware");
    // Update_OTA = 1;
    
    // updateFirmware();
    request->send(200);
  });

  server.on("/resetSD", HTTP_GET, [](AsyncWebServerRequest *request){
    bool success = false;
    vAwalSdCard();
    success = true;
    if (success) {
      request->send(200, "text/plain", "SD Card berhasil direset");
    } else {
      request->send(500, "text/plain", "Gagal mereset SD Card");
    }
  });


  //  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });


  server.begin();
}