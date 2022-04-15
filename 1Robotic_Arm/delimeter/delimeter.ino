#include <NeoSWSerial.h>

#include<Servo.h>
NeoSWSerial hc12(2,3);
Servo throttle;
Servo rudder;
Servo ailerons;
Servo elevators;
Servo servo5;
Servo servo6;
String input;

int boundLow;
int boundHigh;

const char delimiter = ',';

void setup(){
  throttle.attach(9);
  rudder.attach(10);
  ailerons.attach(11);
  elevators.attach(6);
  hc12.begin(9600);
  Serial.begin(9600);
}
void loop(){
  if(hc12.available())
  {
    input=hc12.readStringUntil('\n');
    if(input.length()>0)
    {
    //  Serial.println(input);
      boundLow=input.indexOf(delimiter);
     int throttleV=input.substring(0,boundLow).toInt();

      boundHigh=input.indexOf(delimiter,boundLow+1);
     int rudderV=input.substring(boundLow+1,boundHigh).toInt();

      boundLow=input.indexOf(delimiter,boundHigh+1);
      int aileronsV=input.substring(boundHigh+1,boundLow).toInt();

      boundHigh=input.indexOf(delimiter,boundLow+1);
      int elevatorsV=input.substring(boundLow+1,boundHigh).toInt();

      boundLow=input.indexOf(delimiter,boundHigh+1);
      int servo5=input.substring(boundHigh+1,boundLow).toInt();

      boundHigh=input.indexOf(delimiter,boundLow+1);
      int servo6=input.substring(boundLow+1,boundHigh).toInt();

          
           Serial.print(rudderV);
           Serial.print(";");
            Serial.print(throttleV);
            Serial.print(";");
            Serial.print(aileronsV);
            Serial.print(";");
            Serial.print(elevatorsV);
            Serial.println("");
           
      throttle.write(throttleV);
      rudder.write(rudderV);
      ailerons.write(aileronsV);
      elevators.write(elevatorsV);
      
    
    }
  }
}
