// Define a state machine
#include <M5Stack.h>
#include <time.h>
#include <SD.h>
#include "FS.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <WebServer.h>

WebServer server(80);

//// RELAY ////
const uint8_t relayChannels[] = {
  //GROVE Port A corresponds to GPIO-22 and GPIO-21 https://docs.m5stack.com/#/en/core/m5go?id=pinmap
  22,  // pin-1 gate-A
  21,  // pin-2 gate-A
  16,  // pin-1 gate-C
  17,  // pin-2 gate-C
};
const size_t NUMRELAYS = sizeof(relayChannels) / sizeof(relayChannels[0]);

//// SENSORS ////
#define TEMP_SENSORS_CHANNEL 26 // pin white gate-B

#define  SENSADDRLENGHT 8
const uint8_t sensorAddressList[][SENSADDRLENGHT] = {
  {0x28, 0x62, 0x37, 0x30, 0x09, 0x00, 0x00, 0xA0}, // sensor #0
  {0x28, 0xF7, 0x87, 0x30, 0x09, 0x00, 0x00, 0xB5}, // sensor #1
  {0x28, 0xE4, 0x50, 0x30, 0x09, 0x00, 0x00, 0xB8}, // sensor #2
  {0x28, 0xEA, 0x1F, 0x31, 0x09, 0x00, 0x00, 0xA2}, // sensor #3
  {0x28, 0x71, 0xDC, 0x30, 0x09, 0x00, 0x00, 0x08}, // sensor #4
  {0x28, 0x7D, 0xFC, 0x30, 0x09, 0x00, 0x00, 0x8D}, // sensor #5
  {0x28, 0x13, 0x06, 0x30, 0x09, 0x00, 0x00, 0x46}, // sensor #6
  {0x28, 0x48, 0x96, 0x30, 0x09, 0x00, 0x00, 0x27}, // sensor #7
};
const size_t NUMSENSORS = sizeof(sensorAddressList) / sizeof(sensorAddressList[0]);

////////////////////

#define TEMPINIMIN -1000
#define TEMPINIMAX 1000
#define MAXTEMPON 9000000
#define MAXOPTIME 100000000
#define MAXHEATINGON 100000000
#define MINHEATINGOFF 100000000
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
const char* ssid1     = "tb-e";
const char* password1 = "";
const char* ssid2     = "tombocap";
const char* password2 = "pippo345";
const char* ssid3     = "tbtc";
const char* password3 = "pippo345";
const char* ssid4     = "InfostradaWiFi-D-2GHz";
const char* password4 = "InternetCasaDonat0$";
const char* ssid5     = "MEGA";
const char* password5 = "13801879";

const char* ssid     = ssid1;
const char* password = password1;

const  char* filename_prefix = "tklog_";
char file_name[30];

// Define NTP Client to get time
WiFiUDP ntpUDP;

const char* NTPClientSite1 = "europe.pool.ntp.org";
const char* NTPClientSite2 = "time1.pi.infn.it";
const char* NTPClientSite3 = "";
const char* NTPClientSite = NTPClientSite2;
NTPClient timeClient(ntpUDP, NTPClientSite, 3600, 60000);
bool timeClientStatus = false; // true = ok

enum State {Off, Initialized, Ready, HeatingOn, HeatingOff, DumpResults};
enum State _state;

char *strStates[6] = {"Off", "Initialized", "Ready", "HeatingOn", "HeatingOff" , "DumpResults"};

bool heaters[NUMRELAYS];
float temperatures[NUMSENSORS];

class Result {
  public:
    float sensorReadings[NUMSENSORS];
    unsigned int timestamp;
    unsigned int timestate;
    float maxtemp;
    bool relays[NUMRELAYS];
    State state;
    unsigned int workingSensors;
};

Result results[MAXLOGS];
Result lastResult;

/*
int sens_up_1 = -1;
int sens_up_2 = -1;
int sens_up_3 = -1;
int sens_up_4 = -1;
int sens_down_1 = -1;
int sens_down_2 = -1;
int sens_down_3 = -1;
int sens_down_4 = -1;
*/

