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
    while(file.available()){
        Serial.write(file.read());
    }
    fileLen = file.size();
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
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
    writeFile(SD, "/SuhuData.js", "var suhu = [\n[null,'Suhu Ujung Pipa 1','Suhu Ujung Pipa 2', 'Suhu 1', 'Suhu 2', 'Suhu 3', 'Suhu 4', 'Suhu 5', 'Suhu 6', 'Suhu 7']]\n");
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
    writeFile(SD, "/TunnelData.csv","");
  } else {
    Serial.println("File already exists");
  }
  file.close();
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

void writeToSdCard(String whichFile) {

  String path = "/" + whichFile + ".js";
  File myFile = SD.open(path.c_str(), FILE_WRITE);

  
  int S = (myFile.size()) - 2;  
  myFile.seek(S);  

  myFile.println(',');
  myFile.close();

  String stringToAppend = "";
  if(whichFile == "SuhuData"){
    stringToAppend = SuhuData;
  } else if (whichFile == "WindData") {
    stringToAppend = WindData;
  } else if (whichFile == "TekananData") {
    stringToAppend = TekananData;
  } else if (whichFile == "SuhuPipaData") {
    stringToAppend = SuhuPipaData;
  } else if (whichFile == "SuhuDindingData") {
    stringToAppend = SuhuDindingData;
  } else if (whichFile == "OutPID") {
    stringToAppend = OutPID;
  } else if (whichFile == "GabunganSuhu") {
    stringToAppend = GabunganSuhu;
  }
  appendFile(SD, path.c_str(), stringToAppend.c_str());

  // readFile(SD, path.c_str());
}

void writeAllToSdCard(String whichFile) {

  String path = "/" + whichFile + ".csv";
  String stringToAppend = "";
  // tunnel data
  stringToAppend = TunnelData;
  appendFile(SD, path.c_str(), TunnelData.c_str());

  // readFile(SD, path.c_str());
}


void writeBufferSuhu(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[14]).c_str(), 
  safeValue(data[17]).c_str(), 
  safeValue(data[0]).c_str(), 
  safeValue(data[2]).c_str(), 
  safeValue(data[4]).c_str(), 
  safeValue(data[6]).c_str(), 
  safeValue(data[8]).c_str(), 
  safeValue(data[10]).c_str(), 
  safeValue(data[12]).c_str());
  // Serial.println(buffer);
  SuhuData = buffer;
}

void writeBufferWind(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[15]).c_str(), 
  safeValue(data[18]).c_str(), 
  safeValue(data[1]).c_str(), 
  safeValue(data[3]).c_str(), 
  safeValue(data[5]).c_str(), 
  safeValue(data[7]).c_str(), 
  safeValue(data[9]).c_str(), 
  safeValue(data[11]).c_str(), 
  safeValue(data[13]).c_str());
  // Serial.println(buffer);
  WindData = buffer;
}

void writeBufferTekanan(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s]]\n", waktuUpdate, 
  safeValue(data[16]).c_str(), 
  safeValue(data[19]).c_str());
  // Serial.println(buffer);
  TekananData = buffer;
}

// void writeBufferTunnel(){
//   char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
//   sprintf(buffer, "%lu000,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", waktuUpdate, 
//   safeValue(data[0]).c_str(), 
//   safeValue(data[1]).c_str(), 
//   safeValue(data[2]).c_str(), 
//   safeValue(data[3]).c_str(), 
//   safeValue(data[4]).c_str(), 
//   safeValue(data[5]).c_str(), 
//   safeValue(data[6]).c_str(), 
//   safeValue(data[7]).c_str(), 
//   safeValue(data[8]).c_str(),
//   safeValue(data[9]).c_str(), 
//   safeValue(data[10]).c_str(), 
//   safeValue(data[11]).c_str(), 
//   safeValue(data[12]).c_str(),
//   safeValue(data[13]).c_str(), 
//   safeValue(data[14]).c_str(),
//   safeValue(data[15]).c_str(), 
//   safeValue(data[16]).c_str(), 
//   safeValue(data[17]).c_str(), 
//   safeValue(data[18]).c_str(), 
//   safeValue(data[19]).c_str());
//   Serial.println("Buffer tunnel data");
//   Serial.println(buffer);
//   TunnelData = buffer;
// }

void writeBufferTunnel() {
  char buffer[600];  // perbesar biar cukup
  sprintf(buffer,
          "1,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "2,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "3,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "4,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "5,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "6,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "7,%lu,nan,%s,nan,%s,nan,nan,nan,nan\n"
          "8,%lu,nan,%s,nan,%s,%s,3,4\n"
          "9,%lu,nan,%s,nan,%s,%s,3,4\n",
          waktuUpdate, safeValue(data[0]).c_str(), safeValue(data[1]).c_str(),
          waktuUpdate, safeValue(data[2]).c_str(), safeValue(data[3]).c_str(),
          waktuUpdate, safeValue(data[4]).c_str(), safeValue(data[5]).c_str(),
          waktuUpdate, safeValue(data[6]).c_str(), safeValue(data[7]).c_str(),
          waktuUpdate, safeValue(data[8]).c_str(), safeValue(data[9]).c_str(),
          waktuUpdate, safeValue(data[10]).c_str(), safeValue(data[11]).c_str(),
          waktuUpdate, safeValue(data[12]).c_str(), safeValue(data[13]).c_str(),
          waktuUpdate, safeValue(data[14]).c_str(), safeValue(data[15]).c_str(), safeValue(data[16]).c_str(),
          waktuUpdate, safeValue(data[17]).c_str(), safeValue(data[18]).c_str(), safeValue(data[19]).c_str());

  Serial.println("Buffer tunnel data");
  Serial.println(buffer);
  TunnelData = buffer;
  // tulis ke file
  // appendFile(SD, "/TunnelData.csv", buffer);
}

void writeBufferSuhuPipa(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[28]).c_str(), 
  safeValue(data[41]).c_str(), 
  safeValue(data[54]).c_str());
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

void writeBufferOutPID(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[25]).c_str(), 
  safeValue(data[46]).c_str(), 
  safeValue(data[59]).c_str());
  // Serial.println(buffer);
  OutPID = buffer;
}

void writeBufferGabunganSuhu(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[28]).c_str(), 
  safeValue(data[41]).c_str(), 
  safeValue(data[54]).c_str(),
  safeValue(data[27]).c_str(),
  safeValue(data[40]).c_str(),
  safeValue(data[53]).c_str());
  // Serial.println(buffer);
  GabunganSuhu = buffer;
}

void makeCopyDataFirebase(fs::FS &fs, const char *sourcePath) {
    // Ambil nama file asli
    String src = String(sourcePath);  // contoh "/SuhuData.js"

    // Ambil hanya nama file (tanpa "/")
    // String filename = src.substring(1);  // "SuhuData.js"

    unsigned long epochTime = getCurrentTime();

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

    unsigned long epochTime = getCurrentTime();

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
        return true;
    } else {
        Serial.println("CRC Mismatch ❌ Copy failed or corrupted");
        return false;
    }
}



