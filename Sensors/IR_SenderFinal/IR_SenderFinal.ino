/**
 * Author's:  Pedro Magalhães & Daniel Araújo
 * Created:   06-06-2019
 * Last Edit: 12-06-2019
 * Purpose:   Manipulate a IR Emitter and send data to a MQTT server . 
 * Language:  C
 * 
 * (c) Copyright by 4Automation & Polytechnic of Leiria.
 **/

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//****** MQTT *****
#define wifi_ssid "4Automation"
#define wifi_password "DanielPedro2019"

#define mqtt_server "35.246.57.184"
#define mqtt_user "robot"    // if exist user
#define mqtt_password "teste123" // password

#define robot_topic "sensor/robot5" //Topic robot
WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;
long lastRecu = 0;
bool debug = true; //Display log message if  = True
String mensagem = "NADA";
String msgString = "";

//*****************



const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData1[89] = {4542, 4466, 598, 534, 598, 1652, 600, 534,
                         598, 1654, 598, 532, 598, 1652, 600, 532,
                         598, 536, 4540, 4468, 598, 532, 600, 1654,
                         598, 534, 598, 1654, 598, 534, 598, 1652,
                         600, 532, 598, 538, 4540, 4466, 598, 534,
                         598, 1654, 598, 534, 598, 1652, 600, 534,
                         600, 1652, 598, 534, 598, 536, 4540, 4466,
                         598, 534, 598, 1652, 600, 534, 598, 1656,
                         596, 534, 600, 1654, 598, 534, 598, 536,
                         4540, 4466, 598, 534, 600, 1654, 598, 534,
                         598, 1654, 598, 534, 598, 1654, 598, 532,
                         598
                        };

uint16_t rawData2[89] = {4540, 4464,  598, 534,  598, 532,  598, 534,  598, 1652,  600,
                         ~534,  598, 532,  598, 1652,  600, 536,  4540, 4464,  600, 532,  598, 534,  598, 532,  600,
                         1652,  598, 532,  600, 534,  598, 1650,  600, 536,  4538, 4464,  598, 534,  598, 532,  600, 532,
                         598, 1652,  600, 532,  600, 532,  600, 1652,  600, 536,  4540, 4464,  598, 532,  598, 534,  598, 532,
                         598, 1652,  600, 532,  600, 532,  598, 1652,  598, 536,  4538, 4464,  598, 532,  600, 532,  598, 534,
                         598, 1652,  598, 534,  598, 532,  600, 1652,  598
                        };  // UNKNOWN D2A29631

// Example Samsung A/C state captured from IRrecvDumpV2.ino
uint8_t samsungState[kSamsungAcStateLength] = {
  0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
  0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0
};

char incomingByte;

void setup() {
  irsend.begin();
  // Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.begin(115200);

  setup_wifi();                        //Connect to Wifi network
  client.setServer(mqtt_server, 1883); // Configure MQTT connection
  client.setCallback(callback);
  client.subscribe(robot_topic);
}

void loop() {

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
 
  client.subscribe(robot_topic);
  ir();
}

void ir(){
  if(msgString=="a"){
    autoRobot();
    mensagem="a limpar";
    client.publish(robot_topic, String(mensagem).c_str(), false);
    msgString="";
  }
  if (msgString=="b"){
    msgString="";
    mensagem="a ir para a dock";
    client.publish(robot_topic, String(mensagem).c_str(), false);
    goHomeRobot();
  }
}
// MQTT callback function
void callback(char *topic, byte * payload, unsigned int length)
{
 int i = 0;
  if (debug)
  {
    Serial.println("Message rec =>  topic: " + String(topic));
    Serial.print(" | test: " + String(length, DEC));
  }
  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  msgString = String(message_buff);
  if (debug)
  {
    Serial.println("Payload: " + msgString);
  }
}

void goHomeRobot() {
  unsigned long BT_Input_Code = 0xD2A29631;    //For example
  irsend.send(NEC, BT_Input_Code, 45);
  //delay(2000);
  Serial.println("a rawData capture from IRrecvDumpV2");
  Serial.println("HOME ****");
  irsend.sendRaw(rawData2, 67, 38);  // Send a raw data capture at 38kHz.
}

void autoRobot() {
  //unsigned long BT_Input_Code = 0x52A3E788;    //For example
  //irsend.send(NEC, BT_Input_Code, 45);
  //irsend.sendNEC(0x);
  //delay(2000);
  //Serial.println("Sony");
  //irsend.sendSony(BT_Input_Code, 12, 1);  // 12 bits & 2 repeats
  //delay(2000);
  Serial.println("a rawData capture from IRrecvDumpV2");
  Serial.println("AUTO CLEAN  ****");
  irsend.sendRaw(rawData1, 67, 38);  // Send a raw data capture at 38kHz.
}

//Reconnect
void reconnect()
{

  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESPROBOT", mqtt_user, mqtt_password))
    {
      Serial.println("OK");
    }
    else
    {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before retry");

      delay(5000);
    }
  }
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
}
