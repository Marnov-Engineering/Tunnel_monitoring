void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    Serial.print(path);
    // while(file.available()){
    //     Serial.write(file.read());
    // }
    fileLen = file.size();
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    if (fs.exists(path)) {
        Serial.println("Target ada hapus dulu.......");
        fs.remove(path);   // hapus dulu biar tidak gagal
    }
    
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }


    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("File tertulis kosong");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    // Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        // Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);

    if (fs.exists(path2)) {
        Serial.println("Target file already exists, deleting it first...");
        fs.remove(path2);   // hapus dulu biar tidak gagal
    }

    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void vTulisDataAwalSuhu() {
  File file = SD.open("/SuhuData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/SuhuData.js", "var suhu = [\n[null,'Suhu Ujung Pipa 1','Suhu Ujung Pipa 2', 'Suhu 1', 'Suhu 2', 'Suhu 3', 'Suhu 4', 'Suhu 5', 'Suhu 6', 'Suhu 7', 'Suhu Dinding 1', 'Suhu Dinding 2', 'Suhu Dinding 3', 'Suhu Pipa 1', 'Suhu Pipa 2', 'Suhu Pipa 3']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalHumid() {
  File file = SD.open("/HumidData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/HumidData.js", "var kelembapan = [\n[null,'Kelembapan Ujung Pipa 1','Kelembapan Ujung Pipa 1', 'Kelembapan 1', 'Kelembapan 2', 'Kelembapan 3', 'Kelembapan 4', 'Kelembapan 5', 'Kelembapan 6', 'Kelembapan 7']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalAngin() {
  File file = SD.open("/WindData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/WindData.js", "var wind = [\n[null,'Kecepatan udara ujung pipa 1','Kecepatan udara ujung pipa 2', 'Kecepatan udara 1', 'Kecepatan udara 2', 'Kecepatan udara 3', 'Kecepatan udara 4', 'Kecepatan udara 5', 'Kecepatan udara 6', 'Kecepatan udara 7']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalTekanan() {
  File file = SD.open("/TekananData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/TekananData.js", "var tekanan = [\n[null,'Tekanan ujung pipa 1','Tekanan ujung pipa 2']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalTunnel() {
  File file = SD.open("/TunnelData.csv");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    char buffer[600];  // perbesar biar cukup
    sprintf(buffer, "0,%lu,nan,nan,nan,nan,nan,nan,nan,nan\n", waktuUpdate);
    writeFile(SD, "/TunnelData.csv",buffer);
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataBaruFireEpoch(unsigned long NamaEpochCSV) {
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVFire/%lu.csv", NamaEpochCSV);
    writeFile(SD, bufferEpochName, "");
    xSemaphoreGive(sdCardMutex);
  }
}

void vTulisDataBaruSpreadEpoch(unsigned long NamaEpochCSV) {
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVSpread/%lu.txt", NamaEpochCSV);
    writeFile(SD, bufferEpochName, "");
    xSemaphoreGive(sdCardMutex);
  }
}

bool bPengechekanDuaFile(fs::FS &fs, const char *dirname){
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    File root = fs.open(dirname);
    if (!root || !root.isDirectory()) {
      Serial.println("Gagal membuka direktori atau bukan direktori");
      xSemaphoreGive(sdCardMutex);
      return false;
    }

    int fileCount = 0;
    File file = root.openNextFile();

    while (file) {
      if (!file.isDirectory()) {
        fileCount++;
        if (fileCount >= 2) {
          file.close();
          root.close();
          Serial.printf("Ada %d file di direktori %s\n", fileCount, dirname);
          xSemaphoreGive(sdCardMutex);
          return true;
        }
      }
      file = root.openNextFile();
    }

    root.close();
    Serial.printf("Jumlah file di %s: %d (kurang dari 2)\n", dirname, fileCount);
    xSemaphoreGive(sdCardMutex);
    return false;
  }
}

String vSDEpochTerkecil(fs::FS &fs, const char *dirname){
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    File root = fs.open(dirname);
    if (!root || !root.isDirectory()) {
      Serial.println("Gagal membuka direktori atau bukan direktori");
      xSemaphoreGive(sdCardMutex);
      return "";
    }

    File file = root.openNextFile();

    while (file && file.isDirectory()) {
      // Lewati folder jika ada
      file = root.openNextFile();
    }

    if (!file) {
      Serial.println("Tidak ada file di direktori.");
      root.close();
      xSemaphoreGive(sdCardMutex);
      return "";
    }

    String firstFile = file.name();
    Serial.printf("File pertama (terkecil): %s\n", firstFile.c_str());

    file.close();
    root.close();
    xSemaphoreGive(sdCardMutex);
    return firstFile;
  }
}

String vPersiapanPayloadJson(fs::FS &fs, const char *path) {
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    File file = fs.open(path, FILE_READ);
    if (!file) {
      Serial.printf("Gagal membuka file: %s\n", path);
      xSemaphoreGive(sdCardMutex);
      return "";
    }

    String payload;
    payload.reserve(file.size());  // alokasi awal untuk efisiensi

    while (file.available()) {
      payload += (char)file.read();
    }

    file.close();

    Serial.printf("File %s dibaca (%u byte)\n", path, payload.length());
    xSemaphoreGive(sdCardMutex);
    return payload;
  }
}

