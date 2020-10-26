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

float readTemp(const byte addr[SENSADDRLENGHT]){
  byte data[12];

  oneWire.reset_search();  
  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(750);     // maybe 750ms is enough, maybe not
  
  oneWire.reset();
  oneWire.select(addr);    
  oneWire.write(0xBE);         // Read Scratchpad

  for (size_t i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = oneWire.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  raw = raw & ~7;  // 9 bit resolution, 93.75 ms
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

void updateTemperatures(float temperatures[NUMSENSORS]){
  for(size_t i=0; i<NUMSENSORS; i++){
    temperatures[i] = readTemp(sensorAddressList[i]);
    printAddress(sensorAddressList[i]);
    Serial.println(temperatures[i]);
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
  for(size_t i=0; i<nSensors; i++){
      sensors.getAddress(sensorFoundAddress, i);
      Serial.println("");
      Serial.print("Found sensors with address ");
      printAddress(sensorFoundAddress);
      Serial.println("");
      if(!isInSensorAddressList(sensorFoundAddress)){
//        printAlarm("NEW SENSOR FOUND");
        Serial.println("############ NEW SENSOR FOUND #######################################");
        Serial.println("############ Please add the address into the list SENSOR FOUND ######");
        printAddress(sensorFoundAddress);
        Serial.println("#####################################################################");
        Serial.println("");      
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
