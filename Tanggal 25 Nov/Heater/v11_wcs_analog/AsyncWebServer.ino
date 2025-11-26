String vSendDataHeat() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  
  sprintf(data, "%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f", Suhu_Tunnel, Heater_st, Fan_Power, Raw_WCS, Setpoint, kp, ki, kd, Pulse, Output,Input);
  SDataHeat = String(data);
  Serial.print("SDataHeat = ");
  Serial.println(SDataHeat);

  return SDataHeat;
}

void vAsyncWebServer() {
  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  
  server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){

      // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
      // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
      // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;

      String Dsuhu1 = String(SuhuDinding);
      // String Dsuhu2 = String(data[1]);
      // String Dsuhu3 = String(data[2]);
      
      BME280 = String(bme.readTemperature());

      String json = "{";
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      // json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      // json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"BME280\":" + safeValue(BME280);

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

    });

  

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("In_Heat_st")) {
      Heater_st = request->getParam("In_Heat_st")->value().toInt();
      Serial.print("Heater_st :");
      Serial.println(Heater_st);
      if(Heater_st == 1){
        // servo1.write(35);
        targetPos = 35;

        safety = 1;
      }
      else{
        // servo1.write(0);
        targetPos = 0;

        safety = 0;
      }
    }

    if (request->hasParam("In_Fan_Power")) {
      Fan_Power = request->getParam("In_Fan_Power")->value().toInt();
      Serial.print("Fan_Power :");
      Serial.println(Fan_Power);
      if(Fan_Power == 1){
        // servo1.write(100);
        targetPos = 100;

      }
      else if(safety){
        // servo1.write(35);
        targetPos = 35;

      }
      else{
        // servo1.write(0);
        targetPos = 0;

      }
    }

    if (request->hasParam("In_Suhu_Tunnel")) {
      Suhu_Tunnel = request->getParam("In_Suhu_Tunnel")->value().toInt();
      Serial.print("Suhu_Tunnel :");
      Serial.println(Suhu_Tunnel);
      h = Suhu_Tunnel;
    }

    if (request->hasParam("In_kp")) {
      kp = request->getParam("In_kp")->value().toFloat();
      Serial.print("kp :");
      Serial.println(kp);
    }

    if (request->hasParam("In_ki")) {
      ki = request->getParam("In_ki")->value().toFloat();
      Serial.print("ki :");
      Serial.println(ki);
    }

    if (request->hasParam("In_kd")) {
      kd = request->getParam("In_kd")->value().toFloat();
      Serial.print("kd :");
      Serial.println(kd);
    }

    if (request->hasParam("In_sp")) {
      Setpoint = request->getParam("In_sp")->value().toDouble();
      Serial.print("sp :");
      Serial.println(Setpoint);
    }
    
    request->send(204);
  });

  server.on("/dataSetHeat", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!bSedangMengirimESPNOW){
      request->send_P(200, "text/plain", vSendDataHeat().c_str());
    }
    
  });

  server.on("/updateServer", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("SEDANG PROSES OTA");
    Serial.println("updateFirmware");
    // Update_OTA = 1;
    
    // updateFirmware();
    request->send(200);
  });

  server.begin();
}