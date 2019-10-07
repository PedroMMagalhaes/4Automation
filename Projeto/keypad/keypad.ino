// Libs
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Key.h>
#include <Keypad.h>
#define MAX_STRINGPASS 5 //visto que sizeof de um char apenas ==> 3 então 
#define PASSWORD "1234A"

//****** MQTT *****

#define wifi_ssid "4Automation"
#define wifi_password "projetoInformatico2019"
 
#define mqtt_server "192.168.1.40"
#define mqtt_user "testemqtt"      // if exist user
#define mqtt_password "teste123"  // password
 
#define keypad_topic "sensor/keypad"  //Topic temperature
//#define humidity_topic "sensor/humidity"        //Topic humidity

WiFiClient espClient;
PubSubClient client(espClient);

//Buffer to decode MQTT messages
char message_buff[100];
long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  //Display log message if  = True

//*****************

const byte n_rows = 4;
const byte n_cols = 4;

char keys[n_rows][n_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
byte colPins[n_rows] = {D3, D2, D1, D0};
byte rowPins[n_cols] = {D7, D6, D5, D4};
int tentativas=0;
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols); 
String teste ="";
int contador= 0;

void setup(){
  Serial.begin(115200);
  setup_wifi();           //Connect to Wifi network
  client.setServer(mqtt_server, 1883);    // Configure MQTT connection
  client.setCallback(callback); 
}
 
void loop(){
  


if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  keypadFunction();  

  }

void keypadFunction(){

  char myKey = myKeypad.getKey();
 
  if (myKey!=0){
    Serial.print("Key pressed: ");
    teste+=myKey;
    Serial.println(myKey);
    Serial.println(teste);
    contador++;
  }

  
  if(contador == MAX_STRINGPASS)
  {
    tentativas++;
    
    if(tentativas == 3){
      Serial.println("MAX TRIES EXCEEDED - BURRO");
      //AQUI LANÇA ALERTA 
      String erro = "Maximo tentativas";
      client.publish(keypad_topic, String(erro).c_str(), true);   // Publish temperature on temperature_topic
      tentativas=0;
     
      
    }
    
    if(teste == PASSWORD)
    {
      Serial.println("CORRECT PASSWORDDDDDDD !!!!");
      String info = "Password correta";
      client.publish(keypad_topic, String(info).c_str(), true);   // Publish temperature on temperature_topic
      teste= "";
      tentativas=0;
      
    }
    
    contador=0;
    teste="";
  }
  
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
    Serial.print(" | test: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
  
}
