#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 5
#define IN2 6
#define IN1 7

const int buttonPin = 13;
int buttonState = 0; 
// motor direction
int dir = 1;


void setup() {
  Serial.begin(9600);
  pinMode(PWM,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  
}

void loop() {

  buttonState = digitalRead(buttonPin);
  
  if (buttonState == LOW) {
    setMotor(dir, 255, PWM, IN1, IN2);
    Serial.println("Rotating");
    dir = -dir; // reverse direction
    delay(700);
    setMotor(1, 0, PWM, IN1, IN2);
    Serial.println("Stoping");
  }
  
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
