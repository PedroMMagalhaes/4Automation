/**
   Author's:  Pedro Magalhães & Daniel Araújo
   Created:   16-04-2019
   Last Edit: 06-05-2019
   Purpose:   Manipulate a Keypad and send data to a MQTT server .
   Language:  C

   (c) Copyright by 4Automation & Polytechnic of Leiria.
 **/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//****** MQTT *****
#define wifi_ssid "4automation"
#define wifi_password "projetoInformatico2019"

#define mqtt_server "35.246.57.184"
#define mqtt_user "esp6"    // if exist user
#define mqtt_password "teste123" // password
#define rele_topic1 "sensor/switch1" //Topic rele ch1
#define rele_topic2 "sensor/switch2" //Topic rele ch1

#define pinCh1 4 //D2
#define pinCh2 15 //D8


#define movement_topic "sensor/movement"

int pirPin = D3;
int val;
String mov;

WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;
long lastRecu = 0;
bool debug = true; //Display log message if  = True

//*****************

String msgString = "";

void setup()
{
  Serial.begin(9600);
  setup_wifi();                        //Connect to Wifi network
  client.setServer(mqtt_server, 1883); // Configure MQTT connection
  client.setCallback(callback);

}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  // Send a message every 4/minute, change 20 to 60 for 1 minute
  if (now - lastMsg > 1000 * 5) {
    lastMsg = now;

  }
  if (now - lastRecu > 100)
  {
    lastRecu = now;
    client.subscribe(rele_topic1); //sub topic rele1
    client.subscribe(rele_topic2); //sub topic rele2
  }



  //printMov();
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
  //initialize the switch as an output and set to LOW (off)
  pinMode(pinCh1, OUTPUT); // Relay Switch 1
  digitalWrite(pinCh1, LOW);
  pinMode(pinCh2, OUTPUT); // Relay Switch 1
  digitalWrite(pinCh2, LOW);
}

//Reconnect
void reconnect()
{

  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password))
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

// MQTT callback function
void callback(char *topic, byte *payload, unsigned int length)
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

  if (msgString == "ON1")
  {
    digitalWrite(pinCh1, HIGH);
    Serial.println("ON Rele1");
  }
  else if (msgString == "OFF1")
  {
    digitalWrite(pinCh1, LOW);
    Serial.println("OFF Rele1");

  } else if (msgString == "OFF2") {
    digitalWrite(pinCh2, LOW);
    Serial.println("OFF Rele2");
  } else if (msgString == "ON2") {
    digitalWrite(pinCh2, HIGH);
    Serial.println("OFF Rele2");
  }
}

String movement(void)
{

  val = digitalRead(pirPin);
  String movement;
  //low = no motion, high = motion

  if (val == LOW)
  {
    movement = "No motion";
  }
  else if (val == HIGH)
  {
    movement = "Motion detected";
  }

  return movement;
}

void printMov()
{
  mov = movement();

  Serial.println(mov);

  client.publish(movement_topic, String(mov).c_str(), true);
}
