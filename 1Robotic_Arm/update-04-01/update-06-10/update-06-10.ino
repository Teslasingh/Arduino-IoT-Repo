#include<SPI.h>
#include<SD.h>
#include<Servo.h>

String buffer;
const char delimiter = ',';
int boundLow;
int boundHigh;

const int chipSelect=4;
boolean stay=0;
boolean stay2=0;
boolean onetime=1;
boolean setValue=0;

int recled=2;
unsigned long prevTime=millis();
long interval=1000;
int LED_state=LOW;


int a0=A0;
int a1= A1;
int a2=A2;
int a3=A3;
int a4=A4;
int a5=A5;
int a6=A6;


int a0value=500;
int a1value=510;
int a2value=530;
int a3value=540;
int a4value=487;
int a5value=600;
int a6value;

int servo1Pin = 3; 
int servo2Pin = 5; 
int servo3Pin = 6; 
int servo4Pin = 9; 
int servo5Pin = 10; 
int servo6Pin = 8; 
int buttonPin = 7;

Servo Servo1; 
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
Servo Servo6;


void setup() {
  pinMode(recled,OUTPUT);
  pinMode(buttonPin,INPUT);
  Serial.begin(9600);
 Servo1.attach(servo1Pin); 
 Servo2.attach(servo2Pin);
 Servo3.attach(servo3Pin);
 Servo4.attach(servo4Pin);
 Servo5.attach(servo5Pin);
 Servo6.attach(servo6Pin);
  while(!Serial){
    ;
  }
  Serial.print("Initializing SD card...");
  if(!SD.begin(chipSelect)){
    Serial.println("Card failed,or not present");
    while(1);
  }
  Serial.println("card intialized.");

}