void vTulisDataAwalSuhuPipa() {
  File file = SD.open("/SuhuPipaData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/SuhuPipaData.js", "var suhuPipa = [\n[null,'Suhu Pipa 1','Suhu Pipa 2','Suhu Pipa 3']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalSuhuDinding() {
  File file = SD.open("/SuhuDindingData.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/SuhuDindingData.js", "var suhuDinding = [\n[null,'Suhu Dinding 1','Suhu Dinding 2','Suhu Dinding 3']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalOutPID() {
  File file = SD.open("/OutPID.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/OutPID.js", "var OutPID = [\n[null,'Out PID 1','Out PID 2','Out PID 3']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void vTulisDataAwalGabunganSuhu() {
  File file = SD.open("/GabunganSuhu.js");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/GabunganSuhu.js", "var GabunganSuhu = [\n[null,'Suhu Pipa 1','Suhu Pipa 2','Suhu Pipa 3','Suhu Dinding 1','Suhu Dinding 2','Suhu Dinding 3']]\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

// void writeToSdCard(String whichFile) {

//   String path = "/" + whichFile + ".js";
//   File myFile = SD.open(path.c_str(), FILE_WRITE);

  
//   int S = (myFile.size()) - 2;  
//   myFile.seek(S);  

//   myFile.println(',');
//   myFile.close();

//   String stringToAppend = "";
//   if(whichFile == "SuhuData"){
//     stringToAppend = SuhuData;
//   } else if (whichFile == "HumidData") {
//     stringToAppend = HumidData;
//   } else if (whichFile == "WindData") {
//     stringToAppend = WindData;
//   } else if (whichFile == "TekananData") {
//     stringToAppend = TekananData;
//   } else if (whichFile == "SuhuPipaData") {
//     stringToAppend = SuhuPipaData;
//   } else if (whichFile == "SuhuDindingData") {
//     stringToAppend = SuhuDindingData;
//   } else if (whichFile == "OutPID") {
//     stringToAppend = OutPID;
//   } else if (whichFile == "GabunganSuhu") {
//     stringToAppend = GabunganSuhu;
//   }
//   appendFile(SD, path.c_str(), stringToAppend.c_str());
//   Serial.println("Buffer APPEND");
//   // readFile(SD, path.c_str());
// }

// void writeAllToSdCard(String whichFile) {

//   String path = "/" + whichFile + ".csv";
//   String stringToAppend = "";
//   // tunnel data
//   stringToAppend = TunnelData;
//   appendFile(SD, path.c_str(), TunnelData.c_str());

//   // readFile(SD, path.c_str());
// }


void writeBufferSuhu(const char* path){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]]\n", waktuUpdate, 
  safeValue(Usuhu1).c_str(), 
  safeValue(Usuhu2).c_str(), 
  safeValue(suhu1).c_str(), 
  safeValue(suhu2).c_str(), 
  safeValue(suhu3).c_str(), 
  safeValue(suhu4).c_str(), 
  safeValue(suhu5).c_str(), 
  safeValue(suhu6).c_str(), 
  safeValue(suhu7).c_str(),
  SuhuDS1,
  SuhuDS2,
  SuhuDS3,
  BME1,
  BME2,
  BME3);
  Serial.println("---------------------------------");
  Serial.println("Buffer suhu data");
  Serial.println(buffer);

  File myFile = SD.open(path, FILE_WRITE);
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  
  myFile.println(',');
  myFile.close();

  appendFile(SD, path, buffer);



}

void writeBufferKelembapan(const char* path){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(UHumid1).c_str(), 
  safeValue(UHumid2).c_str(), 
  safeValue(Humid1).c_str(), 
  safeValue(Humid2).c_str(), 
  safeValue(Humid3).c_str(), 
  safeValue(Humid4).c_str(), 
  safeValue(Humid5).c_str(), 
  safeValue(Humid6).c_str(), 
  safeValue(Humid7).c_str());
  Serial.println("---------------------------------");
  Serial.println("Buffer kelembapan data");
  Serial.println(buffer);
  // Serial.println(buffer);
  File myFile = SD.open(path, FILE_WRITE);
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  
  myFile.println(',');
  myFile.close();

  appendFile(SD, path, buffer);
}