void updateFileName(){
  unsigned int num = timeClient.getEpochTime();
  char temp[40];
  itoa(num, temp, 10);
  strcpy(file_name, "/");
  strcat(file_name, filename_prefix);
  strcat(file_name, temp);
  strcat(file_name, ".txt");
  Serial.print("SD FileName is ");
  Serial.println(file_name);
}

void printAlarm(const char* c) {
  M5.Lcd.clear(RED);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(c);
  delay(1000);
}
void printMessage(const char* c) {
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(c);
}

bool setState(State s) {
  _state = s;
  offset = timeClient.getEpochTime();
  Serial.print("----------------setState ");
  Serial.println(s);
  return true;
}

State getState() {
  return _state;
}

bool setStateFromTo(State f, State t) {
  if (getState() != f) {
    return false;
  }
  if (setState(t) == true) {
    return true;
  }
  else {
    return false;
  }
}

#include "Temperature.h"
#include "Relay.h"

bool initialize() {
  // init lcd, serial, sd
  M5.begin(true, true, true);
  Serial.println("In initialize");
  loopNO = 0;
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */

  // only operate if state = Off
  if (getState() != Off) {
    return false;
  }
  M5.Power.begin();

  printMessage("Btn A for 1 sec to start; Btn B (now!) for OTA");
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.setTextColor(RED);
  bool initialized = false;
  while (initialized == false) {
    delay(100);
    M5.update();
    M5.Lcd.print(".");
    if (M5.BtnB.wasPressed()) {
      M5.Lcd.println("OTA");
      while (true) {
        ArduinoOTA.handle();

      }

    }
    if (M5.BtnA.wasPressed()) {
      char buffer[100];
      sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());

      printMessage(buffer);
      sleep(2);
      initialized = true;
    }
  }

  return true;
}

bool getTimeClientStatus(const char* NTPClientSite) {
  {
    M5.Lcd.println(NTPClientSite);
    if(NTPClientSite==""){timeClient = NTPClient (ntpUDP);}
    else{timeClient = NTPClient (ntpUDP, NTPClientSite, 3600, 60000);}
    timeClient.forceUpdate();
    ArduinoOTA.handle();
    timeClientStatus = timeClient.update();
    return timeClientStatus;
  }
}

void printResult(Result r) {
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print ("State ");
  M5.Lcd.println(strStates[getState()]);
  M5.Lcd.print("Since (sec): ");
  char temp[24];
  sprintf(temp, "%d", r.timestate);
  M5.Lcd.println(temp);
  M5.Lcd.print("Total (sec): ");
  sprintf(temp, "%d", r.timestamp - startsessiontime);
  M5.Lcd.println(temp);

  sprintf(temp, "%3.1f", r.maxtemp);
  M5.Lcd.print("MaxTemp(C): ");
  M5.Lcd.println(temp);
  M5.Lcd.print("N. sens.: ");
  M5.Lcd.print(r.workingSensors);
  M5.Lcd.print("/");
  M5.Lcd.println(NUMSENSORS);
  M5.Lcd.print("Heaters: ");
  for (int i = 0; i < NUMRELAYS; ++i) {
    if (r.relays[i] == true) {
      //Serial.println("relay on");
      temp[i] = '1';
    } else {
      temp[i] = '0';
    }
  }
  temp[NUMRELAYS] = '\0';
  M5.Lcd.println(temp);
  //Serial.print("HEATERS ");
  //Serial.println(temp);
  int columns=0;
  M5.Lcd.println("Temperatures: ");
  for (int i = 0; i < NUMSENSORS; ++i) {
    M5.Lcd.print(i);
    M5.Lcd.print(":");
    M5.Lcd.print(r.sensorReadings[i]);
    columns++;
    if(columns>=2){
      columns=0;
     M5.Lcd.println("");
      }
    else{
     M5.Lcd.print("    ");
      }
  }
  temp[NUMRELAYS] = '\0';

  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println();
  char buffer[100];
  sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
  M5.Lcd.print(buffer);
  M5.Lcd.println(file_name);
  M5.Lcd.println("A:Off   B:Hon   C:Hoff");
  Serial.println("printResult updated");
}

