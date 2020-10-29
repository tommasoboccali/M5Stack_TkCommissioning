// Define a state machine
#include <M5Stack.h>
#include <time.h>
#include <SD.h>
#include "FS.h"


#include <WebServer.h>

WebServer server(80);

//// RELAY ////
const uint8_t relayChannels[]={
  //GROVE Port A corresponds to GPIO-22 and GPIO-21 https://docs.m5stack.com/#/en/core/m5go?id=pinmap
  22,  // pin-1 gate-A 
//  21,  // pin-2 gate-A 
};
const size_t NUMRELAYS = sizeof(relayChannels)/sizeof(relayChannels[0]);

//// SENSORS ////
#define TEMP_SENSORS_CHANNEL 5

#define  SENSADDRLENGHT 8
const uint8_t sensorAddressList[][SENSADDRLENGHT]={
  {0x28, 0x62, 0x37, 0x30, 0x09, 0x00, 0x00, 0xA0}, // sensor #1
  {0x28, 0xF7, 0x87, 0x30, 0x09, 0x00, 0x00, 0xB5}, // sensor #2
  {0x28, 0xE4, 0x50, 0x30, 0x09, 0x00, 0x00, 0xB8}, // sensor #3
  {0x28, 0xEA, 0x1F, 0x31, 0x09, 0x00, 0x00, 0xA2}, // sensor #4
  {0x28, 0x71, 0xDC, 0x30, 0x09, 0x00, 0x00, 0x08}, // sensor #5
  {0x28, 0x7D, 0xFC, 0x30, 0x09, 0x00, 0x00, 0x8D}, // sensor #6
  {0x28, 0x13, 0x06, 0x30, 0x09, 0x00, 0x00, 0x46}, // sensor #7
  {0x28, 0x48, 0x96, 0x30, 0x09, 0x00, 0x00, 0x27}, // sensor #8
};
const size_t NUMSENSORS = sizeof(sensorAddressList)/sizeof(sensorAddressList[0]);

////////////////////

#define TEMPINIMIN -1000
#define TEMPINIMAX 1000
#define MAXTEMPON 90
#define MAXOPTIME 1000
#define MAXHEATINGON 1000
#define MINHEATINGOFF 100
#define MAXLOGS 10


unsigned int numreadings;
unsigned int offset;
unsigned int lastupdatetime;
unsigned int startsessiontime;
unsigned int loopNO;

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid     = "tbtc";
const char* password = "pippo345";
//const char* ssid     = "InfostradaWiFi-D-2GHz";
//const char* password = "InternetCasaDonat0$";

const  char* filename_prefix = "tklog_";
char file_name[30];

// Define NTP Client to get time
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP,"time1.pi.infn.it");
//NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

enum State {Off, Initialized, Ready,HeatingOn, HeatingOff, DumpResults};
enum State _state;

char *strStates[6] = {"Off", "Initialized", "Ready","HeatingOn", "HeatingOff" ,"DumpResults"};

bool heaters[NUMRELAYS];
float temperatures[NUMSENSORS];

class Result{
  public:
    float sensorReadings[NUMSENSORS];
    unsigned int timestamp;
    unsigned int timestate;
    float avgtemp;
    bool relays[NUMRELAYS];
    State state;
    unsigned int workingSensors;
};

Result results[MAXLOGS];
Result lastResult;

void printAlarm(const char* c){
   M5.Lcd.clear(RED);
   M5.Lcd.setTextColor(BLACK);
   M5.Lcd.setTextSize(2);
   M5.Lcd.setCursor(0, 0);
   M5.Lcd.println(c);
   delay(1000);
}
void printMessage(const char* c){
  M5.Lcd.clear(BLACK);
   M5.Lcd.setTextColor(YELLOW);
   M5.Lcd.setTextSize(2);
   M5.Lcd.setCursor(0, 0);
   M5.Lcd.println(c);
}

bool setState(State s){
  _state = s;
  offset = timeClient.getEpochTime();
  Serial.print("----------------setState ");
  Serial.println(s);
  return true;
}

State getState(){
  return _state;
}

bool setStateFromTo(State f, State t){
  if (getState() != f) {
    return false;
  }
  if (setState(t) == true){
    return true;
  }
  else {
    return false;
  }
}

#include "Temperature.h"
#include "Relay.h"

bool initialize(){
// init lcd, serial, sd
  M5.begin(true, true, true);
   Serial.println("In initialize");
  loopNO=0;
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */

// only operate if state = Off
  if (getState() != Off){
     return false;
  }
  M5.Power.begin();

  printMessage("Btn A for 1 sec to start");
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.setTextColor(RED);
  bool initialized=false;
  while (initialized == false) {
    delay(100);
    M5.update();
    M5.Lcd.print(".");
    if (M5.BtnA.wasPressed()) {
    char buffer[100];
    sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());

      printMessage(buffer);
      sleep(2);
      initialized=true;
    }
  }

  return true;
}

