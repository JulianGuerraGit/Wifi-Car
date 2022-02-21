/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <BlynkSimpleEsp8266.h>
 
#define SteerSpeed 4
#define SpeedDir   0  //B
#define Speed  5  //A
#define SteerDir    2

#ifndef STASSID
#define STASSID "*****"
#define STAPSK  "*****"
#endif

char auth[] = "*****";

const char* host = "Car";
const char* ssid = STASSID;
const char* password = STAPSK;

int minRange = 312;
int maxRange = 712;

int minSpeed = 800;
int maxSpeed = 1024;
int noSpeed = 0;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void moveControl(int x, int y)
{
  // movement logic
  // move forward

   // y is greater than maxrange and x is greater than minRange and less than max range at the same time 
  if(y >= maxRange && x >= minRange && x <= maxRange) //zataci R
  {
    digitalWrite(SpeedDir,LOW);  
    digitalWrite(SteerDir,HIGH);
    analogWrite(SteerSpeed,noSpeed);
    analogWrite(Speed,maxSpeed);
  }
  
  // move forward right
  else if(x >= maxRange && y >= maxRange)   //zataci R
  {
    digitalWrite(SpeedDir,LOW);
    digitalWrite(SteerDir,LOW);
    analogWrite(SteerSpeed,minSpeed); 
    analogWrite(Speed,maxSpeed);
  }
 
  // move forward left
  else if(x <= minRange && y >= maxRange)
  {
    digitalWrite(SpeedDir,LOW);
    digitalWrite(SteerDir,HIGH);
    analogWrite(SteerSpeed,minSpeed); 
    analogWrite(Speed,maxSpeed);
  }
 
  // neutral zone
  else if(y < maxRange && y > minRange && x < maxRange && x > minRange)
  {
    analogWrite(SteerSpeed,noSpeed); 
    analogWrite(Speed,noSpeed);
  }
 
 // move back
  else if(y <= minRange && x >= minRange && x <= maxRange)
  {
    digitalWrite(SpeedDir,HIGH);
    digitalWrite(SteerDir,LOW);
    analogWrite(SteerSpeed,noSpeed); 
    analogWrite(Speed,maxSpeed);
  }
 
  // move back and left
 else if(y <= minRange && x <= minRange)
  {
    digitalWrite(SpeedDir,HIGH);
    digitalWrite(SteerDir,HIGH);
    analogWrite(SteerSpeed,minSpeed); 
    analogWrite(Speed,maxSpeed);  
  }
 
  // move back and Right
  else if(y <= minRange && x >= maxRange)
  {
    digitalWrite(SpeedDir,HIGH);
    digitalWrite(SteerDir,LOW);
    analogWrite(SteerSpeed,minSpeed); 
    analogWrite(Speed,maxSpeed);
  }

  else if(y > minRange && y < maxRange && x >= maxRange)
  {
    digitalWrite(SpeedDir,HIGH);
    digitalWrite(SteerDir,LOW);
    analogWrite(SteerSpeed,maxSpeed); 
    analogWrite(Speed,noSpeed);
  }

  else if(y > minRange && y < maxRange && x <= maxRange)
  {
    digitalWrite(SpeedDir,HIGH);
    digitalWrite(SteerDir,HIGH);
    analogWrite(SteerSpeed,maxSpeed); 
    analogWrite(Speed,noSpeed);
  }
}
void setup(void) {

  pinMode(SteerSpeed, OUTPUT);
  pinMode(Speed, OUTPUT);
  pinMode(SpeedDir, OUTPUT);
  pinMode(SteerDir, OUTPUT);
  digitalWrite(SteerSpeed, LOW);
  digitalWrite(Speed, LOW);
  digitalWrite(SpeedDir, HIGH);
  digitalWrite(SteerDir,HIGH);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  Blynk.begin(auth, ssid, password);

}

void loop(void) {
  httpServer.handleClient();
  MDNS.update();
  Blynk.run();
}

BLYNK_WRITE(V1)
{
  int x = param[0].asInt();
  int y = param[1].asInt();
  moveControl(x,y); 
}