void loopsetup() {
  M5.Lcd.println("In loopsetup");
  Serial.println("In loopsetup");
  // put your setup code here, to run once:
  // we are starting up

  startsessiontime = timeClient.getEpochTime();
  lastupdatetime = startsessiontime;
  if (getState() != Off) {
    setState(Off);
  }
  initialize();
  if (setStateFromTo(Off, Initialized) == false) {
    printAlarm("Error initializing");
    exit(1);
  } else {
    for (int i = 0; i < NUMRELAYS; ++i) {
      bool res = setRelayToOff(i);
      if (res == false) {
        printAlarm("Error initializing Relay to false. Exiting" + (i + '0'));
        exit (2);
      }
    }
    M5.Lcd.println("End loopsetup");
  }
  // filename create
  updateFileName();
}


void initNTP(){
  M5.Lcd.println("3");
  timeClientStatus = timeClient.update();
  while (!timeClientStatus) {
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Press button A to skip NTP time");

    if (M5.BtnA.wasPressed()) break;
    M5.update();
    if (M5.BtnA.wasPressed()) break;
    delay(100);
    if (M5.BtnA.wasPressed()) break;

    M5.Lcd.print(".");
    timeClientStatus = getTimeClientStatus(NTPClientSite1);
    M5.Lcd.print(".");
    if (timeClientStatus) break;

    if (M5.BtnA.wasPressed()) break;
    M5.update();
    if (M5.BtnA.wasPressed()) break;
    delay(100);
    if (M5.BtnA.wasPressed()) break;

    M5.Lcd.print(".");
    timeClientStatus = getTimeClientStatus(NTPClientSite2);
    M5.Lcd.print(".");
    if (timeClientStatus) break;

    if (M5.BtnA.wasPressed()) break;
    M5.update();
    if (M5.BtnA.wasPressed()) break;
    delay(100);
    if (M5.BtnA.wasPressed()) break;

    M5.Lcd.print(".");
    timeClientStatus = getTimeClientStatus(NTPClientSite3);
    M5.Lcd.print(".");
    if (timeClientStatus) break;

    ArduinoOTA.handle();

    if (M5.BtnA.wasPressed()) break;
    M5.update();
    if (M5.BtnA.wasPressed()) break;
    delay(100);
    if (M5.BtnA.wasPressed()) break;
  }  
}


int connectWiFi (const char* ssid, const char* password){
    Serial.print("Connecting to ");
    Serial.println(ssid);

    M5.Lcd.println("");
    M5.Lcd.print(ssid);
    M5.Lcd.print(".");
    M5.Lcd.print(password);
    M5.Lcd.print(".");

    if (password == "") {
      WiFi.begin(ssid);
    } else {
      WiFi.begin(ssid, password);
    }

    int retry = 10;
    M5.Lcd.print(".");
    while (WiFi.status() != WL_CONNECTED && retry>0) {
      delay(500);
      M5.Lcd.print(".");
      Serial.print(".");
      retry--;
    }
    M5.Lcd.println("");
    return retry;
}

