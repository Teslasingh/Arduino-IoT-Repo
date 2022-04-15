#define acs712 A0
float vpp= 0.0048828125;
void setup() {
  // put your setup code here, to run once:
pinMode (acs712,INPUT);
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int counts= analogRead(acs712);
  float voltage = counts*vpp;
Serial.println("Voltage" + String(voltage));

}
