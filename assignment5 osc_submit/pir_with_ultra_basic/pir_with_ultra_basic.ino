#include <NewPing.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#define COMMON_ANODE

#define MAX_DISTANCE 400   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int echo_pin = 0;              // Echo Pin of the ultrasonic sensor
int trigger_pin = 5;           // Trigger pin ofthe ultrasonic sensor
int led_pin = 1;           // Sound alarm buzzer
int pir_pin = 6;              // PIR sensor Pin
float distance, duration;          // Default distance measured by ultrasonic sensor
float previousDistance, currentDistance, outputValue;

float signalStart, signalDuration;
int pirState = LOW; 
long unsigned int lowIn;
// the amount of milliseconds the sensor has to be low
// before we assume all motion has stopped
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int calibrationTime = 30;

// Instance creation for the New ping Library
NewPing sonar(trigger_pin, echo_pin, MAX_DISTANCE);

// Wifi setup
char ssid[] = "catchingafish";     //  your network SSID (name)
char pass[] = "tee4853yet7727ding";  // your network password

//char ssid = "NYU";
//char username = 
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiUDP Udp;
IPAddress outIp(192,168,1,153); //destination of outgoing OSC messages
IPAddress ip(192,168,1,184);    //local ip

const unsigned int outPort = 9001;  //arduino out port; match the input port of OF
const unsigned int inPort = 12000;  //arduino receiver port; match the output port of OF

//led
int redPin = 2;
int greenPin = 3;
int bluePin = 4;

void setup() {
  //Serial.begin(9600);
  
  //pinMode(led_pin, OUTPUT);      // declare LED as output
  
  // declare sensor as input
  pinMode(pir_pin, INPUT);       
  // declare LED as output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

  // wait 10 seconds for connection:
  delay(10000);
  }
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
  
  Udp.begin(inPort);   //initialising UDP, passing in to the inPort
  // activation of the pull up resistor for the PIR pin
  digitalWrite(pir_pin, HIGH);
  // give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}
 
void loop(){
  readPIR();
  //pingDist();  
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

//motion dection with PIR
void readPIR() {
  if (digitalRead(pir_pin) == HIGH) {
//    pingDist();        //Call the function to measure distance from th ultrasonic sensor if motion is detected and send distance data to oF
//    setColor(0, 255, 0);
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      Serial.println("---");
      Serial.print("motion detected at ");
      signalStart = millis()/1000;
      Serial.print(signalStart);
      Serial.println("sec");
      delay(50);
    }
    //Call the function to measure distance from th ultrasonic sensor if motion is detected and send distance data to oF
    pingDist();      
    setColor(0, 255, 0);
    takeLowTime = true;
  }

  if (digitalRead(pir_pin) == LOW) {
    setColor(255, 0, 0);
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //keep sending data during pir idle time
    if (!lockLow && millis() - lowIn < pause) {
      setColor(0, 0, 255);
      pingDist();  
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    } else if (!lockLow && millis() - lowIn > pause) {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      Serial.print("signalDuration ");
      Serial.println((millis() - pause) / 1000 - signalStart);
      signalDuration = (millis() - pause) / 1000 - signalStart;
      //setColor(0, 0, 255);
      delay(50);
//      OSCMessage msgOUT("/duration/0");
//      msgOUT.add((float)signalDuration);
//      // send the OSC packet via UDP:
//      Udp.beginPacket(outIp, outPort);
//      msgOUT.send(Udp);
//      Udp.endPacket();
//      msgOUT.empty();
//      delay(20);
       float idleDistance = 210;
       OSCBundle bndl;
       bndl.add("/duration/0").add((float)signalDuration);
       bndl.add("/distance/0").add((float)idleDistance);
       Udp.beginPacket(outIp, outPort);
       bndl.send(Udp); // send the bytes to the SLIP stream
       Udp.endPacket(); // mark the end of the OSC Packet
       bndl.empty(); // empty the bundle to free room for a new one
       delay(20);
    }
  }
}

// Ultrasonic sensor distance reading
void pingDist () {
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  //distance = sonar.convert_cm(sonar.ping_median(10));

  duration = sonar.ping();
  
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  
  distance = (duration / 2) * 0.0343;

  previousDistance = outputValue;
  currentDistance = distance;

  if (currentDistance > 0){
    outputValue = currentDistance;
  } else if (currentDistance == 0){
    outputValue = previousDistance;
  }
  
  Serial.print("Output");
  Serial.println(outputValue); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("Distance");
  Serial.print(currentDistance);
  delay(20);

   // create an OSCMessage:
  OSCMessage msgOUT("/distance/0");
  msgOUT.add((float)outputValue);
//  } else if (distance == 0){
//    msgOUT.add((float)1);
//  }
  // send the OSC packet via UDP:
  Udp.beginPacket(outIp, outPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
  delay(20);
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
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

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}


