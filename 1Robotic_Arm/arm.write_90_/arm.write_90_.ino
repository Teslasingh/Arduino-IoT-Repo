// Include the Servo library 
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
// pinMode(ir,INPUT);
 Servo1.attach(servo1Pin); 
 Servo2.attach(servo2Pin);
 Servo3.attach(servo3Pin);
 Servo4.attach(servo4Pin);
 Servo5.attach(servo5Pin);
 Servo6.attach(servo6Pin);
}
void loop(){ 

   Servo1.write(100); //ground servo
   Servo2.write(90);   /// 0-90-160
   Servo3.write(100);   ///0 -100 -180
   Servo4.write(95);    //  0-95-180
   Servo5.write(90);   ///  0-90-180 , 0 anti-clock and 90 normal pos.
   Servo6.write(0);    ///gripper 0-50
   delay(20);

}