void setup() {
  // put your setup code here, to run once:
  // we are starting up
  M5.begin();
  M5.Lcd.fillScreen(WHITE);
  delay(100);
  M5.Power.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println ("Hello!! ");
  sleep(2);

  ssid = ssid1;
  password = password1;

  Serial.begin(115200);
  Serial.println("In setup");
  setState(Off);
  Serial.print("Connecting to ");
  Serial.println(ssid);


  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print ("Hello! ");
  M5.Lcd.print (NTPClientSite);
  M5.Lcd.println();

  int retry = connectWiFi(ssid1, password1);
  if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid2, password2);
  if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid3, password3);
  if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid4, password4);
  if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid5, password5);

  if (retry == 0) {
    M5.Lcd.println("Starting M5");
    WiFi.softAP("M5");
  }
  else {
    M5.Lcd.println("Starting timeClient");
    timeClient.begin();
  }

  M5.Lcd.println("DONE");


  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("M5cms");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  M5.Lcd.println("ArduinoOTA.begin() 1");

  ArduinoOTA.begin();
  //doesn't work without internet, so we just wait for OTA
  if (retry == 0) while (true)   ArduinoOTA.handle();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  M5.Lcd.println("ArduinoOTA.begin() 2");

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.softAPIP());
  // Initialize a NTPClient to get time

  server.on("/", handle_OnConnect);
  server.on("/off", handle_off);
  server.on("/ready", handle_ready);
  server.on("/newfile", handle_newfile);
  server.on("/addcomment", handle_addcomment);
//  server.on("/setupsensors", handle_setupsensors);

  server.on("/ls", handle_ls);
  server.on("/dl", handle_dl);
  server.on("/rm", handle_rm);
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

  M5.Lcd.println("initNTP();");
  initNTP();
  M5.Lcd.println("Exiting setup()");
}

Result getResult() {
  int tstamp = timeClient.getEpochTime();
  Result r = Result();
  r.timestamp = tstamp;
  r.timestate = tstamp - offset;
  r.state = getState();
  updateTemperatures(temperatures);
  for (int i = 0; i < NUMRELAYS; ++i) {
    r.relays[i] = heaters[i];
  }
  r.workingSensors = 0;
  r.maxtemp = -99.;
  for (int i = 0; i < NUMSENSORS; ++i) {
    float temp = temperatures[i];
    r.sensorReadings[i] = temp;
    if (temp != 85. && temp != -127. && temp != -0.5 && temp != 0.0) {
      r.workingSensors += 1;
      r.maxtemp += max(temp,r.maxtemp);
    }
    else {
      //    temp=-127.; //set error value
    }
  }
  lastResult = r;
  return r;
}


bool reallyGoToHeatingOn() {
  bool res2 = setRelaysToOn();
  if (res2 == false) {
    printAlarm("Cannot set relays to on");
    exit(5);
  }

  bool res3 = setState(HeatingOn);
  if (res3 == false) {
    printAlarm("Cannot move to state HeatingOn.");
    exit(6);
  }
  return true;
}



bool goToHeatingOn() {
  // read temp


  float temp = readMaxTemp();
  if (temp < TEMPINIMIN || temp > TEMPINIMAX) {
    printAlarm("Initial temperature not within limits.");
    exit (3);
  }
  return reallyGoToHeatingOn();
}


bool reallyGoToHeatingOff() {
  bool res2 = setRelaysToOff();
  if (res2 == false) {
    printAlarm("Cannot set relays to off");
    exit(5);
  }

  bool res3 = setState(HeatingOff);
  if (res3 == false) {
    printAlarm("Cannot move to state HeatingOn.");
    exit(6);
  }
  return true;
}

bool goToHeatingOff() {
  // read temp
  float temp = readMaxTemp();
  if (temp < TEMPINIMIN || temp > TEMPINIMAX) {
    printAlarm("Initial temperature not within limits.");
    exit (3);
  }
  return reallyGoToHeatingOff();
}

bool storeResult(Result r) {
  Serial.print("Store results");
  Serial.println(numreadings);
  if (numreadings < MAXLOGS) {
    results[numreadings] = r;
    numreadings++;
  } else {
    appendLogs();
    numreadings = 0;
  }
  return true;
}