void printResult(Result r){
   M5.Lcd.clear(BLACK);
   M5.Lcd.setTextColor(YELLOW);
   M5.Lcd.setTextSize(3);
   M5.Lcd.setCursor(0, 0);
   M5.Lcd.print ("State ");
   M5.Lcd.println(strStates[getState()]);
   M5.Lcd.print("Since (sec): ");
   char temp[24];
   sprintf(temp,"%d",r.timestate);
   M5.Lcd.println(temp);
   M5.Lcd.print("Total (sec): ");
   sprintf(temp,"%d",r.timestamp-startsessiontime);
   M5.Lcd.println(temp);

   sprintf(temp, "%3.1f", r.avgtemp);
   M5.Lcd.print("Avg.Temp(C): ");
   M5.Lcd.println(temp);
   M5.Lcd.print("N. sens.: ");
   M5.Lcd.print(r.workingSensors);
   M5.Lcd.print("/");
   M5.Lcd.println(NUMSENSORS);
   M5.Lcd.print("Heaters: ");
   for (int i=0; i< NUMRELAYS; ++i){
    if (r.relays[i] == true){
      //Serial.println("relay on");
      temp[i]='1';
    }else{
      temp[i]='0';
    }
   }
  temp[NUMRELAYS]='\0';
    M5.Lcd.println(temp);
    //Serial.print("HEATERS ");
    //Serial.println(temp);

  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println();
  char buffer[100];
  sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  M5.Lcd.println(buffer);
  M5.Lcd.println();
  M5.Lcd.println("A:Off   B:Hon   C:Hoff");
  Serial.println("printResult updated");
}

void loopsetup() {
  Serial.println("In loopsetup");
// put your setup code here, to run once:
  // we are starting up

  startsessiontime = timeClient.getEpochTime();
  lastupdatetime = startsessiontime;
  if (getState() != Off){
    setState(Off);
  }
   initialize();
   if (setStateFromTo(Off,Initialized) == false) {
    printAlarm("Error initializing");
    exit(1);
  } else {
   for (int i=0; i<NUMRELAYS; ++i){
      bool res = setRelayToOff(i);
      if (res == false){
          printAlarm("Error initializing Relay to false. Exiting"+(i+'0'));
          exit (2);
      }
   }
  }
// filename create
unsigned int num=timeClient.getEpochTime();
char temp[40];
itoa(num,temp,10);
strcpy(file_name,"/");
strcat(file_name,filename_prefix);
strcat(file_name,temp);
strcat(file_name,".txt");
Serial.print("SD FileName is ");
Serial.println(file_name);
}

void setup() {
  // put your setup code here, to run once:
  // we are starting up
  M5.Power.begin();
  Serial.begin(115200);
  Serial.println("In setup");
  setState(Off);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Initialize a NTPClient to get time
  timeClient.begin();
  server.on("/",handle_OnConnect);
  server.on("/off",handle_off);
  server.on("/ready",handle_ready);

  server.on("/ls",handle_ls);
  server.on("/dl",handle_dl);
  server.on("/rm",handle_rm);
  server.begin();
  Serial.println("HTTP Server started.");

  // SD
  Serial.print("Initializing SD card...");

   if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  Serial.print("Initializing temperature sensors...");
  setupTemperature();
  Serial.println(" initialization done.");
  Serial.print("Initializing relays...");
  setupRelay();
  Serial.println(" initialization done.");
}

Result getResult(){
  int tstamp = timeClient.getEpochTime();
  Result r =Result();
  r.timestamp = tstamp;
  r.timestate = tstamp-offset;
  r.state = getState();
  updateTemperatures(temperatures);
  for (int i=0; i< NUMRELAYS; ++i){
   r.relays[i] = heaters[i];
  }
  r.workingSensors = 0;
  r.avgtemp = 0.;
  for (int i=0; i< NUMSENSORS; ++i){
   float temp = temperatures[i];
   r.sensorReadings[i] = temp;
   if(temp!=85.&&temp!=-127.&&temp!=-0.5){
    r.workingSensors += 1;
    r.avgtemp+=temp;
   }
   else{
//    temp=-127.; //set error value
  }
  }
  r.avgtemp/=r.workingSensors;
  lastResult = r;
  return r;
}


bool reallyGoToHeatingOn(){
     bool res2 = setRelaysToOn();
     if (res2 == false){
      printAlarm("Cannot set relays to on");
      exit(5);
     }

     bool res3 = setState(HeatingOn);
     if (res3 == false){
      printAlarm("Cannot move to state HeatingOn.");
      exit(6);
     }
    return true;
}



