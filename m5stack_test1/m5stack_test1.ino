// Define a state machine
#include <M5Stack.h>
#include <time.h>

#define NUMSENSORS 15
#define NUMRELAYS 2
#define TEMPINIMIN 12
#define TEMPINIMAX 25
#define MAXTEMPON 55
#define MAXOPTIME 10000

unsigned int offset;
unsigned int startsessiontime;

enum State {Off, Initialized, HeatingOn, HeatingOff, DumpResults};
enum State _state;

class Result{
  public:
    float sensorReadings[NUMSENSORS];
    unsigned int timestamp;
    unsigned int timestate;
    bool relays[NUMRELAYS];
};


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
  offset = (unsigned int) time(NULL);
  return true;
}

bool setRelayToOn(int i){
  // FIXME
  return true;
}
bool setRelayToOff(int i){
  // FIXME
  return true;
}

bool setRelaysToOn(){
    for (int i=0; i< NUMRELAYS; ++i){
      if (setRelayToOn(i) == false){
        printAlarm("Cannot operate on Relay "+(i+'0'));
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
// init lcd, serial, but don't init sd card
  M5.begin(true, false, true);
  
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
    
      printMessage("Sequence to start in 5 sec");
      sleep(5);
      initialized=true;
    }
  }
  return true;
}



void loopsetup() {
// put your setup code here, to run once:
  // we are starting up 

  startsessiontime = (unsigned int) time(NULL);
  if (getState() != Off){
    setState(Off);
  }
   initialize();
   printMessage("Initialized");
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
  M5.Lcd.println("here");
}

void setup() {
  // put your setup code here, to run once:
  // we are starting up 
  M5.Power.begin();
  setState(Off);
}

Result getResult(){
  int tstamp = (unsigned) time (NULL);
  Result r =Result();
  r.timestamp = tstamp;
  return r;
  r.timestate = (unsigned int) time(NULL)-offset;
  return r;  
}

float readAvgTemp(){
//fixme
return 12.;
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

bool printResult(Result r){
}
  
bool storeResult(Result r){
}


void loop() {
  delay(200);
  // put your setup code here, to run once:
  M5.begin(true, false, true);
 //M5.Lcd.setCursor(3, 35);
  //M5.Lcd.println("Press buttons");
  //M5.Lcd.setTextColor(RED);
   // M5.update();

//while(1==1){
 // if(M5.BtnA.wasPressed()) M5.Lcd.println("Button A");
  //if(M5.BtnB.wasPressed()) M5.Lcd.println("Button B");
  //if(M5.BtnC.wasPressed()) M5.Lcd.println("Button C");
  //M5.update();
//}
  //
  // time out!
  //

  if ((unsigned int) time(NULL)-startsessiontime > MAXOPTIME){
  }
  // refresh screen
  // Handle buttons.
  //A = go Off
  //B = set Heating On
  //C = set Heating Off
  State g=getState();


  M5.Lcd.println("here2");
  
  if ( g == Off ){
    loopsetup();
  }
  M5.Lcd.println("here3");
  delay(2000);
  Result r= getResult();  

  printResult(r);
  storeResult(r);
M5.update();
  if (M5.BtnA.wasPressed()) {
bool res2 = setRelaysToOff();
    if (res2 == false){
      printAlarm("Cannot set relays to off");
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

  if (g == Initialized){
     bool res2 = goToHeatingOn();
     }
 
  if (g == HeatingOn){
    //eventually go to HeatingOff after some time / temperature
    float temp = readAvgTemp();
    if (temp > MAXTEMPON) {
      bool res2 = goToHeatingOff();
    }
 }
 //done
 
}