void writeBufferWind(const char* path){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(Ukec1).c_str(), 
  safeValue(Ukec2).c_str(), 
  safeValue(kec1).c_str(), 
  safeValue(kec2).c_str(), 
  safeValue(kec3).c_str(), 
  safeValue(kec4).c_str(), 
  safeValue(kec5).c_str(), 
  safeValue(kec6).c_str(), 
  safeValue(kec7).c_str());
  Serial.println("---------------------------------");
  Serial.println("Buffer angin data");
  Serial.println(buffer);

  File myFile = SD.open(path, FILE_WRITE);
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  
  myFile.println(',');
  myFile.close();

  appendFile(SD, path, buffer);
}

void writeBufferTekanan(const char* path){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s]]\n", waktuUpdate, 
  safeValue(USDP1).c_str(), 
  safeValue(USDP2).c_str());
  // Serial.println(buffer);
  Serial.println("---------------------------------");
  Serial.println("Buffer tekanan data");
  Serial.println(buffer);

  File myFile = SD.open(path, FILE_WRITE);
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  
  myFile.println(',');
  myFile.close();

  appendFile(SD, path, buffer);
}

void writeBufferSpreadSheet(const char* path) {
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    int counterjson = 0;
    JsonArray rows = doc["rows"].as<JsonArray>();
    if (rows.isNull()) {
      rows = doc.createNestedArray("rows");
    }
    for (int i = 0; i < counterParsing; i++) {
      JsonObject row = rows.createNestedObject();
      row["Time"] = arraywaktu[i];
      row["S1"] = arrayJson[1][i];
      row["S2"] = arrayJson[2][i];
      row["S3"] = arrayJson[3][i];
      row["S4"] = arrayJson[4][i];
      row["S5"] = arrayJson[5][i];
      row["S6"] = arrayJson[6][i];
      row["S7"] = arrayJson[7][i];
      row["SU1"] = arrayJson[8][i];
      row["SU2"] = arrayJson[9][i];
      row["SD1"] = arrayJson[10][i];
      row["SD2"] = arrayJson[11][i];
      row["SD3"] = arrayJson[12][i];
      row["SP1"] = arrayJson[13][i];
      row["SP2"] = arrayJson[14][i];
      row["SP3"] = arrayJson[15][i];
      row["H1"] = arrayJson[16][i];
      row["H2"] = arrayJson[17][i];
      row["H3"] = arrayJson[18][i];
      row["H4"] = arrayJson[19][i];
      row["H5"] = arrayJson[20][i];
      row["H6"] = arrayJson[21][i];
      row["H7"] = arrayJson[22][i];
      row["HU1"] = arrayJson[23][i];
      row["HU2"] = arrayJson[24][i];
      row["K1"] = arrayJson[25][i];
      row["K2"] = arrayJson[26][i];
      row["K3"] = arrayJson[27][i];
      row["K4"] = arrayJson[28][i];
      row["K5"] = arrayJson[29][i];
      row["K6"] = arrayJson[30][i];
      row["K7"] = arrayJson[31][i];
      row["KU1"] = arrayJson[32][i];
      row["KU2"] = arrayJson[33][i];
      row["PU1"] = arrayJson[34][i];
      row["PU2"] = arrayJson[35][i];
      row["PUS1"] = arrayJson[36][i];
      row["PUS2"] = arrayJson[37][i];
      row["PID1"] = arrayJson[38][i];
      row["PID2"] = arrayJson[39][i];
      row["PID3"] = arrayJson[40][i];
      row["Error"] = arrayJson[41][i];
      counterjson++;
      delay(1);
    }

    String payload;
    serializeJson(doc, payload);

    Serial.println("Buffer tunnel data");
    Serial.println(payload);

    Serial.print("counterjson =");
    Serial.println(counterjson);
    
    appendFile(SD, path, payload.c_str());
    doc.clear();
    // tulis ke file
    // appendFile(SD, "/TunnelData.csv", buffer);
    xSemaphoreGive(sdCardMutex);
  }
}