bool goToHeatingOn(){
  // read temp


     float temp = readAvgTemp();
     if (temp<TEMPINIMIN || temp>TEMPINIMAX){
      printAlarm("Initial temperature not within limits.");
      exit (3);
     }
    return reallyGoToHeatingOn();
}


bool reallyGoToHeatingOff(){
  bool res2 = setRelaysToOff();
     if (res2 == false){
      printAlarm("Cannot set relays to off");
      exit(5);
     }

     bool res3 = setState(HeatingOff);
     if (res3 == false){
      printAlarm("Cannot move to state HeatingOn.");
      exit(6);
     }
    return true;
}

bool goToHeatingOff(){
  // read temp
     float temp = readAvgTemp();
     if (temp<TEMPINIMIN || temp>TEMPINIMAX){
      printAlarm("Initial temperature not within limits.");
      exit (3);
     }
     return reallyGoToHeatingOff();
}

bool storeResult(Result r){
  Serial.print("Store results");
  Serial.println(numreadings);
  if (numreadings<MAXLOGS){
    results[numreadings] = r;
    numreadings++;
  }else{
    appendLogs();
    numreadings=0;
  }
  return true;
}


void loop() {
  delay(00);
  server.handleClient();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
    loopNO=loopNO+1;
    if (loopNO%5000 == 0){
  Serial.print("Entering LOOP ");
  Serial.print(strStates[getState()]);
  Serial.print ( "  time: ");
  Serial.print(timeClient.getEpochTime());
  Serial.print ("  TempAvg = ");
  Serial.println(readAvgTemp());
    }

  if (timeClient.getEpochTime()-startsessiontime > MAXOPTIME){
  }
  // refresh screen
  // Handle buttons.
  //A = go Off
  //B = set Heating On
  //C = set Heating Off
  State g=getState();



  if ( g == Off ){
    loopsetup();
  }
  delay(0);
  Result r;

  timeClient.getEpochTime()-lastupdatetime;
  unsigned int startsessiontime;

  // update every ...
  const int updateTime = 1; // ...seconds
  if (timeClient.getEpochTime()-lastupdatetime>=updateTime){
    r =  getResult();
    printResult(r);
    storeResult(r);
    lastupdatetime = timeClient.getEpochTime();
  }


  M5.update();
  if (M5.BtnA.wasPressed()) {
bool res2 = setRelaysToOff();
    if (res2 == false){
      printAlarm("Cannot set relays to off_");
      exit(7);
    }
    bool res3 = setState(Off);
    if (res3 == false){
      printAlarm("Cannot go Off");
      exit(8);
    }
    M5.Lcd.println("going off");
  }
  if (M5.BtnB.wasPressed()) {
    if (g == Initialized) {
       bool res3 = setState(Ready);
    }
    bool res2 = reallyGoToHeatingOn();
  }
  if (M5.BtnC.wasPressed()) {
    bool res2 = reallyGoToHeatingOff();
  }
  if (g == Ready || (g==HeatingOff && (timeClient.getEpochTime()-offset>MINHEATINGOFF))){
     bool res2 = goToHeatingOn();
  }

  if (g == HeatingOn){
    //eventually go to HeatingOff after some time / temperature
    float temp = readAvgTemp();
    if (temp > MAXTEMPON || (timeClient.getEpochTime()-offset>MAXHEATINGON)  ) {
//      Serial.println("Switching heating off");

      bool res2 = goToHeatingOff();
    }
 }
 //done

}




void handle_off(){
  server.send(200,"text/html",sendOff());
}


void handle_ready(){
  server.send(200,"text/html",sendReady());
}
void handle_OnConnect(){
  server.send(200,"text/html",sendResult(getResult()));
}
void handle_ls(){
  server.send(200,"text/html",sendls());
}

void handle_dl(){
  server.send(200,"text/plain",sendFile());
}
void handle_rm(){
  server.send(200,"text/plain",sendRemove());
}

String sendFile(){
  String ptr;
  if (server.hasArg("name")) {
    String name=String(server.arg("name"));
   // open file
    File file = SD.open(name,FILE_READ);
    if(!file){
        Serial.println("Failed to open "+name);
        return ptr;
    }
    while (file.available()){
    int pippo =  file.read();
     ptr+= char(pippo);
     Serial.println(pippo);
    }
    file.close();

  }
  return ptr;
}

String sendReady(){
  String ptr;
  ptr+="Setting System to Ready...<br>\n";
  bool res2 = setRelaysToOff();
  if (res2 == false){
      ptr+="Cannot set relays to off<br>\n";
      return (ptr);
    }
    bool res3 = setState(Ready);
    if (res3 == false){
      ptr+="Cannot set state to Ready<br>\n";
      return (ptr);
    }
    return ptr;
}

