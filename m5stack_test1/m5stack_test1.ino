// Define a state machine
#include <M5Stack.h>
#include <time.h>
#include <SD.h>
#include "FS.h"


#include <WebServer.h>

WebServer server(80);

#define NUMSENSORS 15
#define NUMRELAYS 2
#define TEMPINIMIN 12
#define TEMPINIMAX 25
#define MAXTEMPON 55
#define MAXOPTIME 10000
#define MAXHEATINGON 10
#define MINHEATINGOFF 12
#define MAXLOGS 10


unsigned int numreadings;
unsigned int offset;
unsigned int startsessiontime;
unsigned int loopNO;

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid     = "tbtc";
const char* password = "pippo345";

const  char* filename_prefix = "tklog_";
char file_name[30];

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"time1.pi.infn.it");

enum State {Off, Initialized, HeatingOn, HeatingOff, DumpResults};
enum State _state;

char *strStates[5] = {"Off", "Initialized", "HeatingOn", "HeatingOff" ,"DumpResults"};

bool heaters[NUMRELAYS];

class Result{
  public:
    float sensorReadings[NUMSENSORS];
    unsigned int timestamp;
    unsigned int timestate;
    float avgtemp;
    bool relays[NUMRELAYS];
    State state;
};

Result results[MAXLOGS];

void printAlarm(const char* c){
   M5.Lcd.clear(RED);
   M5.Lcd.setTextColor(BLACK);
   M5.Lcd.setTextSize(2);
   M5.Lcd.setCursor(0, 0);
   M5.Lcd.println(c);
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

bool setRelayToOn(int i){
  // FIXME
  heaters[i]=true;
  return true;
}
bool setRelayToOff(int i){
  // FIXME
  heaters[i]=false;
  return true;
}

bool setRelaysToOn(){
    for (int i=0; i< NUMRELAYS; ++i){
      if (setRelayToOn(i) == false){
        printAlarm("Cannot operate on Relay "+(i+'0'));
        Serial.println("Cannot operate on Relay "+(i+'0'));
        return false;
      }
    }
    return true;
}
bool setRelaysToOff(){
    for (int i=0; i< NUMRELAYS; ++i){
      if (setRelayToOff(i) == false){
        printAlarm("Cannot operate on Relay "+(i+'0'));
        return false;
      }
    }
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
    delay(2000);
    M5.update();
    M5.Lcd.print(".");
    if (M5.BtnA.wasReleased()) {

      printMessage("Sequence to start in 2 sec");
      sleep(2);
      initialized=true;
    }
  }
     Serial.println("In initialize2");

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
  M5.Lcd.println();
  M5.Lcd.println();
  M5.Lcd.println("A:Off   B:Hon   C:Hoff");
}

void loopsetup() {
  Serial.println("In loopsetup");
// put your setup code here, to run once:
  // we are starting up

  startsessiontime = timeClient.getEpochTime();
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
  server.begin();
  Serial.println("HTTP Server started.");

  // SD
  Serial.print("Initializing SD card...");

   if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

Result getResult(){
  int tstamp = timeClient.getEpochTime();
  Result r =Result();
  r.timestamp = tstamp;
  r.timestate = tstamp-offset;
  r.state = getState();
  r.avgtemp = readAvgTemp();
  for (int i=0; i< NUMRELAYS; ++i){
   r.relays[i] = heaters[i]; 
  }
  return r;
}

float readAvgTemp(){
//fixme
return 14.;
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
  bool res2 = setRelaysToOn();
     if (res2 == false){
      printAlarm("Cannot set relays to on");
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
  // put your setup code here, to run once:
  M5.begin(true, false, true);
  M5.update();

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
  Result r= getResult();

  if (loopNO % 5000 == 0){
    printResult(r);
    storeResult(r);
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
    bool res2 = reallyGoToHeatingOn();
  }
  if (M5.BtnC.wasPressed()) {
    bool res2 = reallyGoToHeatingOff();
  }
  if (g == Initialized || (g==HeatingOff && (timeClient.getEpochTime()-offset>MINHEATINGOFF))){
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


void handle_OnConnect(){
  server.send(200,"text/html",sendResult(getResult()));
}

String sendResult(Result r){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
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
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
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
    for (int j=0; j<NUMSENSORS; ++j){
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
