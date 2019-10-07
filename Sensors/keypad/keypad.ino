/**
 * Author's:  Pedro Magalhães & Daniel Araújo
 * Created:   16-04-2019
 * Last Edit: 06-06-2019
 * Purpose:   Manipulate a Keypad and send data to a MQTT server . 
 * Language:  C
 * 
 * (c) Copyright by 4Automation & Polytechnic of Leiria.
 **/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Key.h>
#include <Keypad.h>
#define MAX_STRINGPASS 5 //sizeof char ==> 3 so...
#define PASSWORD "1234A"

//****** MQTT *****
#define wifi_ssid "4Automation"
#define wifi_password "DanielPedro2019"

#define mqtt_server "172.16.1.2"
#define mqtt_user "seguranca3"    // if exist user
#define mqtt_password "teste123" // password

#define keypad_topic "sensor/keypad" //Topic security

WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;
long lastRecu = 0;
bool debug = true; //Display log message if  = True

//*****************

const byte n_rows = 4;
const byte n_cols = 4;

char keys[n_rows][n_cols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte colPins[n_rows] = {D3, D2, D1, D0};
byte rowPins[n_cols] = {D7, D6, D5, D4};
int tentativas = 0;
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
String teste = "";
int contador = 0;
String estado = "";
String mensagem = "UNLOCK";
String msgString = "";

void setup()
{
  Serial.begin(9600);
  setup_wifi();                        //Connect to Wifi network
  client.setServer(mqtt_server, 1883); // Configure MQTT connection
  client.setCallback(callback);
  client.subscribe(keypad_topic);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  
  keypadFunction();
}
void keypadFunction()
{

  char myKey = myKeypad.getKey();

  if (myKey != 0)
  {
    Serial.print("Key pressed: ");
    teste += myKey;
    Serial.println(myKey);
    Serial.println(teste);
    contador++;
  }

  if (contador == MAX_STRINGPASS)
  {
    tentativas++;

    if (tentativas == 3)
    {
      Serial.println("MAX TRIES EXCEEDED - ...");
      //SEND ALERT
      mensagem = "LOCK";
      client.publish(keypad_topic, String(mensagem).c_str(), true); // Publish keypad error
      tentativas = 0;
    }

    if (teste == PASSWORD)
    {
      client.subscribe(keypad_topic); //
      Serial.println("CORRECT PASSWORDDDDDDD !!!!");
      Serial.println("TESTE: " + msgString);
      if (msgString == "LOCK")
      {
        mensagem = "UNLOCK";
        client.publish(keypad_topic, String(mensagem).c_str(), true); // Publish keypad correct password
      }
      else
      {
        mensagem = "LOCK";
        client.publish(keypad_topic, String(mensagem).c_str(), true); // Publish keypad correct password
      }

      teste = "";
      tentativas = 0;
    }
    contador = 0;
    teste = "";
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

//Reconnect
void reconnect()
{

  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client3", mqtt_user, mqtt_password))
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
}
