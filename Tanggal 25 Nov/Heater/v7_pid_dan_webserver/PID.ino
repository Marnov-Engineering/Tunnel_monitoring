// void vStartOutputPID(){
//   static uint32_t LastTime10ms = 0;
//   if (millis() - LastTime10ms > 10) {
//     counting++;
//     LastTime10ms = millis();
//     // Serial.println(counting);
//   }
//   if (counting < Pulse) {
//     digitalWrite(SSRPin, HIGH);
//     // Serial.print("SSR Nyala ");
//   } else {
//     digitalWrite(SSRPin, LOW);
//     // Serial.print("SSR MATI ");
//   }
//   if (counting == 100) {
//     counting = 0;
//   }
// }

void vStartOutputPID() {
  static uint32_t LastTime10ms = 0;
  if (millis() - LastTime10ms > 10) {
    counting++;
    LastTime10ms = millis();
  }
  digitalWrite(SSRPin, (counting < Pulse) ? HIGH : LOW);
  if (counting >= 100) counting = 0;
}

void vModeA(){
  static uint32_t LastTime = 0;
  if (millis() - LastTime > 2000){
    LastTime = millis();
    // vBacaSuhuPipa();
    // Input = SuhuDinding;    ///// input suhu dinding
    Input = bme.readTemperature();   //// input suhu pipa
    lonjakan_BME = abs(Input - last_Input);
    if (lonjakan_BME > 50){
      // Input = last_Input;
      int retry = 0;
      while(lonjakan_BME > 50 && retry < 3){
        delay(50);
        Input = bme.readTemperature();
        lonjakan_BME = abs(Input - last_Input);
        retry++;
      }
    } 

    else{
      last_Input = Input;
    }
    
    error = Setpoint - Input;

    if (error < 10){
      myPID.SetTunings(kp, ki, kd);
    }
    else {
      myPID.SetTunings(kp, 0, kd);
    }
    myPID.Compute();
    Pulse = (int)round(Output); // Output sudah dibatasi
    OutPID = (int)round(Output);
    // Pulse = (Output / 255) * 100;
    // if (Pulse > 50) {
    //   Pulse = 50;
    // }

    // static unsigned long saturStart = 0;
    // bool saturated = (Output >= 49.9 || Output <= 0.1); // hampir saturasi
    // if (saturated) {
    //   if (saturStart == 0) saturStart = millis();
    // } else {
    //   saturStart = 0;
    // }
    // // jika sudah saturasi lebih dari X detik, matikan I sementara
    // if (saturStart && (millis() - saturStart) > 20000UL) { // 20s
    //   myPID.SetTunings(kp, 0.0, kd);
    // } else {
    //   myPID.SetTunings(kp, ki, kd);
    // }
    // myPID.Compute();
    // Pulse = (int)round(Output); // Output sudah dibatasi


    // === Debug Print ===
    Serial.print("SP=");
    Serial.print(Setpoint, 1);
    Serial.print(" | In=");
    Serial.print(Input, 1);
    Serial.print(" | Error=");
    Serial.print(error, 1);
    Serial.print(" | Out=");
    Serial.print(Output, 1);
    Serial.print(" | Pulse=");
    Serial.print(Pulse, 1);
    Serial.print(" | Kp=");
    Serial.print(kp, 5);
    Serial.print(" Ki=");
    Serial.print(ki, 5);
    Serial.print(" Kd=");
    Serial.println(kd, 5);
  }
} 