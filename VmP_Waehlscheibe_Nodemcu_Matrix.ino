

#include <WiFiServer.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiClient.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiAP.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <Bounce2.h>
#include <Dhcp.h>
#include <EthernetClient.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetServer.h>

#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = LED_BUILTIN; // This code uses the built-in led for visual feedback that a message has been received
static const byte NSApin = D2;  // NSA, LOW wenn sich die Waehlscheibe dreht
static const byte NSIpin = D1; // NSI, Erzeugt bei der Zuckbewegung der Waehlscheibe die Pulse

/*
Project: Wifi controlled LED matrix display
NodeMCU pins    -> EasyMatrix pins
MOSI-D7-GPIO13  -> DIN
CLK-D5-GPIO14   -> Clk
GPIO0-D3        -> LOAD (CS)

*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#define SSID "Freifunk Erfurt"                      // insert your SSID 
#define PASS ""                    // insert your password 

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "MQTT SERVER";     // Hier bitte den MQTT Server eintragen
const char* mqtt_topic = "/vmp/ws";           // Hier bitte das Topic eintragen
const char* mqtt_username = "MQTTUSER";      // Hier bitte den MQTT Benutzername eintragen
const char* mqtt_password = "MQTTPW";        // Hier bitte das MQTT Passwort eintragen
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "VMPWS";

//long period;
//int offset=1,refresh=0;
int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;          // Hier die Anzahl der zusammengeschaltetet Displays (MATRIX 8x8) eintragen
int numberOfVerticalDisplays = 1;          
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);



Bounce waehlpulse = Bounce();
Bounce scheibedreht = Bounce();

// Initialise the WiFi and MQTT Client objects
          WiFiClient wifiClient;
          PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
      client.subscribe(mqtt_topic);
      return true;
    }
    else {
      return false;
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("flash", "hello world");
      // ... and resubscribe
      client.subscribe("Flash_Message");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
  {    matrix.setIntensity(10); // Use a value between 0 and 15 for brightness

          // Adjust to your own needs
          //  matrix.setPosition(0, 1, 0); // The first display is at <0, 0>
           //  matrix.setPosition(0, 0, 0); // The second display is at <1, 0>
          // Adjust to your own needs                
            matrix.setPosition(0, 0, 0); // The first display is at <0, 0>           // Hier die Position  DISPLAY 1 (MATRIX 8x8) eintragen
            //matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
            //matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
            //matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
            matrix.setRotation(0, 3);  // Display 1 mit 90°                         // Hier die Ausrichtung DISPLAY 1 (MATRIX 8x8) eintragen
            //matrix.setRotation(1, 1); // Display 2 mit 90°
            //matrix.setRotation(2, 1); // Display 3 mit 90°
            //matrix.setRotation(3, 1); // Display 4 mit 90°

   
   //ESP.wdtDisable();                               // used to debug, disable wachdog timer, 
       Serial.begin(115200);                           // full speed to monitor
                               
        WiFi.begin(SSID, PASS);                         // Connect to WiFi network
        while (WiFi.status() != WL_CONNECTED) {         // Wait for connection // Schleife deaktivieren wenn kein WiFi
        delay(500);                                     // sonst startet die Funktion nicht, solange kein WiFi Connect erfolgt ist !
        Serial.print(".");
       }

          Serial.print("SSID : ");                        // prints SSID in monitor
          Serial.println(SSID);                           // to monitor             
 
           char result[16];
           sprintf(result, "%3d.%3d.%1d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
           Serial.println();
           Serial.println(result);


           Serial.println(WiFi.localIP());                 // Serial monitor prints localIP
           //Serial.print(analogRead(A0));
  
 
           // Connect to MQTT Broker
           // setCallback sets the function to be called when a message is received.
              client.setServer(mqtt_server,9765);
              if (Connect()) {
              Serial.println("Connected Successfully to MQTT Broker!");  
              if (client.subscribe(mqtt_topic)){
              Serial.println("Subscribe"+ String(mqtt_topic));
              }
             }
              else {
              Serial.println("Connection Failed!");
             }
   



  pinMode(NSApin, INPUT_PULLUP);  // Set the button as an input
  pinMode(NSIpin, INPUT_PULLUP);  // Set the button as an input
  //pinMode(ledPin,OUTPUT);

  waehlpulse.attach(NSIpin);
  waehlpulse.interval(5); // interval in ms
  scheibedreht.attach(NSApin);
  scheibedreht.interval(5); // interval in ms

  //Serial.begin(115200);
	
}

unsigned int count=0;

char inttochar(int a)         // int to Char für DISPLAY Anzeige
{
  char b[2];
  String str;
  str = String(a);
  str.toCharArray(b,2);
  return b[0];
  
}


void loop() {
   // If the connection is lost, try to connect again
  if (!client.connected()) {
    Connect();
  }
  // client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
  client.loop();
  
  waehlpulse.update();
  scheibedreht.update();

                                        
  if(scheibedreht.read() == LOW)    // Wenn sich die Waehlscheibe dreht...
  {
                                       
    if(waehlpulse.fallingEdge()) {  //... und wenn ein Impuls anliegt
      count++;                      //... Impuls zaehlen
    }  
  }
   else // Wenn sich die Waehlscheibe nicht dreht...
   {                          
      if(count != 0) //... und wenn wir vorher etwas gezaehlt haben
      {                
        matrix.fillScreen(LOW);
        if(count == 10) //... und wenn der Counter auf 10 steht
        {              
        
          Serial.println(0);         //... schreibe eine 0 
        
          matrix.drawChar(2, 1, inttochar(0), HIGH, LOW, 1);
          client.connect(clientID, mqtt_username, mqtt_password);
          delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
          client.publish(mqtt_topic,"0");
        
        } 
        else   //... andernfalls, wenn der Counter nicht 0 aber auch nicht 10 ist
        {                     
          Serial.println(0+count);   //... schreibe den Counterwert 
          matrix.drawChar(2, 1, inttochar(0+count), HIGH, LOW, 1);  //drawChar(0, 0, 0+count, HIGH, LOW, 1)
          client.connect(clientID, mqtt_username, mqtt_password);
          delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
          char b[2] = {'0'+count, '\0'};         // int to Char für MQTT
          client.publish(mqtt_topic, b);   
        }
        matrix.write(); // Send bitmap to display
                    
        count = 0;                    // Counter zuruecksetzen

      
      }
    }
    // If the connection is lost, try to connect again
  if (!client.connected()) {
    Connect();
  }
  // client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
  client.loop();
  // Once it has done all it needs to do for this cycle, go back to checking if we are still connected.
    
}

