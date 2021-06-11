// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TEMP_SENSORS_CHANNEL);


//uint8_t addr[SENSADDRLENGHT] = {0};
const uint8_t missingSensor[SENSADDRLENGHT]={0};

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setAddress(byte addr[], OneWire& oneWire){
  if ( !oneWire.search(addr)) {
    oneWire.reset_search();
    return ;
  }
  return;
}

void prepareReadTemp(const byte addr[SENSADDRLENGHT]){
  oneWire.reset_search();  
  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end  
  return;
}

float readTemp(const byte addr[SENSADDRLENGHT]){
  byte data[12];

  oneWire.reset();
  oneWire.select(addr);    
  oneWire.write(0xBE);         // Read Scratchpad

  for (size_t i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = oneWire.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;                  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3;             // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1;             // 11 bit res, 375 ms
  // default is 12 bit resolution, 750 ms conversion time
    
  return (float)raw / 16.0;
}

void printAddress(const uint8_t deviceAddress[]){
  for (uint8_t i = 0; i < SENSADDRLENGHT; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    Serial.print(" ");
  }
}

void printAddressDisplay(const uint8_t deviceAddress[]){
  for (uint8_t i = 0; i < SENSADDRLENGHT; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    M5.Lcd.print(deviceAddress[i], HEX);
    M5.Lcd.print(" ");
  }
}

void delayBadSensor(const byte addr[SENSADDRLENGHT]){
    if(addr[7] == byte(0x27)) {
      Serial.println(" Delay");    
      delay(100);
    }    
}

void updateTemperatures(float temperatures[NUMSENSORS]){
  for(size_t i=0; i<NUMSENSORS; i++){
    prepareReadTemp(sensorAddressList[i]);
//    delayBadSensor(sensorAddressList[i]);
  }  
  delay(750);     // maybe 750ms is enough, maybe not
  for(size_t i=0; i<NUMSENSORS; i++){
    temperatures[i] = readTemp(sensorAddressList[i]);
//    printAddress(sensorAddressList[i]);
//    Serial.println(temperatures[i]);
  }
}


bool isInSensorAddressList(const uint8_t sensorFoundAddress[SENSADDRLENGHT]){
  bool found = false;
  for(size_t i=0; i<NUMSENSORS; i++){
    bool identical = true;
    for(size_t j=0; j<SENSADDRLENGHT; j++){
      if (sensorFoundAddress[j] != sensorAddressList[i][j]) identical = false;
    }
    if(identical) found = true;
  }
  return found;
}

void setupTemperature(void){
  oneWire.reset_search();
  sensors.begin();
  uint8_t sensorFoundAddress[SENSADDRLENGHT];
  uint8_t nSensors = sensors.getDeviceCount();
  Serial.println("Found sensors with address: ");
  M5.Lcd.println("Found sensors with address: ");
  bool newSensor = false;
  for(size_t i=0; i<nSensors; i++){
      sensors.getAddress(sensorFoundAddress, i);
      Serial.println("");
      printAddress(sensorFoundAddress);
//      printAddressDisplay(sensorFoundAddress);
      Serial.println("Setting highest resolution");
      sensors.setResolution(sensorFoundAddress, 12);
      Serial.println("");
      if(!isInSensorAddressList(sensorFoundAddress)){
        newSensor = true;
//        printAlarm("NEW SENSOR FOUND");
        Serial.println("############ NEW SENSOR FOUND #######################################");
        Serial.println("############ Please add the address into the list SENSOR FOUND ######");
        printAddress(sensorFoundAddress);
        M5.Lcd.println("");
        printAddressDisplay(sensorFoundAddress);
        Serial.println("#####################################################################");
        Serial.println("");      
      }
  }
  if(newSensor){
    M5.update();
    delay(100);
   M5.Lcd.println("Press a button to continue");
   while (!M5.BtnA.wasPressed() && !M5.BtnB.wasPressed() && !M5.BtnC.wasPressed()) {
    delay(100);
    M5.update();
  }
 }
}


/*

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  oneWire.reset_search();

//  setAddress(addr, oneWire);

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  sensors.getAddress(addr, 0);
  Serial.println("");
  Serial.print("Address = ");
  printAddress(addr);
  Serial.println("");
}

void loop(void)
{ 
  float temp = readTemp(addr);
  Serial.print("Temperatur2e = ");
  Serial.println(temp);
}

*/
