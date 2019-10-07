// Libs
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//****** MQTT *****

#define wifi_ssid "teste"
#define wifi_password "teste123"

#define mqtt_server "192.168.1.40"
#define mqtt_user "testemqtt"      // if exist user
#define mqtt_password "teste123"  // password

#define sound_topic "sensor/sound"  //Topic temperature


WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;
long lastRecu = 0;
bool debug = true;  //Display log message if  = True
int sensorPin= 4;
boolean val = 0;

void setup() {
  Serial.begin(9600);
  setup_wifi();           //Connect to Wifi network
  client.setServer(mqtt_server, 1883);    // Configure MQTT connection
  client.setCallback(callback);
  pinMode(sensorPin, INPUT);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
 
  soundsensor();
  delay(220);
}
void soundsensor() {

  val = digitalRead(sensorPin);
  Serial.println (val);


}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");

}

//Reconnect
void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before retry");

      delay(5000);
    }
  }
}


// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) {
    Serial.println("Message rec =>  topic: " + String(topic));
    Serial.print(" | test: " + String(length, DEC));
  }
  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }

}