void loop() {
  M5.Lcd.println("Loop");
  ArduinoOTA.handle();


  M5.Lcd.println("1");
  delay(00);
  M5.Lcd.println("2");
  server.handleClient();

//  initNTP();

  M5.Lcd.println("4");
  loopNO = loopNO + 1;
if (loopNO % 5000 == 0) {
  M5.Lcd.println("Entering LOOP ");
  Serial.print("Entering LOOP ");
  Serial.print(strStates[getState()]);
  Serial.print ( "  time: ");
  Serial.print(timeClient.getEpochTime());
  Serial.print ("  MaxTemp = ");
  Serial.println(readMaxTemp());
}

if (timeClient.getEpochTime() - startsessiontime > MAXOPTIME) {
}
// refresh screen
// Handle buttons.
//A = go Off
//B = set Heating On
//C = set Heating Off
State g = getState();



if ( g == Off ) {
  M5.Lcd.println("loopsetup()");
  loopsetup();
}
delay(0);
Result r;

timeClient.getEpochTime() - lastupdatetime;
unsigned int startsessiontime;

// update every ...
const int updateTime = 1; // ...seconds
if (timeClient.getEpochTime() - lastupdatetime >= updateTime) {
  r =  getResult();
  printResult(r);
  storeResult(r);
  lastupdatetime = timeClient.getEpochTime();
}


M5.update();

if (M5.BtnA.wasPressed()) {
  bool res2 = setRelaysToOff();
  if (res2 == false) {
    printAlarm("Cannot set relays to off_");
    exit(7);
  }
  bool res3 = setState(Off);
  if (res3 == false) {
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
if (g == Ready || (g == HeatingOff && (timeClient.getEpochTime() - offset > MINHEATINGOFF))) {
  bool res2 = goToHeatingOn();
}

if (g == HeatingOn) {
  //eventually go to HeatingOff after some time / temperature
  float temp = readMaxTemp();
  if (temp > MAXTEMPON || (timeClient.getEpochTime() - offset > MAXHEATINGON)  ) {
    //      Serial.println("Switching heating off");

    bool res2 = goToHeatingOff();
  }
}
//done

}




void handle_off() {
  server.send(200, "text/html", sendOff());
}


void handle_newfile() {
  server.send(200, "text/html", sendNewFile());
}

void handle_addcomment() {
  server.send(200, "text/html", sendAddComment());
}

/*
void handle_setupsensors() {
  server.send(200, "text/html", sendSetupSensors());
}
*/

void handle_ready() {
  server.send(200, "text/html", sendReady());
}
void handle_OnConnect() {
  server.send(200, "text/html", sendResult(getResult()));
}
void handle_ls() {
  server.send(200, "text/html", sendls());
}

void handle_dl() {
  server.send(200, "text/plain", sendFile());
}
void handle_rm() {
  server.send(200, "text/html", sendRemove());
}

String sendFile() {
  String ptr;
  if (server.hasArg("name")) {
    String name = String(server.arg("name"));
    // open file
    File file = SD.open(name, FILE_READ);
    if (!file) {
      Serial.println("Failed to open " + name);
      return ptr;
    }
    while (file.available()) {
      int pippo =  file.read();
      ptr += char(pippo);
      Serial.println(pippo);
    }
    file.close();

  }
  return ptr;
}


String sendAddComment() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =ls\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "Adding comment: ";
  String comment = "";
  if (server.hasArg("comment")) {
    comment = String(server.arg("comment"));
    // open file
    ptr += comment;
    ptr += " ";
  }
  ptr += "\n";
  ptr += "</body>\n";
  ptr += "</html>\n";

  File file = SD.open(file_name, FILE_APPEND);
  printCommentOnFile(file, comment);
  file.close();
  return ptr;
}

/*
String sendSetupSensors() {
  sens_up_1 = atoi(server.arg("sens_up_1").c_str());
  sens_up_2 = atoi(server.arg("sens_up_2").c_str());
  sens_up_3 = atoi(server.arg("sens_up_3").c_str());
  sens_up_4 = atoi(server.arg("sens_up_4").c_str());

  sens_down_1 = atoi(server.arg("sens_down_1").c_str());
  sens_down_2 = atoi(server.arg("sens_down_2").c_str());
  sens_down_3 = atoi(server.arg("sens_down_3").c_str());
  sens_down_4 = atoi(server.arg("sens_down_4").c_str());
  
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
//  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =.\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "Updated sensors map: \n <br>";

  ptr += "\n top";
  
  if(sens_up_1>=0){ ptr += String(sens_up_1)+" ";};
  if(sens_up_2>=0){ ptr += String(sens_up_2)+" ";};
  if(sens_up_3>=0){ ptr += String(sens_up_3)+" ";};
  if(sens_up_4>=0){ ptr += String(sens_up_4)+" ";};

  ptr += "\n <br> ";


  ptr += "\nbottom: ";
  
  if(sens_down_1>=0){ ptr += String(sens_down_1)+" ";};
  if(sens_down_2>=0){ ptr += String(sens_down_2)+" ";};
  if(sens_down_3>=0){ ptr += String(sens_down_3)+" ";};
  if(sens_down_4>=0){ ptr += String(sens_down_4)+" ";};

  ptr += "\n";
  
  return ptr;
}
*/

String sendNewFile() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =.\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "I'm creating a new file... <br>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
 
  updateFileName();
  return ptr;
}