void writeBufferTunnel(const char* path) {
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      
    waktuUpdate = rtc.getEpoch();
    char buffer[1000];  // perbesar biar cukup
    sprintf(buffer,
          "1,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "2,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "3,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "4,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "5,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "6,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "7,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "8,%lu,nan,%s,%s,%s,%s,1,2\n"
          "9,%lu,nan,%s,%s,%s,%s,1,2\n"
          "10,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "11,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "12,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "13,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "14,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "15,%lu,nan,%.2f,nan,nan,nan,nan,nan\n",
          waktuUpdate, safeValue(suhu1).c_str(), safeValue(Humid1).c_str(), safeValue(kec1).c_str(),
          waktuUpdate, safeValue(suhu2).c_str(), safeValue(Humid2).c_str(), safeValue(kec2).c_str(),
          waktuUpdate, safeValue(suhu3).c_str(), safeValue(Humid3).c_str(), safeValue(kec3).c_str(),
          waktuUpdate, safeValue(suhu4).c_str(), safeValue(Humid4).c_str(), safeValue(kec4).c_str(),
          waktuUpdate, safeValue(suhu5).c_str(), safeValue(Humid5).c_str(), safeValue(kec5).c_str(),
          waktuUpdate, safeValue(suhu6).c_str(), safeValue(Humid6).c_str(), safeValue(kec6).c_str(),
          waktuUpdate, safeValue(suhu7).c_str(), safeValue(Humid7).c_str(), safeValue(kec7).c_str(),
          waktuUpdate, safeValue(Usuhu1).c_str(), safeValue(UHumid1).c_str(), safeValue(Ukec1).c_str(),  safeValue(USDP1).c_str(),
          waktuUpdate, safeValue(Usuhu2).c_str(), safeValue(UHumid2).c_str(), safeValue(Ukec2).c_str(),  safeValue(USDP2).c_str(),
          waktuUpdate,SuhuDS1,
          waktuUpdate,SuhuDS2,
          waktuUpdate,SuhuDS3,
          waktuUpdate,BME1,
          waktuUpdate,BME2,
          waktuUpdate,BME3 );

    Serial.println("Buffer tunnel data");
    Serial.println(buffer);
    
    appendFile(SD, path, buffer);
    // tulis ke file
    // appendFile(SD, "/TunnelData.csv", buffer);
    xSemaphoreGive(sdCardMutex);
  }
}

