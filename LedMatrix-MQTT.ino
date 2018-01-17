#include <LEDMatrixDriver.hpp>
#include "fontbasic.h"
#include "functions.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "<YOUR-SSID-HERE>";
const char* password =  "<YOUR-WIFI-KEY-HERE>";
const char* mqttServer = "<YOUR-MQTT-SERVER>";
const int   mqttPort = 1883;
const char* mqttUser = "<MQTT-USER>";
const char* mqttPassword = "<MQTT-PASS>";
const int   MatrixSize = 4;

/*

Connect the following PINS to the MAX LEDMatrix Module
[This is based on the 4 x 8x8 module]


MAX		NodeMCU
VCC		3V3 
GND		GND
DIN		D7
CS		D8
CLK		D5

*/

int MQTTstate = -1;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  //while (1){};
  Serial.begin(115200);

  //WIFI
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  
  //MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }  

  client.publish("ledmatrix/alive", "I'm here");
  client.subscribe("ledmatrix/text");
  
  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(1);   // 0 = low, 10 = high
  lmd.clear();
  lmd.display();
}

void loop() {
  
  // while ( client.loop() == true ) yield();
  client.loop();
  MQTTstate = client.state();
  if ( MQTTstate == -1 )
  {
    Serial.print("Got disconnected :-(");

    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
 
      if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
      } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
      }
    }  

  }
  if ( MQTTstate == 0 )
  {
    //Serial.println("Still connected :-)");
  }
  ESP.wdtFeed();
  //client.publish("ledmatrix/alive", "I'm here");
  delay(500);
  
}


void callback(char* topic, byte* payload, unsigned int length) {
  String Message = "";
  String Command = "";
  String DisplayText = "";
  char cnvMessage[128] = "";
  String DisplayMessage = "";
  int Intensity = 1;
  
  Serial.print("Topic   : ");
  Serial.println(topic);
  //Serial.print("Message : ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    Message = Message + (char)payload[i];
  }
  int Message_len = Message.length() - 3;

  if ( Message_len < 1 ) {
    Serial.print("ERR:Length is too small:");
    Serial.println(Message_len + 4);
  }

  Command     = Message.substring(0,3);
  DisplayText = Message.substring(4);

  //Serial.println( DisplayText.indexOf("&#") );

    DisplayMessage = ConvertHTMLHex ( DisplayText );  
    String Text= "";
    String HexCodeStr = "";
    
/*      while ( Text.indexOf("&#") > -1 and Text.charAt( Text.indexOf("&#") + 4 ) == ';' ) {
      // Special characters
      //Serial.print("It contains a specials! : ");
      HexCodeStr = Text.substring( Text.indexOf("&#") + 2 ,Text.indexOf("&#") +4 );
      //Serial.println(HexCodeStr);
      Text.replace( "&#" + HexCodeStr +";", String(char(strtol( &HexCodeStr[0], NULL, 16))));
      //DisplayText.replace( "&#80;" , String(char(0x80)) );
      }
*/       
  Serial.println(DisplayMessage);
  Serial.print("Command = ");
  Serial.println(Command);
  Serial.print("Text    = ");
  Serial.println(DisplayText);

  
  if ( Command == "SCR" ) 
  {
    // 'SCRL':
    //

    DisplayMessage.toCharArray(cnvMessage, Message_len);
    //Serial.print(Message);
    //Serial.println();
    
    marquee_run(cnvMessage,20);
    //Serial.println("Done displaying");
  } 
  else if ( Command == "INT" )
  {
    Serial.println("Set Intensity ");

    Intensity = DisplayText.toInt();
    if ( Intensity < 1 || Intensity > 10 )
    {
      Serial.println("Illegal Intensity");
    }
    else
    {
      lmd.setIntensity(Intensity);
    }
  }
  else if ( Command == "CLR" )
  {
    Serial.println("Clear Display");
    lmd.clear();
    lmd.display();
  } 

  else if ( Command == "DIS" )
  {
    Serial.println("Display");
    lmd.clear();
    DisplayMessage = DisplayMessage.substring(0,MatrixSize+1);
    DisplayMessage.toCharArray(cnvMessage, MatrixSize + 1);
    drawString(cnvMessage,MatrixSize + 1,0,0);
    lmd.display();
    
  } 

  
  else if ( Command == "NON" )
  {
    Serial.println("Nothing to do ... ");
  } 
  
  else {
    // default:
    //
    Serial.println("Not implemented");
  
  }
  Serial.println("-- ");
  return;
}

