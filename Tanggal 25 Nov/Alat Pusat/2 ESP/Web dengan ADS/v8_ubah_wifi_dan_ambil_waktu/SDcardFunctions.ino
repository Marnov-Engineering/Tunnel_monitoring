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
    writeFile(SD, "/SuhuData.js", "var suhu = [\n[null,'US1','US2', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7']]\n");
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
    writeFile(SD, "/WindData.js", "var wind = [\n[null,'UW1','UW2', 'W1', 'W2', 'W3', 'W4', 'W5', 'W6', 'W7']]\n");
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
    writeFile(SD, "/TekananData.js", "var tekanan = [\n[null,'T1','T2']]\n");
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
    writeFile(SD, "/SuhuPipaData.js", "var suhuPipa = [\n[null,'SP1','SP2','SP3']]\n");
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
    writeFile(SD, "/SuhuDindingData.js", "var suhuDinding = [\n[null,'SD1','SD2','SD3']]\n");
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
  }
  appendFile(SD, path.c_str(), stringToAppend.c_str());

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

void writeBufferSuhuPipa(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[28]).c_str(), 
  safeValue(data[34]).c_str(), 
  safeValue(data[40]).c_str());
  // Serial.println(buffer);
  SuhuPipaData = buffer;
}

void writeBufferSuhuDinding(){
  char buffer[400];  // Sesuaikan ukuran buffer sesuai kebutuhan
  sprintf(buffer, "[%lu000,%s,%s,%s]]\n", waktuUpdate, 
  safeValue(data[27]).c_str(), 
  safeValue(data[33]).c_str(), 
  safeValue(data[39]).c_str());
  // Serial.println(buffer);
  SuhuDindingData = buffer;
}

