int a0=A0;
int a1= A1;
int a2=A2;
int a3=A3;
int a4=A4;
int a5=A5;
int a6=A6;
//int a7=A7;

int a0value=447;
int a1value=498;
int a2value=530;
int a3value=540;
int a4value=487;
int a5value=600;
int a6value;
//int a7value;
unsigned long prevTime=millis();
long interval=1000;
int LED_state=LOW;
#include <Servo.h> 

int servo1Pin = 3; 
int servo2Pin = 5; 
int servo3Pin = 6; 
int servo4Pin = 9; 
int servo5Pin = 10; 
int servo6Pin = 8; 
int buttonPin = 7;
int recled=2;
boolean setValue=0;
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
pinMode(a6,INPUT);
pinMode(buttonPin,INPUT);
pinMode(recled,OUTPUT);
//pinMode(a6,INPUT);
//pinMode(a7,INPUT);
 
 Servo1.attach(servo1Pin); 
 Servo2.attach(servo2Pin);
 Servo3.attach(servo3Pin);
 Servo4.attach(servo4Pin);
 Servo5.attach(servo5Pin);
 Servo6.attach(servo6Pin);
}

void loop() {
   int btValue=digitalRead(buttonPin);

    if(setValue==1){
    a0value= analogRead(a0);
    a1value= analogRead(a1);
    a2value= analogRead(a2);       ////anlog read pot value
    a3value= analogRead(a3);
    a4value= analogRead(a4);
    a5value= analogRead(a5);
    }
    
    a6value=analogRead(a6);
    
    unsigned long currentTime=millis();
    if(currentTime-prevTime>500){
      LED_state = !LED_state;
      digitalWrite(recled,LED_state);
      prevTime=currentTime;
    }
      
     
    if(btValue==HIGH){
      if(setValue==0){
        setValue=1;
        delay(300);
      }                                ///Button to Activate or Deactivate analogRead
      else{
        setValue=0;
        delay(300);
      }
    }
   Serial.print(btValue);
   Serial.print(" ");
    
   Serial.print(setValue);
   Serial.print(" ");

   Serial.print(a6value);
   Serial.print(" ");
    
// Seral.print("a=");
  Serial.print(a0value);
  Serial.print(" "); 
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
  

int ser1=map(a0value,100,800,0,180);
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
   Serial.println(ser6); ////
  // Serial.print(" ");
   
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50

  

  delay(10);
}
