// //----------- OTA ---------------//
// void updateFirmware() {
//   HTTPClient http;
//   http.begin(firmwareURL);
//   // vDisplayText("TRY", "DOWNLOAD");
//   int httpCode = http.GET();
//   if (httpCode <= 0) {
//     Serial.printf("HTTP failed, error: %s\n", http.errorToString(httpCode).c_str());
//     // vDisplayText("DOWNLOAD", "FAIL");
//     // WakeUpReason == LORA;
//     return;
//   }
//   // vDisplayText("DOWNLOAD", "PROGRAM");
//   int contentLen = http.getSize();
//   Serial.printf("Content-Length: %d\n", contentLen);
//   if (!Update.begin(contentLen)) {
//     Serial.println("Not enough space to begin OTA");
//     // vDisplayText("NOT", "SPACE");
//     // WakeUpReason == LORA;
//     return;
//   }
//   // vDisplayText("UPDATING", "PROGRAM");
//   WiFiClient *client = http.getStreamPtr();
//   size_t written = Update.writeStream(*client);
//   Serial.printf("OTA: %d/%d bytes written.\n", written, contentLen);
//   if (written != contentLen) {
//     Serial.println("Wrote partial binary. Giving up.");
//     // vDisplayText(" WROTE", "FAIL");
//     // WakeUpReason == LORA;
//     return;
//   }
//   if (!Update.end()) {
//     Serial.println("Error from Update.end(): " + String(Update.getError()));
//     // vDisplayText("FAILED", String(Update.getError()).c_str());
//     // WakeUpReason == LORA;
//     return;
//   }
//   if (Update.isFinished()) {
//     // vDisplayText("RESTART", "5s");
//     Serial.println("Update successfully completed. Rebooting.");
//     vTaskDelay(5000);
//     ESP.restart();
//   } else {
//     Serial.println("Error from Update.isFinished(): " + String(Update.getError()));
//     // vDisplayText("FAILED", String(Update.getError()).c_str());
//     // WakeUpReason == LORA;
//     return;
//   }
// }