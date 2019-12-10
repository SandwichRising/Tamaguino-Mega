/* ===========================
 *
 * Battery Monitoring Functions
 *
 * #define settings at top of main .ino determine type of battery formula used
 *
 * For reading battery levels.
 * Pull a value every 6 seconds, add to array,
 * average 10 array elements for current voltage.
 * ToDo: Detect if battery is charging?
 *
     ======================== */

float vArray[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int vIndex = 0;
float previousVoltageMillis = 0;
float smoothedVoltage = 0;
int batteryLevel = 0;

int pinVread = 0;




float getVoltage(){
  //Takes the reference voltage board is running on and the 
  //resistor values used in the voltage divider to calulate
  //current voltage level. 
  float referenceVoltage = 3.8;
  // measurements were giving slightly off readings
  // float R1 = 9970;
  // float R2 = 5060;
  float vRead = analogRead(VOLTAGE_PIN);
  pinVread = floor(vRead);
  // using actual R values is coming out with off V measurements
  // float vOutput = ((vRead*((R1+R2)/R2))/1023)*referenceVoltage;
  float vOutput = ((vRead*3.01)/1024)*referenceVoltage;
  return vOutput;
}

void vSmoothing(long unsigned currentMillisFunc){
  int readingDelay = 6000;
  float readingSmoother = 0;
  if (currentMillisFunc > previousVoltageMillis + readingDelay) {
    vArray[vIndex] = getVoltage();
    if (vIndex == 9) {
      vIndex = 0;
    } else {
      vIndex += 1;
    }
    int readingDivisor = 0;
    for (int i = 0; i < 10; i++){
      readingSmoother += vArray[i];
      if (vArray[i] > 0){
        readingDivisor += 1;
      }
    }
    smoothedVoltage = readingSmoother / readingDivisor;
    
#ifdef BATTERY_TYPE_18650
    //power curve for samsung 18650
    //this battery stays consistent: 4.0v+ drops fast, 4.0v->3.2v is 90% of charge, 3.2v->2.7v is %10
    if (smoothedVoltage > 4){
      batteryLevel = 100;
    } else if (smoothedVoltage > 3.2) {
      batteryLevel = floor((smoothedVoltage-3.2)*112.5+10);
    } else if (smoothedVoltage > 2.7) {
      batteryLevel = floor((smoothedVoltage-2.7)*20);
    } else {
      batteryLevel = 0;
    }
    if (currentMillisFunc > 30000 && smoothedVoltage > 0 && smoothedVoltage < 3.2) {
      lowBattery=true;
    }
#endif
#ifdef BATTERY_TYPE_LIPO_PACK
    //power curve for 2-wire protected lipo pack
    //this battery stays consistent: 4.1v+ drops fast, 4.1v->3.5v is 93.5% of charge, 3.5v->3.1v is %6.5
    if (smoothedVoltage > 4.1){
      batteryLevel = 100;
    } else if (smoothedVoltage > 3.5) {
      batteryLevel = floor((smoothedVoltage-3.5)*155+6.5);
    } else if (smoothedVoltage > 3.1) {
      batteryLevel = floor((smoothedVoltage-3.1)*1.25);
    } else {
      batteryLevel = 0;
    }
    if (currentMillisFunc > 30000 && smoothedVoltage > 0 && smoothedVoltage < 3.2) {
      lowBattery=true;
    }
#endif  
    
    previousVoltageMillis = currentMillisFunc;
  }
}

void debugVoltageDisplay (int displayInfo){
  if (displayInfo == 2){
  display.fillRect(0,55,30,11,BLACK);
  display.setCursor(3,56);
  display.print(pinVread);
  } else {
    display.fillRect(95,55,31,11,BLACK);
  }
  display.setCursor(97,56);
  display.print(smoothedVoltage, 2);
  display.print("v");
}

void batteryWindow (){
  display.fillRect(15,17,98,43,BLACK);
  display.drawRect(16,18,96,41,WHITE);
  display.fillRect(16,18,96,13,WHITE);
  display.setCursor(28,21);
  display.setTextColor(BLACK);
  display.print("Battery:");
  display.print(batteryLevel);
  display.print("%");
  display.setTextColor(WHITE);
  display.setCursor(21,35);
  display.print("Voltage: ");
  display.print(smoothedVoltage, 2);
  display.print("v");
  display.setCursor(21,46);
  display.print("Pin Read:");
  display.print(pinVread);
}
