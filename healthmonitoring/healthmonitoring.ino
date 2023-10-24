#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "MAX30100_PulseOximeter.h"
#include "html.h"

ESP8266WebServer server(80);

const char* ssid = "LEVEL ZERO";    /*Enter Your SSID*/
const char* password = "smuct#17"; /*Enter Your Password*/

#define REPORTING_PERIOD_MS     1000

// Creating object for PulseOximeter class as pox
PulseOximeter pox;

uint32_t tsLastReport = 0;
float HeartRate,SpO;

static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;
int  m = 9740;
int y = 71;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin); 
SoftwareSerial SIM800l(7, 8);
int Buzzer = 4; 
String textForSMS;
int Switch = 5; 

String datareal;
String dataimaginary;
String combined;
int raw = 1000000;

String datareal2;
String dataimaginary2;
String combined2;

double longitude;
double latitude;

void onBeatDetected()
{
    Serial.println("Beat!");
}

void MainPage() {
  String _html_page = html_page;              /*Read The HTML Page*/
  server.send(200, "text/html", _html_page);  /*Send the code to the web server*/
}

void MAX30100() {
  String data = "[\""+String(HeartRate)+"\",\""+String(SpO)+"\"]";
  server.send(200, "text/plane", data);
}

void setup()
{
  Serial.begin(115200);
  SIM800l.begin(9600);
  //Serial.begin(9600);
  ss.begin(GPSBaud);
  WiFi.mode(WIFI_STA);                  /*Set the WiFi in STA Mode*/
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  delay(10000); 
  Serial.println(" logging time completed!");
  while(WiFi.waitForConnectResult() != WL_CONNECTED){Serial.print(".");} /*Wait while connecting to WiFi*/
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("Your Local IP address is: ");
    Serial.println(WiFi.localIP());       /*Print the Local IP*/
   
    server.on("/", MainPage);             /*Display the Web/HTML Page*/
    server.on("/readmax30100", MAX30100); /*Display the updated Distance value(CM and INCH)*/
    server.begin();                       /*Start Server*/
    delay(1000);                          /*Wait for 1000mS*/


    Serial.print("Initializing pulse oximeter..");


    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

  pox.setOnBeatDetectedCallback(onBeatDetected);

  randomSeed(analogRead(0));
  pinMode(Switch, INPUT);
  digitalWrite(Switch, HIGH);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);

  Serial.println(F("DeviceExample.ino"));
  Serial.print(F("Testing TinyGPS++ library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());

  Serial.println();
}



void sendSMS(String message)
{
  SIM800l.print("AT+CMGF=1\r");                     
  delay(100);
  SIM800l.println("AT + CMGS = \"+8801941271076\"");  
  delay(100);
  SIM800l.println(message);                         
  delay(100);
  SIM800l.println((char)26);                        
  delay(100);
  SIM800l.println();
  delay(5000);                                     

}

void makecall()
{
  SIM800l.println("ATD+ +8801941271076;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000); // wait for 20 seconds...
  SIM800l.println("ATH"); //hang up
  updateSerial();
}

void loop()
{
    // Make sure to call update as fast as possible
  pox.update();
  server.handleClient();
  int reading;

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      
        HeartRate = pox.getHeartRate();
        Serial.print("Heart rate:");
        Serial.print(HeartRate);
        
        SpO = pox.getSpO2();
        Serial.print("bpm / SpO2:");
        Serial.print(SpO);
        Serial.println("%");

        tsLastReport = millis();
    }
  
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
  
  if (digitalRead(Switch) == LOW)
  {
    makecall();
    displayInfo();
    latitude = gps.location.lat(), 6 ;
    longitude = gps.location.lng(), 6 ;
    long datareal = int(latitude);
    int shakil = ( latitude - datareal) * 100000;
    long datareal2 = int(longitude);
    int shakil2 = (longitude - datareal2 ) * 100000;
    textForSMS.concat(shakil);
    //textForSMS = "Longitude:  ";
    textForSMS.concat(datareal2);
    textForSMS = textForSMS + ".";
    textForSMS.concat(shakil2);
    //textForSMS = textForSMS + " Latitude: ";
    textForSMS.concat(datareal);
    textForSMS = textForSMS + ".";
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("message sent.");
    delay(5000);
  }

  else if(HeartRate>=170 || HeartRate<=40)
  {
    makecall();
    displayInfo();
    latitude = gps.location.lat(), 6 ;
    longitude = gps.location.lng(), 6 ;
    long datareal = int(latitude);
    int shakil = ( latitude - datareal) * 100000;
    long datareal2 = int(longitude);
    int shakil2 = (longitude - datareal2 ) * 100000;
    textForSMS.concat(shakil);
    //textForSMS = "Longitude:  ";
    textForSMS.concat(datareal2);
    textForSMS = textForSMS + ".";
    textForSMS.concat(shakil2);
    //textForSMS = textForSMS + " Latitude: ";
    textForSMS.concat(datareal);
    textForSMS = textForSMS + ".";
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("message sent.");
    delay(5000);
  }

  else
    digitalWrite(Switch, HIGH);
  digitalWrite(Buzzer, LOW);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    SIM800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SIM800l.available()) 
  {
    Serial.write(SIM800l.read());//Forward what Software Serial received to Serial Port
  }
}


void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(" ");
    Serial.print(F("Speed:"));
    Serial.print(gps.speed.kmph());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}