void writeBufferSuhuPipa(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%.2f,%.2f,%.2f]]\n", waktuUpdate,  
  BME1, 
  BME2, 
  BME3);
  // Serial.println(buffer);
  SuhuPipaData = buffer;
}

void writeBufferSuhuDinding(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%.2f,%.2f,%.2f]]\n", waktuUpdate, 
  SuhuDS1, 
  SuhuDS2, 
  SuhuDS3);
  // Serial.println(buffer);
  SuhuDindingData = buffer;
}

void writeBufferOutPID(const char* path){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%.2f,%.2f,%.2f]]\n", waktuUpdate, 
  out_pid1,
  out_pid2,
  out_pid3 );
  // Serial.println(buffer);
  Serial.println("Buffer PID");

  File myFile = SD.open(path, FILE_WRITE);
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  
  myFile.println(',');
  myFile.close();


  appendFile(SD, path, buffer);
}

void writeBufferGabunganSuhu(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]]\n", waktuUpdate, 
  BME1, 
  BME2, 
  BME3,
  SuhuDS1,
  SuhuDS2,
  SuhuDS3);
  // Serial.println(buffer);
  GabunganSuhu = buffer;
}

void makeCopyDataFirebase(fs::FS &fs, const char *sourcePath) {
    // Ambil nama file asli
    String src = String(sourcePath);  // contoh "/SuhuData.js"

    // Ambil hanya nama file (tanpa "/")
    // String filename = src.substring(1);  // "SuhuData.js"

    // unsigned long epochTime = getCurrentTime();
    unsigned long epochTime = rtc.getEpoch();

    struct tm timeinfo;
    gmtime_r((time_t*)&epochTime, &timeinfo);  // pecah epoch ke tm

    currentYear  = timeinfo.tm_year + 1900;
    currentMonth = timeinfo.tm_mon + 1;
    currentDay   = timeinfo.tm_mday;

    char fileNameUpload[80] = "";
    char fileLocUpload[80] = "";
    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);

    // Buat nama baru dengan prefix "Up_"
    String newFilename = fileLocUpload; // "/Up_SuhuData.js"

    Serial.printf("Copying %s to %s\n", sourcePath, newFilename.c_str());

    File sourceFile = fs.open(sourcePath, FILE_READ);
    if (!sourceFile) {
        Serial.println("Failed to open source file for reading");
        return;
    }

    File destFile = fs.open(newFilename.c_str(), FILE_WRITE);
    if (!destFile) {
        Serial.println("Failed to create new file for writing");
        sourceFile.close();
        return;
    }

    // buffer copy
    uint8_t buffer[512];
    size_t bytesRead;
    while ((bytesRead = sourceFile.read(buffer, sizeof(buffer))) > 0) {
        destFile.write(buffer, bytesRead);
    }

    sourceFile.close();
    destFile.close();

    Serial.printf("File copied successfully to %s\n", newFilename.c_str());
}

void renameDataFirebase(fs::FS &fs, const char *sourcePath) {
    // Ambil waktu sekarang
    unsigned long epochTime = rtc.getEpoch();
    struct tm timeinfo;
    gmtime_r((time_t*)&epochTime, &timeinfo);

    currentYear  = timeinfo.tm_year + 1900;
    currentMonth = timeinfo.tm_mon + 1;
    currentDay   = timeinfo.tm_mday;

    char fileLocUpload[80] = "";
    snprintf(fileLocUpload, sizeof(fileLocUpload),
             "/Up_Tunnel_%04d-%02d-%02d.csv",
             currentYear, currentMonth, currentDay);

    String newFilename = fileLocUpload;

    Serial.printf("Renaming %s to %s\n", sourcePath, newFilename.c_str());

    if (fs.exists(newFilename.c_str())) {
        Serial.println("Target file already exists, deleting it first...");
        fs.remove(newFilename.c_str());   // hapus dulu biar tidak gagal
    }

    if (fs.rename(sourcePath, newFilename.c_str())) {
        Serial.printf("File renamed successfully to %s\n", newFilename.c_str());
    } else {
        Serial.println("Failed to rename file");
    }
}

