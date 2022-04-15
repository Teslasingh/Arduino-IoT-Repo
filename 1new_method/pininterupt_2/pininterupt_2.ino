const byte  interruptPin = D5; // Or other pins that support an interrupt
volatile bool sat=0;
volatile bool sata=0;
unsigned long RiseTime ;
unsigned long FallTime ;
long delta;
#include <EEPROM.h>
String esid;
String eauths;
#define           LED_pin D1
#define         redled D7
//portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;
void setup() {
  Serial.begin(115200);
   EEPROM.begin(512);
  pinMode(LED_pin, OUTPUT);   // LED pin to Output mode
  pinMode(redled, OUTPUT);
  digitalWrite(LED_pin, LOW); // Turn LED off (High = On, Low = Off)
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, CHANGE); //Respond to falling edges on the pin
 // attachInterrupt(digitalPinToInterrupt(interruptPin), IS, CHANGE);
 delay(10000);
   Serial.println("Reading EEPROM ssid");

  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 64; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  String eauth = "";
  for (int i = 64; i < 128; ++i)
  {
    eauth += char(EEPROM.read(i));
  }
  Serial.print("AUTH: ");
  Serial.println(eauth);
  eauths = eauth.c_str();
  Serial.println(eauths);
}
 
void loop() {
Serial.print("loop working");
  delay(10000);
  digitalWrite(redled,LOW);
}
 
ICACHE_RAM_ATTR void ISR () {                  // Interrupt Service Routine, come here when an interrupt occurs
  // unsigned long currentTime=millis();
  Serial.println("isr loop..............................");
 
  if(sat==0){
  doRising();
 RiseTime=millis();
 Serial.print("riseTime = ");
 Serial.println(RiseTime);
  sat=1;
  }
  else{
   doFalling();
 FallTime=millis();
 Serial.print("fallTime = ");
 Serial.println(FallTime);
    sat=0;
  }
  delta = FallTime - RiseTime;
  Serial.print("delta = ");
  Serial.println(delta);
  
  if((delta > 6000UL) && (delta < 12000UL)){
    //digitalWrite(redled,HIGH);
    doReset();
  }
}
void doReset(){
      Serial.println("reseting...................");
      for (int i = 0; i < 160; ++i)
      {
        EEPROM.write(i, 0);
      }
      EEPROM.end();
      Serial.println("EEPROM reset");
      digitalWrite(l1,LOW);
      digitalWrite(ledgreen,HIGH);
      digitalWrite(ledblue,HIGH);
      ESP.reset();
}
void doRising(){
  Serial.println("rising loop");
   
  if(sata==0){
  digitalWrite(l1, HIGH); // Interrupt occurred, so turn On LED
  digitalWrite(ledgreen, LOW);
  digitalWrite(ledblue,LOW);
  sata=1;
  
  }
  else{
    digitalWrite(l1,LOW);
    digitalWrite(ledgreen,HIGH);
    sata=0;
  }
}
void doFalling(){
   
  Serial.println("falling loop");
 // sat=0;
}
