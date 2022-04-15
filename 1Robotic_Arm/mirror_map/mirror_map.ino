int a0=A0;
int a1= A1;
int a2=A2;
int a3=A3;
int a4=A4;
int a5=A5;
//int a6=A6;
//int a7=A7;

int a0value;
int a1value;
int a2value;
int a3value;
int a4value;
int a5value;
//int a6value;
//int a7value;
const int numReadings = 10;
int readings[numReadings]; 
int readIndex = 0;              
int total = 0;                 
int average;

#include <Servo.h> 

int servo1Pin = 3; 
int servo2Pin = 5; 
int servo3Pin = 6; 
int servo4Pin = 9; 
int servo5Pin = 10; 
int servo6Pin = 8; 

//int ir=8;
//int irst;
Servo Servo1; 
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
Servo Servo6;

void setup() {
Serial.begin(9600);
pinMode(a0,INPUT);
pinMode(a1,INPUT);
pinMode(a2,INPUT);
pinMode(a3,INPUT);
pinMode(a4,INPUT);
pinMode(a5,INPUT);
//pinMode(a6,INPUT);
//pinMode(a7,INPUT);
for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

 Servo1.attach(servo1Pin); 
 Servo2.attach(servo2Pin);
 Servo3.attach(servo3Pin);
 Servo4.attach(servo4Pin);
 Servo5.attach(servo5Pin);
 Servo6.attach(servo6Pin);
}


void loop() {

    total = total - readings[readIndex];
    readings[readIndex] = analogRead(a0);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
    }
     average = total / numReadings;
     
    //a0value= analogRead(a0);
    a1value= analogRead(a1);
    a2value= analogRead(a2);       ////anlog read pot value
    a3value= analogRead(a3);
    a4value= analogRead(a4);
    a5value= analogRead(a5);
    
// Seral.print("a=");
 // Serial.print(a0value);
  //Serial.print(" "); 
// Serial.print("b=");
 Serial.print(a1value);
 Serial.print(" ");
 //Serial.print("c=");
  Serial.print(a2value);           //Serial Print pot raw value
  Serial.print(" ");
 //Serial.print("a3=");
  Serial.print(a3value);
 Serial.print(" ");
 //Serial.print("a4=");
  Serial.print(a4value);
  Serial.print(" ");
 //Serial.print("a5=");
  Serial.print(a5value);
  Serial.print("      ");
  

int ser1=map(average,100,800,0,180);
int ser2=map(a1value,150,750,0,160);
int ser3=map(a2value,860,260,0,180);    //Map pot value to servo motors.
int ser4=map(a3value,857,260,0,180);
int ser5=map(a4value,158,808,180,0);
int ser6=map(a5value,600,761,0,50);

   Serial.print(ser1); ////
   Serial.print(" ");
   Serial.print(ser2); ////
   Serial.print(" ");
   Serial.print(ser3); ////
   Serial.print(" ");
   Serial.print(ser4); ////
   Serial.print(" ");
   Serial.print(ser5); ////
   Serial.print(" ");
   Serial.print(ser6); ////
   Serial.print(" ");
   
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50

  

  delay(10);
}
