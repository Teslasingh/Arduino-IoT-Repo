#include<SPI.h>
#include<SD.h>
String buffer;
int a0=A0;
int a1= A1;
int a2=A2;
int a3=A3;
int a4=A4;
int a5=A5;

int a0value=447;
int a1value=498;
int a2value=530;
int a3value=540;
int a4value=487;
int a5value=600;

const int chipSelect=4;
boolean stay=0;
boolean stay2=0;
boolean onetime=1;

int a6=A6;
int a6value;

int recled=2;
unsigned long prevTime=millis();
long interval=1000;
int LED_state=LOW;

void setup() {
  pinMode(recled,OUTPUT);
  Serial.begin(9600);
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
  Serial.print("a6value= ");
  Serial.println(a6value);
 ////////////////////////////////////////////////////////////////////////////////////
  
  if(a6value==1023 || stay==1){   // stay conferms that we are in the if loop   if@@
  String dataString="";
  stay=1;
  
    Serial.print("one time");
    Serial.println(onetime);
    
    if(onetime==1){
    SD.remove("datalog.txt");
    Serial.println("data delete");
    delay(1000);
    onetime=0;
        }

      if(a6value==340||a6value==341||a6value==342){  ////stop button push in
        stay=0;
      }
    
  for(int analogPin = 0; analogPin < 6; analogPin++){  //0 to 5
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
    Serial.println(dataString);
     unsigned long currentTime=millis();
    if(currentTime-prevTime>500){                  //rec led blink code
      LED_state = !LED_state;
      digitalWrite(recled,LED_state);
      prevTime=currentTime;
    }
   // delay(5);
    }
  else {
    Serial.println("error opening datalog.txt");
  }
  }
////////////////////////////////////////////////////////////////////////////////////////
else if(a6value==0 || stay2==1){    /////read sdcard aka play
  File dataFile = SD.open("datalog.txt");
  stay2=1;
   
      
  if(dataFile){
   
   while(dataFile.available()){
    //  Serial.write(dataFile.read());   //uncomment to go to back to code
     // Serial.println("writing done");
   buffer = dataFile.readStringUntil('\n');
   Serial.println(buffer);                                   /////start here
  // Serial.println("end of line");
  // Serial.println(a6value);
   //myservo.write(buffer.toInt());
    if(a6value==340||a6value==341||a6value==342){  ////stop button push in
        stay2=0;
        dataFile.close();
       break;
      }
   }
    dataFile.close();
  }
  
}
  ///////////////////////////////////////////////////////////////////////////////////
  else{   
     digitalWrite(recled,LOW);      
       Serial.println("main loop");
        onetime=1;    
        delay(5);   

    a0value= analogRead(a0);
    a1value= analogRead(a1);
    a2value= analogRead(a2);       ////anlog read pot value
    a3value= analogRead(a3);
    a4value= analogRead(a4);
    a5value= analogRead(a5);
    Serial.print("a0value=");
    Serial.println(a0value);
  }
  
}