String sendReady() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =.\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "Setting System to Ready...<br>\n";
  bool res2 = setRelaysToOff();
  if (res2 == false) {
    ptr += "Cannot set relays to off<br>\n";
    return (ptr);
  }
  bool res3 = setState(Ready);
  if (res3 == false) {
    ptr += "Cannot set state to Ready<br>\n";
    return (ptr);
  }
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

String sendOff() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =.\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "Setting System to Initialized...<br>\n";
  bool res2 = setRelaysToOff();
  if (res2 == false) {
    ptr += "Cannot set relays to off<br>\n";
    return (ptr);
  }
  bool res3 = setState(Initialized);
  if (res3 == false) {
    ptr += "Cannot set state to Intialized<br>\n";
    return (ptr);
  }
  ptr += "Now We are Initialized - you can go back and reload<br>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
String sendRemove() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
  ptr += "<meta http-equiv = \"refresh\" content = \"1; url =ls\"  />  ";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "Removing file ";
  if (server.hasArg("name")) {
    String name = String(server.arg("name"));
    // open file
    ptr += name;
    ptr += " ";
    SD.remove(name);
  }
  ptr += "\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}




String sendls() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Tk Tests Pisa</title>\n";

  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  String temp;

  temp = String("LS goes here") + String("<br>\n");
  listDir(ptr, SD, "/", 0);
  Serial.println(temp);
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

String sendResult(Result r) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Tk Pisa tests</title>\n";
  ptr += "<meta http-equiv=\"refresh\" content=\"5\" >\n";

  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Web Server</h1>\n";
  char buffer[100];
  sprintf(buffer, "IP %s\n", WiFi.localIP().toString().c_str());

  ptr += "<h2>IP ADDRESS: " + String(buffer) + "</h2>\n";

  ptr += "Filename = ";
  ptr += String(file_name);
  ptr += "<br>\n";

  ptr += "Timestamp = ";
  ptr += String(r.timestamp);
  ptr += "<br>\n";

  ptr += "State = ";
  ptr += String(strStates[r.state]);
  ptr += "<br>\n";

  ptr += "State since = " + String(r.timestate) + " sec<br>\n";

  ptr += "Total time from start = " + String(r.timestamp - startsessiontime) + " sec<br>\n";

  ptr += "Max Temperature (C) = " + String(r.maxtemp) + " <br>\n";

  for (int i = 0; i < NUMRELAYS; ++i) {
    ptr += "Relay " + String(i) + "  Status " + String(r.relays[i]) + "<br>\n";

  }
  for (int i = 0; i < NUMSENSORS; ++i) {
    ptr += "Sensor " + String(i) + "  Reading (C) " + String(r.sensorReadings[i]) + "<br>\n";

  }

  ptr += "<br>\n";
  ptr += "<form action=\"/addcomment\">\n";
  ptr += "<label for=\"comment\">Comment:</label><br>\n";
  ptr += "<input type=\"text\" id=\"comment\" name=\"comment\" onFocus=\"this.select()\"><br>\n";
  ptr += "<input type=\"submit\" value=\"Submit\"> <br>\n";
  ptr += "</form> <br>\n";

  ptr += " <a href=\"/ls\"> Directory Listing </a> ";
  ptr += "<br><br><br>\n";
  ptr += " <a href=\"/off\"> <button>Set System to Initialize (will not start sequence)</button> </a><br><br><br>\n ";
  ptr += " <a href=\"/ready\"> <button>Set System to Ready and Start Automatic Sequence</button> </a><br><br><br>\n ";
  ptr += " <a href=\"/newfile\"> <button>Start a new file</button> </a><br><br><br>\n ";