String sendOff(){
  String ptr;
  ptr+="Setting System to Initialized...<br>\n";
  bool res2 = setRelaysToOff();
  if (res2 == false){
      ptr+="Cannot set relays to off<br>\n";
      return (ptr);
    }
    bool res3 = setState(Initialized);
    if (res3 == false){
      ptr+="Cannot set state to Intialized<br>\n";
      return (ptr);
    }
  ptr+="Now We are Initialized - you can go back and reload<br>\n";
  return ptr;
}
String sendRemove(){
  String ptr;
  ptr+="Removing file ";
  if (server.hasArg("name")) {
    String name=String(server.arg("name"));
   // open file
   ptr+=name;
   ptr+="  ";
    SD.remove(name);
  }
  return ptr;
}




String sendls(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Tk Tests Pisa</title>\n";

  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
String temp;

   temp=String("LS goes here")+String("<br>\n");
   listDir(ptr,SD, "/",0);
   Serial.println(temp);
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String sendResult(Result r){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Tk Pisa tests</title>\n";
  ptr+="<meta http-equiv=\"refresh\" content=\"5\" >\n";

  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  char buffer[100];
  sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());

  ptr +="<h2>IP ADDRESS: "+String(buffer)+"</h2>\n";


  ptr+= "Timestamp = ";
  ptr+= String(r.timestamp);
  ptr+= "<br>\n";

  ptr+= "State = ";
  ptr+= String(strStates[r.state]);
  ptr+= "<br>\n";

  ptr+= "State since = "+ String(r.timestate)+" sec<br>\n";

  ptr+= "Total time from start = "+ String(r.timestamp-startsessiontime)+" sec<br>\n";

  ptr+= "Average Temperature (C) = "+ String(r.avgtemp)+" sec<br>\n";

  for (int i=0; i< NUMRELAYS; ++i){
       ptr+= "Relay "+String(i)+ "  Status " +String(r.relays[i])+"<br>\n";

  }
  for (int i=0; i< NUMSENSORS; ++i){
       ptr+= "Sensor "+String(i)+ "  Reading (C) " +String(r.sensorReadings[i])+"<br>\n";

  }

ptr+="<br><br><br>\n";
ptr+=" <a href=\"/ls\"> Directory Listing </a> ";
ptr+="<br><br><br>\n";
ptr+=" <a href=\"/off\"> <button>Set System to Initialize (will not start sequence)</button> </a><br><br><br>\n ";
ptr+=" <a href=\"/ready\"> <button>Set System to Ready and Start Automatic Sequence</button> </a> ";


  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;

}

bool printOnFile(File file){
  for (int i=0; i< MAXLOGS; ++i){
    Result r = results[i];
    file.print(r.timestamp);
    file.print(" State ");
    file.print(strStates[r.state]);
    file.print(" timeState ");
    file.print(r.timestate);
    file.print(" heaters ");
    for (int j=0; j<NUMRELAYS; ++j){
       if (r.relays[j] == true){
          file.print('1');
       }else{
          file.print('0');
       }
    }
    file.print(" temp_readings ");
    for (int j=0; j<NUMSENSORS; ++j){
      file.print (r.sensorReadings[j]);
      file.print (" ");
    }
    file.println("");
  }
}

void appendLogs(){
  Serial.println("In AppendLogs");
  File file = SD.open(file_name, FILE_APPEND);

  if(!file) {
    Serial.print("Failed to open file for appending:");
    Serial.println(file_name);
    return;
  }
  Serial.println("Printing in  AppendLogs");

  bool ok  = printOnFile(file);

  file.close();

}
//class Result{
 // public:
  //  float sensorReadings[NUMSENSORS];
   // unsigned int timestamp;
    //unsigned int timestate;
    //float avgtemp;
    //bool relays[NUMRELAYS];
//};


void listDir(String &ptr, fs::FS &fs, const char * dirname, uint8_t levels){

//http://10.0.37.12/dl?name=/tklog_1602755604.txt

  Serial.println("Starting LS");

    ptr+="Listing directory /<br><br><br>\n";;

    File root = fs.open(dirname);
    if(!root){
        ptr+="Failed to open directory<br>\n";
        return;
    }


    if(!root.isDirectory()){
        ptr+="Not a directory<br>\n";
        return;
    }

    File file = root.openNextFile();
    while(file){

        if(file.isDirectory()){
            ptr+="  DIR : ";
            ptr+=String(file.name())+"<br>\n";
            if(levels){
                listDir(ptr,fs, file.name(), levels -1);
            }
        } else {

            ptr+="  FILE: ";
            ptr+=file.name();
            ptr+=String("  SIZE ")+String(file.size())+  "<a href=\"/dl?name="+file.name()+"\"> Download </a>  <a href=\"/rm?name="+file.name()+"\"> Remove </a>  <br>\n";
        }
        file = root.openNextFile();
    }
}

float readAvgTemp(){
  return lastResult.avgtemp;
}