void loop() {
 
  a6value=analogRead(a6);
  //Serial.print("a6value = ");
  //Serial.println(a6value);
  
  
  
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
if(a6value==1023 || stay==1){   //write data on sdCard AKA record.
  String dataString="";
  stay=1;
  
     unsigned long currentTime=millis();
    if(currentTime-prevTime>500){                  //rec led blink code without delay
      LED_state = !LED_state;
      digitalWrite(recled,LED_state);
      prevTime=currentTime;
    }
      
   // Serial.print("one time = ");
    //Serial.println(onetime);
    
    if(onetime==1){
    SD.remove("datalog.txt");
    Serial.println("data delete");
    delay(1000);
    onetime=0;
        }

      if(a6value==340||a6value==341||a6value==342){  ////stop button push in
        stay=0;
      }
    
  for(int analogPin = 0; analogPin < 6; analogPin++){  //0 to 5 analog pin as input
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if(analogPin < 5){
      dataString+=",";
    }
    }

    
  File dataFile = SD.open("datalog.txt",FILE_WRITE);
  if (dataFile){
    dataFile.println(dataString);
    dataFile.close();
  //  Serial.println(dataString);
    }

  else {
    Serial.println("error opening datalog.txt");
  }
      boundLow=dataString.indexOf(delimiter);
     int a0value=dataString.substring(0,boundLow).toInt();

      boundHigh=dataString.indexOf(delimiter,boundLow+1);
    int  a1value=dataString.substring(boundLow+1,boundHigh).toInt();

      boundLow=dataString.indexOf(delimiter,boundHigh+1);
     int a2value=dataString.substring(boundHigh+1,boundLow).toInt();    ///get seprate data from the string which are seprated by ","

      boundHigh=dataString.indexOf(delimiter,boundLow+1);
     int a3value=dataString.substring(boundLow+1,boundHigh).toInt();

      boundLow=dataString.indexOf(delimiter,boundHigh+1);
     int a4value=dataString.substring(boundHigh+1,boundLow).toInt();

      boundHigh=dataString.indexOf(delimiter,boundLow+1);
     int a5value=dataString.substring(boundLow+1,boundHigh).toInt();

    //  serialPrintServo();
  int ser1=map(a0value,100,800,0,180);
  int ser2=map(a1value,100,800,0,160);
  int ser3=map(a2value,860,260,0,180);    //Map pot value to servo motors.
  int ser4=map(a3value,857,260,0,180);
  int ser5=map(a4value,158,808,180,0);
  int ser6=map(a5value,600,761,0,50);
  
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50
  
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(a6value==0 || stay2==1){    /////read sdCard AKA play
  File dataFile = SD.open("datalog.txt");
  stay2=1;
   
      
  if(dataFile){
   
   while(dataFile.available()){             //change to if(dataFile.available()) , in case while dose not work
    //  Serial.write(dataFile.read());   //uncomment to go to back to code to just read and write on Serial monitor
     // Serial.println("writing done");
   buffer = dataFile.readStringUntil('\n');
   //  Serial.println(buffer); 
     delay(10);                                //decrease this delay to increase speed the playback action
     boundLow=buffer.indexOf(delimiter);
     int  a0value=buffer.substring(0,boundLow).toInt();

      boundHigh=buffer.indexOf(delimiter,boundLow+1);
     int a1value=buffer.substring(boundLow+1,boundHigh).toInt();

      boundLow=buffer.indexOf(delimiter,boundHigh+1);
      int a2value=buffer.substring(boundHigh+1,boundLow).toInt();

      boundHigh=buffer.indexOf(delimiter,boundLow+1);
      int a3value=buffer.substring(boundLow+1,boundHigh).toInt();        ///get seprate data from the string which are seprated by ","

      boundLow=buffer.indexOf(delimiter,boundHigh+1);
      int a4value=buffer.substring(boundHigh+1,boundLow).toInt();

      boundHigh=buffer.indexOf(delimiter,boundLow+1);
      int a5value=buffer.substring(boundLow+1,boundHigh).toInt();
     
      // serialPrintServo();

  int ser1=map(a0value,100,800,0,180);
  int ser2=map(a1value,100,800,0,160);
  int ser3=map(a2value,860,260,0,180);   //Map pot value to servo motors.
  int ser4=map(a3value,857,260,0,180);
  int ser5=map(a4value,158,808,180,0);
  int ser6=map(a5value,600,761,0,50);
  
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50

       if(a6value==340||a6value==341||a6value==342){  ////stop button push in 
        stay2=0;
        dataFile.close();
        delay(10);
      }
   
   }

    dataFile.close();
  
  }
  
}
  /////////////////////////////////////////////////////////////////////////////////////////////////////
else{   
     digitalWrite(recled,LOW);      
  //   Serial.println("main loop");
        onetime=1;     
 int btValue=digitalRead(buttonPin);
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
    if(setValue==1){
    a0value= analogRead(a0);
    a1value= analogRead(a1);
    a2value= analogRead(a2);       ////anlog read pot value
    a3value= analogRead(a3);
    a4value= analogRead(a4);
    a5value= analogRead(a5);
    /*
    Serial.print("a0value= ");
    Serial.println(a0value);
    Serial.print("a1value= ");
    Serial.println(a1value);             //uncomment for debuging.
    Serial.print("a2value= ");
    Serial.println(a2value);
    Serial.print("a3value= ");
    Serial.println(a3value);
    Serial.print("a4value= ");
    Serial.println(a4value);
    Serial.print("a5value= ");
    Serial.println(a5value);
    */
    }
  int ser1=map(a0value,100,800,0,180);
  int ser2=map(a1value,100,800,0,160);
  int ser3=map(a2value,860,260,0,180);    //Map pot value to servo motors.
  int ser4=map(a3value,857,260,0,180);
  int ser5=map(a4value,158,808,180,0);
  int ser6=map(a5value,600,761,0,50);
  
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50


  }
  
}
/*
void servo(){
  int ser1=map(a0value,100,800,0,180);
  int ser2=map(a1value,100,900,0,160);
  int ser3=map(a2value,860,260,0,180);    //Map pot value to servo motors.
  int ser4=map(a3value,857,260,0,180);
  int ser5=map(a4value,158,808,180,0);
  int ser6=map(a5value,600,761,0,50);
  
   Servo1.write(ser1); //ground servo
   Servo2.write(ser2);   /// 0-90-160
   Servo3.write(ser3);   ///0 -100 -180
   Servo4.write(ser4);    //  0-95-180
   Servo5.write(ser5);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(ser6);    ///gripper 0-50
 
}
*/
/*

void serialPrintServo(){
      Serial.print("servo1= ");
      Serial.println(servo1);
      Serial.print("servo2= ");
      Serial.println(servo2);
      Serial.print("servo3= ");         //uncomment for debuging
      Serial.println(servo3);
      Serial.print("servo4= ");
      Serial.println(servo4);
      Serial.print("servo5= ");
      Serial.println(servo5);
      Serial.print("servo6= ");
      Serial.println(servo6);
}
*/
