/**
 * Author's:  Pedro Magalhães & Daniel Araújo
 * Created:   17-04-2019
 * Last Edit: 04-05-2019
 * Purpose:   Manipulate a RFID and send data to a MQTT server . 
 * Language:  C
 * 
 * (c) Copyright by 4Automation & Polytechnic of Leiria.
 **/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;     // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

//****** MQTT *****
#define wifi_ssid "4Automation"
#define wifi_password "DanielPedro2019"

#define mqtt_server "35.246.57.184"
#define mqtt_user "seguranca2"    // if exist user
#define mqtt_password "teste123" // password

#define keypad_topic "sensor/keypad" //Topic security
#define cardHex "21 F9 28 1E"
#define keyChainHex "DB 48 93 1B"
WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;
long lastRecu = 0;
bool debug = true; //Display log message if  = True

//*****************
String mensagem = "UNLOCK";
String msgString = "";
String variavelGlobal = "";
double teste;

void setup() {
  Serial.begin(115200);
  setup_wifi();                        //Connect to Wifi network
  client.setServer(mqtt_server, 1883); // Configure MQTT connection
  client.setCallback(callback);
  client.subscribe(keypad_topic);

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522

  client.publish(keypad_topic, String("LOCK").c_str(), true); // Publish keypad correct topic

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  
} 

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  rfid();
}

void rfid()
{

  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  client.subscribe(keypad_topic);

  Serial.print("Tag :");
  String tag = "";
  byte caractere;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  tag.toUpperCase();


  if (tag.substring(1) == "21 F9 28 1E") // tag verification
  {

    Serial.println("TAG checked - Access allow !");
    Serial.println();

    if (msgString == "LOCK")
    {
      Serial.println("Mensagem: ");
      Serial.println(mensagem);
      Serial.println();
      mensagem = "UNLOCK";
      client.publish(keypad_topic, String(mensagem).c_str(), true); // Publish correct topic 
    }
    else if (mensagem = "UNLOCK")
    {
      mensagem = "LOCK";
      Serial.println("Mensagem: ");
      Serial.println(mensagem);
      Serial.println();

      client.publish(keypad_topic, String(mensagem).c_str(), true); // Publish correct topic 
    }
    delay(300);
  }

  else if (tag.substring(1) == "DB 48 93 1B")
  {

    Serial.println("TAG checked - Access allow !");
    Serial.println();
    if (msgString == "LOCK")
    {
      Serial.println("Mensagem: ");
      Serial.println(mensagem);
      Serial.println();
      mensagem = "UNLOCK";
      client.publish(keypad_topic, String(mensagem).c_str(), true); 
    }
    else if (mensagem = "UNLOCK")
    {
      mensagem = "LOCK";
      Serial.println("Mensagem: ");
      Serial.println(mensagem);
      Serial.println();

      client.publish(keypad_topic, String(mensagem).c_str(), true); 
    }
    delay(3000);
  }
  else
  {
    Serial.println("Unknown tag - Access refused !!!");
    Serial.println();
  }
  delay(1000);
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
    if (client.connect("ESPRFID", mqtt_user, mqtt_password))
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
