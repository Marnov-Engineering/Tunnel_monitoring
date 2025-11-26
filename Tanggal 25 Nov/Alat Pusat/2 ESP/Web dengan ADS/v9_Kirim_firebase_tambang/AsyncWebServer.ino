String vSendData() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%d,%d,%d,%d", Safety, Watt1, Watt2, Watt3, Watt4, kecepatan1, kecepatan2, kecepatan3, kecepatan4, calM_Wcs1, calM_Wcs2, calM_Wcs3, calM_Wcs4, calB_Wcs1, calB_Wcs2, calB_Wcs3, calB_Wcs4, Arus1, Arus2, Arus3, Arus4);
  SData = String(data);
  Serial.print("WebSet = ");
  Serial.println(SData);

  return SData;
}

String vSendDataHeat() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu

  DindSuhu1 = SuhuDS1;
  DindSuhu2 = SuhuDS2;
  DindSuhu3 = SuhuDS3;

  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.2f,%.2f,%.2f,%d,%d,%d", Suhu_Tunnel_1, Power_Tunnel_1, Heater_st_1, Fan_Power_1, Suhu_Tunnel_2, Power_Tunnel_2, Heater_st_2, Fan_Power_2, Suhu_Tunnel_3, Power_Tunnel_3, Heater_st_3, Fan_Power_3, CalM_DinD1, CalB_DinD1, CalM_DinD2, CalB_DinD2, CalM_DinD3, CalB_DinD3, DindSuhu1, DindSuhu2, DindSuhu3, WCSstate1, WCSstate2, WCSstate3);
  SDataHeat = String(data);
  Serial.print("SDataHeat = ");
  Serial.println(SDataHeat);

  return SDataHeat;
}

