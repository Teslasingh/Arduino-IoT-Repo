const byte        interruptPin = D5; // Or other pins that support an interrupt
volatile boolean  interrupt_occurred = false; 
volatile bool sat=0;
volatile bool sata=0;
unsigned long RiseTime;
unsigned long FallTime;
#include <EEPROM.h>
#define           LED_pin D1
#define         redled D7
//portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;
void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);   // LED pin to Output mode
  pinMode(redled, OUTPUT);
  digitalWrite(LED_pin, LOW); // Turn LED off (High = On, Low = Off)
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, CHANGE); //Respond to falling edges on the pin
 // attachInterrupt(digitalPinToInterrupt(interruptPin), IS, CHANGE);
 //.....read from eeprom
 Serial.println("Reading EEPROM ssid");

  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
//  Serial.println(esid.length());
  if(esid==0){
    Serial.println("eeprom empty");
  }
  ////.......end of reading
}

 
void loop() {
Serial.println("loop working");
  delay(10000);
  digitalWrite(redled,LOW);
  String qsid="m";
  Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
         EEPROM.commit();
}
 
ICACHE_RAM_ATTR void ISR () {                  // Interrupt Service Routine, come here when an interrupt occurs
  // unsigned long currentTime=millis();
  Serial.println("isr loop");
 
  if(sat==0){
  doRising();
 RiseTime=millis();
  sat=1;
  }
  else{
   doFalling();
 FallTime=millis();
    sat=0;
  }
  //interrupt_occurred = true;   // Record that an interrupt occurred
  if((FallTime - RiseTime > 7000UL) && (FallTime -  RiseTime < 12000UL) || (RiseTime - FallTime > 7000UL) && (RiseTime -  FallTime < 12000UL)){
    digitalWrite(redled,HIGH);
    doreset();
  }
}
void doRising(){
  Serial.println("rising loop");
   
  if(sata==0){
  digitalWrite(LED_pin, HIGH); // Interrupt occurred, so turn On LED
  sata=1;
  
  }
  else{
    digitalWrite(LED_pin,LOW);
    sata=0;
  }
}
void doFalling(){
   
  Serial.println("falling loop");
 // sat=0;
}
void doreset(){
   Serial.println("reseting...................");
        for (int i = 0; i < 160; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.end();
        Serial.println("EEPROM reset");
         ESP.reset();
}
