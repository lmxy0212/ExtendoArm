#include <util/atomic.h> // For the ATOMIC_BLOCK macro

#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 5
#define IN2 6
#define IN1 7

volatile int posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
long prevT = 0;
float eprev = 0;
float eintegral = 0;
const int buttonPin = 13;
int buttonState = 0; 
// motor direction
int dir = 1;
//int flag = 1;

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  
  pinMode(PWM,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  
//  Serial.println("target pos");
}

void loop() {

  // set target position
//  int target = 90;
  int target = 90*sin(prevT/1e6);

  // PID constants
  float kp = 1;
  float kd = 0.025;
  float ki = 0.0;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }
  int e = pos - target;
  float dedt = (e-eprev)/(deltaT);
  eintegral = eintegral + e*deltaT;
  float u = kp*e + kd*dedt + ki*eintegral;

  
  float pwr = fabs(u);
  if( pwr > 255 ){
      pwr = 255;
    }

  
  if(u<0){
    pwr = 0;
//    flag = (-1)*flag;
  }
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    dir = dir * (-1);
    pwr = 255;
    Serial.println(dir);
    Serial.println(pwr);
  }else{
//    pwr = 0;
  }
  setMotor(dir,pwr,PWM,IN1,IN2);

  eprev = e;

  delay(100);
  
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(pwm,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}

void readEncoder(){
  int b = digitalRead(ENCB);
  if(b > 0){
    posi++;
  }
  else{
    posi--;
  }
}
