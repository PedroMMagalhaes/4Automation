#include <IRremote.h>
#include <avr/io.h>
// ISR interrupt service routine
#include <avr/interrupt.h>

const int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        irrecv.resume();
  }
}
