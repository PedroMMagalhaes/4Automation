/**
 * Author's:  Pedro Magalhães & Daniel Araújo
 * Created:   03-07-2019
 * Last Edit: 07-07-2019
 * Purpose:   Show on LCD and LEDS when the entrance is open or closed and if access is granted or denied.
 * Language:  C
 *
 * (c) Copyright by 4Automation & Polytechnic of Leiria.
 **/


//LCD
// Pins
// SCL -> D1
// SDA -> D2
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define greenPin D3
#define redPin D4

#define wifi_ssid "4Automation"
#define wifi_password "DanielPedro2019"

#define mqtt_server "35.246.57.184"
#define mqtt_user "esp10"    // if exist user
#define mqtt_password "teste123" // password


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  Serial.begin(115200);
    
  setup_wifi();                        //Connect to Wifi network
  client.setServer(mqtt_server, 1883); // Configure MQTT connection
  client.setCallback(callback);
  client.subscribe("sensor/keypad");


  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

}

void reconnect()
{

  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("LCDLCD", mqtt_user, mqtt_password))
    {
      Serial.println("OK");
      Serial.println("Subscribing");
      client.subscribe("sensor/keypad");
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
void callback(char *topic, byte * payload, unsigned int length)
{

    Serial.println("Message rec =>  topic: " + String(topic));
    Serial.print(" | test: " + String(length, DEC));
    std::string my_string(topic);
    String message;
    for (int i = 0; i < length; i++) {
    message += (char)payload[i];
    }
    Serial.println(message);
  // create character buffer with ending null terminator (string)

  //LCD & LEDS
  if(my_string == "sensor/keypad"){
    if(message == "UNLOCK"){
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("ACESSO");
    lcd.setCursor(4,1);
    lcd.print("CONCEDIDO");
    }
    else
    {
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("ACESSO");
    lcd.setCursor(5,1);
    lcd.print("NEGADO");
    }
  }
}
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