void vAsyncWebServer() {

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", index_html);
  // });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", controling_html);
  // });


  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/controling.html", "text/html"); });


  server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  server.serveStatic("/data.js", LittleFS, "/data.js");
  server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");
  
  server.on("/SuhuData.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/SuhuData.js", "text/javascript");
  });
  server.on("/WindData.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/WindData.js", "text/javascript");
  });
  server.on("/TekananData.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/TekananData.js", "text/javascript");
  });
  server.on("/SuhuPipaData.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/SuhuPipaData.js", "text/javascript");
  });
  server.on("/SuhuDindingData.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/SuhuDindingData.js", "text/javascript");
  });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
      suhu1 = data[0];
      kec1 = data[1];
      suhu2 = data[2];
      kec2 = data[3];
      suhu3 = data[4];
      kec3 = data[5];
      suhu4 = data[6];
      kec4 =  data[7];
      suhu5 = data[8];
      kec5 = data[9];
      suhu6 = data[10];
      kec6 = data[11];
      suhu7 = data[12];
      kec7 = data[13];

      Usuhu1 = data[14];
      Ukec1 = data[15];
      USDP1 = data[16];
      Usuhu2 = data[17];
      Ukec2 = data[18];
      USDP2 = data[19];


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
      json += "\"suhu1\":" + safeValue(suhu1) + ",";
      json += "\"suhu2\":" + safeValue(suhu2) + ",";
      json += "\"suhu3\":" + safeValue(suhu3) + ",";
      json += "\"suhu4\":" + safeValue(suhu4) + ",";
      json += "\"suhu5\":" + safeValue(suhu5) + ",";
      json += "\"suhu6\":" + safeValue(suhu6) + ",";
      json += "\"suhu7\":" + safeValue(suhu7) + ",";

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
      Watt1 = request->getParam("In_Watt_Max1")->value().toInt();
      Serial.print("Watt1 = ");
      Serial.println(Watt1);
      servo1.write(Watt1);
    }

    if (request->hasParam("In_Watt_Max2")) {
      Watt2 = request->getParam("In_Watt_Max2")->value().toInt();
      Serial.print("Watt2 = ");
      Serial.println(Watt2);
      servo2.write(Watt2);
    }

    if (request->hasParam("In_Watt_Max3")) {
      Watt3 = request->getParam("In_Watt_Max3")->value().toInt();
      Serial.print("Watt3 = ");
      Serial.println(Watt3);
      servo3.write(Watt3);
    }

    if (request->hasParam("In_Watt_Max4")) {
      Watt4 = request->getParam("In_Watt_Max4")->value().toInt();
      Serial.print("Watt4 = ");
      Serial.println(Watt4);
      servo4.write(Watt4);
    }

    if (request->hasParam("In_Rref")) {
      Safety = request->getParam("In_Rref")->value().toInt();
      Serial.print("Safety :");
      Serial.println(Safety);
      if(Safety == 1){
        servo1.write(30);
        servo2.write(30);
        servo3.write(30);
        servo4.write(30);
      }
      else{
        servo1.write(0);
        servo2.write(0);
        servo3.write(0);
        servo4.write(0);
      }
    }


    if (request->hasParam("In_Heat_st1")) {
      Heater_st_1 = request->getParam("In_Heat_st1")->value().toInt();
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

    if (request->hasParam("In_Suhu_Tunnel1")) {
      Suhu_Tunnel_1 = request->getParam("In_Suhu_Tunnel1")->value().toInt();
      Serial.print("Suhu_Tunnel_1 :");
      Serial.println(Suhu_Tunnel_1);
      preferences.putInt("Suhu_Tunnel", Suhu_Tunnel_1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Heat_st2")) {
      Heater_st_2 = request->getParam("In_Heat_st2")->value().toInt();
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

    if (request->hasParam("In_Suhu_Tunnel2")) {
      Suhu_Tunnel_2 = request->getParam("In_Suhu_Tunnel2")->value().toInt();
      Serial.print("Suhu_Tunnel_2 :");
      Serial.println(Suhu_Tunnel_2);
      preferences.putInt("Suhu_Tunnel_2", Suhu_Tunnel_2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Heat_st3")) {
      Heater_st_3 = request->getParam("In_Heat_st3")->value().toInt();
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

    if (request->hasParam("In_Suhu_Tunnel3")) {
      Suhu_Tunnel_3 = request->getParam("In_Suhu_Tunnel3")->value().toInt();
      Serial.print("Suhu_Tunnel_3 :");
      Serial.println(Suhu_Tunnel_3);
      preferences.putInt("Suhu_Tunnel_3", Suhu_Tunnel_3);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (request->hasParam("In_CalM_DinD1")) {
      CalM_DinD1 = request->getParam("In_CalM_DinD1")->value().toFloat();
      Serial.print("CalM_DinD1 = ");
      Serial.println(CalM_DinD1);
      preferences.putFloat("CalM_DinD1", CalM_DinD1);
    }
    if (request->hasParam("In_CalB_DinD1")) {
      CalB_DinD1 = request->getParam("In_CalB_DinD1")->value().toFloat();
      Serial.print("CalB_DinD1 = ");
      Serial.println(CalB_DinD1);
      preferences.putFloat("CalB_DinD1", CalB_DinD1);
    }

    if (request->hasParam("In_CalM_DinD2")) {
      CalM_DinD2 = request->getParam("In_CalM_DinD2")->value().toFloat();
      Serial.print("CalM_DinD2 = ");
      Serial.println(CalM_DinD2);
      preferences.putFloat("CalM_DinD2", CalM_DinD2);
    }
    if (request->hasParam("In_CalB_DinD2")) {
      CalB_DinD2 = request->getParam("In_CalB_DinD2")->value().toFloat();
      Serial.print("CalB_DinD2 = ");
      Serial.println(CalB_DinD2);
      preferences.putFloat("CalB_DinD2", CalB_DinD2);
    }

    if (request->hasParam("In_CalM_DinD3")) {
      CalM_DinD3 = request->getParam("In_CalM_DinD3")->value().toFloat();
      Serial.print("CalM_DinD3 = ");
      Serial.println(CalM_DinD3);
      preferences.putFloat("CalM_DinD3", CalM_DinD3);
    }
    if (request->hasParam("In_CalB_DinD3")) {
      CalB_DinD3 = request->getParam("In_CalB_DinD3")->value().toFloat();
      Serial.print("CalB_DinD3 = ");
      Serial.println(CalB_DinD3);
      preferences.putFloat("CalB_DinD3", CalB_DinD3);
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
    
    request->send(204);
  });

  server.on("/dataSet", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendData().c_str());
  });

  server.on("/dataSetHeat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendDataHeat().c_str());
  });

  server.on("/updateServer", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("SEDANG PROSES OTA");
    Serial.println("updateFirmware");
    // Update_OTA = 1;
    
    // updateFirmware();
    request->send(200);
  });

  //  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });


  server.begin();
}