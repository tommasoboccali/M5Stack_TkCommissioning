/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <M5Stack.h>
#include "tones.h"


#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme2; // I2C
Adafruit_BME280 bme(5); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

#include <WiFi.h>

const char* ssid1     = "CMSPisa";
const char* password1 = "silicon2003";

const char* ssid2     = "tombocap";
const char* password2 = "pippo345";

const char* ssid     = ssid1;
const char* password = password1;

unsigned long delayTime;

float alarmThreshold = 10.;

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


#include <WebServer.h>

WebServer server(80);


String sendResult(Adafruit_BME280 & bme, Adafruit_BME280 & bme2) {
  /*
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

  */

  String ptr = String("{");
  ptr += "\n\"Env\" : ";
  ptr += sensorDataTxt(bme);
  ptr += ",\n\"ColdBox\" :  ";
  ptr += sensorDataTxt(bme2);
  ptr += ",\n\"Diff\" :  ";
  ptr += sensorDataDiffTxt(bme, bme2);
  ptr += "\n}";

  return ptr;

}


void handle_OnConnect() {
  server.send(200, "text/html", sendResult(bme, bme2));
}

void setup() {
    M5.begin();
//    Serial.begin(115200);
//    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    M5.Lcd.fillScreen(WHITE);
    delay(10);
    M5.Power.begin();
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println ("Hello!! ");

    sleep(0.5);


    int retry = connectWiFi(ssid1, password1);
    if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid2, password2);
//    if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid3, password3);
//    if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid4, password4);
//    if (WiFi.status() != WL_CONNECTED) retry = connectWiFi(ssid5, password5);
    
    Serial.println(F("BME280 test"));

    unsigned status,status2;
    
    // default settings
    status = bme.begin();  
    status2 = bme2.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status or !status2) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("SensorID was: 0x"); Serial.println(bme2.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
//        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();

    M5.Lcd.setTextColor(YELLOW);

    server.on("/", handle_OnConnect);
    server.begin();
    Serial.println("HTTP Server started.");
}


double CalculateDewPoint(const double& temp, const int& humidity) {
  //taken from https://www.programmersought.com/article/76165000614/
  if (humidity==0)
    return temp;
  double dew_numer = 243.04*(log(double(humidity)/100.0)+((17.625*temp)/(temp+243.04)));
  double dew_denom = 17.625-log(double(humidity)/100.0)-((17.625*temp)/(temp+243.04));
  if (dew_numer==0)
    dew_numer=1;
  return dew_numer/dew_denom;
}

void printStatusOnDisplay() { 
    auto press1 = bme.readPressure() / 100.0F;
    auto press2 = bme2.readPressure() / 100.0F;
    auto temp1 = bme.readTemperature();
    auto temp2 = bme2.readTemperature();
    auto hum1 = bme.readHumidity();
    auto hum2 = bme2.readHumidity();
    auto dew1 = CalculateDewPoint( bme.readTemperature(),  bme.readHumidity());
    auto dew2 = CalculateDewPoint( bme2.readTemperature(), bme2.readHumidity());

    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);

    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.println ("Cold box");
    M5.Lcd.print (String(press2) + " hPa.  ");
    M5.Lcd.println (String(temp2) + " C");
    M5.Lcd.print (String(hum2) + "%   Dew: ");
    M5.Lcd.println (String(dew2) + " C");
    M5.Lcd.println ("");

    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.println ("Env");
    M5.Lcd.print (String(press1) + " hPa.  ");
    M5.Lcd.println (String(temp1) + " C");
    M5.Lcd.print (String(hum1) + "%   Dew: ");
    M5.Lcd.println (String(dew1) + " C");
    M5.Lcd.println ("");
    
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(4);
    M5.Lcd.println ("Difference");
    M5.Lcd.print (String(press2 - press1) + " hPa.  ");
    M5.Lcd.println ("");
    M5.Lcd.setTextColor(BLUE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println (WiFi.localIP());
    M5.Lcd.setTextColor(ORANGE);
    M5.Lcd.println ("alarmThreshold = " + String(alarmThreshold));
    
//    M5.Lcd.println (String(temp2  - temp1) + " C");
//    M5.Lcd.print (String(hum2 - hum1) + "%. Dew: ");
//    M5.Lcd.println (String(dew2 - dew2) + " C");
}


void loop() { 
  Serial.println("################# Env ############");
    Serial.println(sensorDataTxt(bme));
    Serial.println("################# Cold box ############");
    Serial.println(sensorDataTxt(bme2));
    Serial.println("################# Diff ############");
    Serial.println(sensorDataDiffTxt(bme, bme2));
    delay(delayTime);
    printStatusOnDisplay();
    server.handleClient();
    if(M5.BtnA.wasPressed()) alarmThreshold = alarmThreshold -1;
    if(M5.BtnC.wasPressed()) alarmThreshold = alarmThreshold +1;
    if((bme2.readPressure()-bme.readPressure())/100.0F > alarmThreshold || M5.BtnB.wasPressed()) alarm(); //Alarm if delta pressure is too large
    M5.update();
}

void printValues(Adafruit_BME280 & bme) {
    Serial.println(sensorDataTxt(bme));
}

String sensorDataTxt(Adafruit_BME280 & bme) {
    String txt = "{";
    txt += "\n\"Temperature\" : " + String(bme.readTemperature()) + " ,\n"; //#C
    txt += "\"Pressure\" : " + String(bme.readPressure() / 100.0F) + " ,\n"; //#hPa
    txt += "\"Humidity\" : " + String(bme.readHumidity()) + ",\n"; // # %
    txt += "\"DewPoint\" : " + String(CalculateDewPoint(bme.readTemperature(),bme.readHumidity())) + "\n"; //#C
    txt += "}";
    return txt; 
}

String sensorDataDiffTxt(Adafruit_BME280 & bme1, Adafruit_BME280 & bme2) {
    String txt = "{";
    txt += "\n\"Temperature\" : " + String(bme1.readTemperature() - bme2.readTemperature()) + " ,\n"; //#C
    txt += "\"Pressure\" : " + String(bme1.readPressure() / 100.0F - bme2.readPressure() / 100.0F) + " ,\n"; //#hPa
    txt += "\"Humidity\" : " + String(bme1.readHumidity() - bme2.readHumidity()) + " ,\n"; //#%
    txt += "\"DewPoint\" : " + String(CalculateDewPoint(bme1.readTemperature(),bme1.readHumidity()) - CalculateDewPoint(bme2.readTemperature(),bme2.readHumidity())) + "\n"; // #C
    txt += "}";
    return txt; 
}
