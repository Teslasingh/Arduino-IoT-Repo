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

/*int d5=5;
int d6=6;
int d7=7;
int d8=8;
int d9=9;
int d10=10;
int d11=11;
int d12=12;
int d13=13;

int d5value;
int d6value;
int d7value;
int d8value;
int d9value;
int d10value;
int d11value;
int d12value;
int d13value;
*/

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

/*pinMode(d5,INPUT);
pinMode(d6,INPUT);
pinMode(d7,INPUT);
pinMode(d8,INPUT);
pinMode(d9,INPUT);
pinMode(d10,INPUT);
pinMode(d11,INPUT);
pinMode(d12,INPUT);
pinMode(d13,INPUT);
*/
}

void loop() {
 //Serial.println("******************************");
 // Serial.print("\t");
 a0value= analogRead(a0);
// Serial.print("a=");
  Serial.print(a0value);
  Serial.print(" "); 


 a1value= analogRead(a1);
// Serial.print("b=");
 Serial.print(a1value);
 Serial.print(" ");

  a2value= analogRead(a2);
 //Serial.print("c=");
  Serial.print(a2value);
  Serial.print(" ");



  a3value= analogRead(a3);
 //Serial.print("a3=");
  Serial.print(a3value);
 Serial.print(" ");

 
  a4value= analogRead(a4);
 //Serial.print("a4=");
  Serial.print(a4value);
  Serial.print(" ");


  a5value= analogRead(a5);
 //Serial.print("a5=");
  Serial.println(a5value);
  //Serial.println(",");

/*
  a6value= analogRead(a6);
 Serial.print("a6= ");
  Serial.println(a6value);

  a7value= analogRead(a7);
 Serial.print("a7= ");
  Serial.println(a7value);
  */
/*
  d5value= digitalRead(d5);
  Serial.print("d5= ");
  Serial.println(d5value);

  d6value= digitalRead(d6);
  Serial.print("d6= ");
  Serial.println(d6value);

  d7value= digitalRead(d7);
  Serial.print("d7= ");
  Serial.println(d7value);

  d8value= digitalRead(d8);
  Serial.print("d8= ");
  Serial.println(d8value);
  
  d9value= digitalRead(d9);
  Serial.print("d9= ");
  Serial.println(d9value);

  d10value= digitalRead(d10);
  Serial.print("d10= ");
  Serial.println(d10value);

  d11value= digitalRead(d11);
  Serial.print("d11= ");
  Serial.println(d11value);

  d12value= digitalRead(d12);
  Serial.print("d12= ");
  Serial.println(d12value);

  d13value= digitalRead(d13);
  Serial.print("d13= ");
  Serial.println(d13value);
 */

//Serial.println("******************************");
//Serial.println();


  delay(20);
  

}