void appendAndReplace(fs::FS &fs, const char *sourcePath, const char *targetPath) {
    Serial.printf("Appending %s into %s\n", sourcePath, targetPath);

    // --- 1. Buka file asal ---
    File sourceFile = fs.open(sourcePath, FILE_READ);
    if (!sourceFile) {
        Serial.println("Failed to open source file for reading");
        return;
    }

    // --- 2. Buka file tujuan dengan mode append ---
    File destFile = fs.open(targetPath, FILE_APPEND);
    if (!destFile) {
        Serial.println("Failed to open target file for appending");
        sourceFile.close();
        return;
    }

    // --- 3. Append isi file asal ke file tujuan ---
    uint8_t buffer[512];
    size_t bytesRead;
    while ((bytesRead = sourceFile.read(buffer, sizeof(buffer))) > 0) {
        destFile.write(buffer, bytesRead);
    }

    sourceFile.close();
    destFile.close();

    // --- 4. Hapus file asal ---
    if (fs.remove(sourcePath)) {
        Serial.printf("Deleted source file: %s\n", sourcePath);
    } else {
        Serial.println("Failed to delete source file");
        return;
    }

    // // --- 5. Hapus dulu jika file asal (nama lama) masih ada ---
    // if (fs.exists(sourcePath)) {
    //     fs.remove(sourcePath);
    // }

    // --- 6. Rename file tujuan menjadi file asal ---
    if (fs.rename(targetPath, sourcePath)) {
        Serial.printf("Renamed %s back to %s\n", targetPath, sourcePath);
    } else {
        Serial.println("Failed to rename target file back to source name");
    }
}



uint32_t calculateFileCRC(fs::FS &fs, const char *path) {
    File file = fs.open(path, FILE_READ);
    if (!file) {
        Serial.printf("Failed to open %s for CRC\n", path);
        return 0;
    }

    CRC32 crc;
    uint8_t buffer[512];
    size_t bytesRead;

    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        crc.update(buffer, bytesRead);
    }

    file.close();
    return crc.finalize();
}

bool verifyCopyCRC(fs::FS &fs, const char *sourcePath) {
    // buat nama baru "Up_xxx"
    // String src = String(sourcePath); 
    // String filename = src.substring(1);  
    // String newFilename = "/Up_" + filename;

    // unsigned long epochTime = getCurrentTime();
    unsigned long epochTime = rtc.getEpoch();

    struct tm timeinfo;
    gmtime_r((time_t*)&epochTime, &timeinfo);  // pecah epoch ke tm

    currentYear  = timeinfo.tm_year + 1900;
    currentMonth = timeinfo.tm_mon + 1;
    currentDay   = timeinfo.tm_mday;

    char fileNameUpload[80] = "";
    char fileLocUpload[80] = "";
    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);

    // Buat nama baru dengan prefix "Up_"
    String newFilename = fileLocUpload; // "/Up_SuhuData.js"

    // hitung CRC keduanya
    uint32_t crcSrc = calculateFileCRC(fs, sourcePath);
    uint32_t crcNew = calculateFileCRC(fs, newFilename.c_str());

    Serial.printf("CRC %s : %08X\n", sourcePath, crcSrc);

    Serial.printf("CRC %s : %08X\n", newFilename.c_str(), crcNew);

    if (crcSrc == crcNew) {
        Serial.println("CRC Match ✅ File copy is valid");
        deleteFile(SD, "/TunnelData.csv");
        vTulisDataAwalTunnel(); 
        return true;
    } else {
        Serial.println("CRC Mismatch ❌ Copy failed or corrupted");
        return false;
    }
}