/*
  ptr += "<br>\n";
  ptr += "<form action=\"/setupsensors\">\n";
  ptr += "<label for=\"sens_up_1\">Sensors top   :</label><br>\n";
  ptr += "<input type=\"text\" id=\"sens_up_1\" name=\"sens_up_1\" value=" + String(sens_up_1) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_up_2\" name=\"sens_up_2\" value=" + String(sens_up_2) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_up_3\" name=\"sens_up_3\" value=" + String(sens_up_3) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_up_4\" name=\"sens_up_4\" value=" + String(sens_up_4) + "><br>\n";
  ptr += "<label for=\"sens_down_1\">Sensors bottom:</label><br>\n";
  ptr += "<input type=\"text\" id=\"sens_down_1\" name=\"sens_down_1\" value=" + String(sens_down_1) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_down_2\" name=\"sens_down_2\" value=" + String(sens_down_2) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_down_3\" name=\"sens_down_3\" value=" + String(sens_down_3) + ">\n";
  ptr += "<input type=\"text\" id=\"sens_down_4\" name=\"sens_down_4\" value=" + String(sens_down_4) + ">\n";
  ptr += "<br> <input type=\"submit\" value=\"Setup sensors\"> <br>\n";
  ptr += "</form> <br>\n";
*/

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;

}

bool printCommentOnFile(File file, String comment) {
  int i = 0;
  Result r = results[i];
  file.print(r.timestamp);
  file.print(" Log ");
  file.print(comment.c_str());
  file.println("");
}

bool printOnFile(File file) {
  for (int i = 0; i < MAXLOGS; ++i) {
    Result r = results[i];
    file.print(r.timestamp);
    file.print(" State ");
    file.print(strStates[r.state]);
    file.print(" timeState ");
    file.print(r.timestate);
    file.print(" heaters ");
    for (int j = 0; j < NUMRELAYS; ++j) {
      if (r.relays[j] == true) {
        file.print('1');
      } else {
        file.print('0');
      }
    }
    file.print(" temp_readings ");
    for (int j = 0; j < NUMSENSORS; ++j) {
      file.print (r.sensorReadings[j]);
      file.print (" ");
    }
    file.println("");
  }
}

void appendLogs() {
  Serial.println("In AppendLogs");
  File file = SD.open(file_name, FILE_APPEND);

  if (!file) {
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
//float maxtemp;
//bool relays[NUMRELAYS];
//};


void listDir(String &ptr, fs::FS &fs, const char * dirname, uint8_t levels) {

  //http://10.0.37.12/dl?name=/tklog_1602755604.txt

  Serial.println("Starting LS");

  ptr += "Listing directory /<br><br><br>\n";;

  File root = fs.open(dirname);
  if (!root) {
    ptr += "Failed to open directory<br>\n";
    return;
  }


  if (!root.isDirectory()) {
    ptr += "Not a directory<br>\n";
    return;
  }

  File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      ptr += "  DIR : ";
      ptr += String(file.name()) + "<br>\n";
      if (levels) {
        listDir(ptr, fs, file.name(), levels - 1);
      }
    } else {

      ptr += "  FILE: ";
      ptr += file.name();
      ptr += String("  SIZE ") + String(file.size()) +  "<a href=\"/dl?name=" + file.name() + "\"> Download </a>  <a href=\"/rm?name=" + file.name() + "\"> Remove </a>  <br>\n";
    }
    file = root.openNextFile();
  }
}

float readMaxTemp() {
  return lastResult.maxtemp;
}
