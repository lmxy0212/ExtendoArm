#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 5
#define IN2 6
#define IN1 7

const int buttonPin = 13;
int buttonState = 0; 
// motor direction
int dir = 1;

const int A0_pin = A0;  // Analog input pin for muscle 1
const int A1_pin = A1;  // Analog input pin for muscle 2
const int samples = 10;  // Number of samples
const float lowerThresholdFilter = 0.9;  // Lower threshold for filtering
const float upperThresholdFilter = 1.2;  // Upper threshold for filtering
const int peakCountRequired = 3;  // Number of peaks required
const unsigned long peakDetectionTime = 3000;  // Time frame for peak detection (in milliseconds)
int cocontracting = 0;

double valorSensor0, valorSensor1;
double valorEmg0, valorEmg1;
double norm0,norm1;
double v_rec0[samples], v_rec1[samples];  // Arrays to store rectified values // 4000 milliseconds = 4 seconds
int num = 0;  // Sample counter
int peakCount = 0;

unsigned long peakStartTime = 0;
// int peakCount0 = 0;
// int peakCount1 = 0;



void setup() {
  Serial.begin(9600);
  pinMode(PWM,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

  Serial.println("Arduino MLX90614 Testing");  
  mlx.begin();
  
}

void loop() {
  
  // Capture data from channel A0 (muscle 1 - m1)
  valorSensor0 = analogRead(A0_pin);
  valorEmg0 = map(valorSensor0, 0, 1023, 0, 500); //#don't edit!
  norm0 = valorEmg0-149; //#don't edit!

  // Rectification of channel A0 (subtracting 1.5 volts to center at 0) 
  v_rec0[num] = abs(norm0);
  

  // Capture data from channel A1 (muscle 2 - m2)
  valorSensor1 = analogRead(A1_pin);
  valorEmg1 = map(valorSensor1, 0, 1023, 0, 350); 
  norm1 = valorEmg1- 104; //#don't edit!

  // Rectification of channel A1
  v_rec1[num] = abs(norm1);


  if (num == (samples - 1)) {
    // Calculate RMS for each channel for each sample
    float rms0 = calculateRMS(v_rec0, samples);
    float rms1 = calculateRMS(v_rec1, samples);
    
    // Check if RMS values are within the specified range
    if ((rms0 > lowerThresholdFilter && rms0 < upperThresholdFilter)&&(rms1 > lowerThresholdFilter && rms1 < upperThresholdFilter)){
      peakCount++;
    }
    // if {
    //   peakCount1++;
    // }

    // Check if the time frame for peak detection has elapsed
    if (millis() - peakStartTime >= peakDetectionTime) {  
      // Reset counts and time
      // peakCount0 = 0;
      // peakCount1 = 0;
      reset();
    }

    // Check if both signals hit above 0.9 and below 1.2 at least three times within 3.5 seconds
    if (peakCount >= peakCountRequired && peakCount >= peakCountRequired) {
      cocontracting = 2;
    }else {
      cocontracting = 0;
    }

    

    Serial.print("Min:0,Max:3");
    Serial.print("\tCo:");
    Serial.print(cocontracting);
    Serial.print("\tRMS0:");
    Serial.print(rms0);
    Serial.print("\tRMS1:");
    Serial.print(rms1);
    Serial.println("");

    //reset counter and window
    if (cocontracting == 2) {
//      delay(1000);
      setMotor(dir, 255, PWM, IN1, IN2);
      Serial.println("Rotating");
      dir = -dir; // reverse direction
      delay(280);
      setMotor(1, 0, PWM, IN1, IN2);
      Serial.println("Stoping");
      reset();
    }
    


    // Reset the sample counter
    num = 0;
  }

  num = num + 1;
  delay(4);

  
//  buttonState = digitalRead(buttonPin);
//  
//  if (buttonState == LOW) {
//    setMotor(dir, 255, PWM, IN1, IN2);
//    Serial.println("Rotating");
//    dir = -dir; // reverse direction
//    delay(280);
//    setMotor(1, 0, PWM, IN1, IN2);
//    Serial.println("Stoping");
//  }
  
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

void readTempreture(){
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  Serial.println();
}

int reset(){
      cocontracting = 0;
      peakCount = 0; 
      peakStartTime = millis(); // reset window
      return 0;
}
float calculateRMS(double *array, int length) {
  long sumSquared = 0;

  for (int i = 0; i < length; i++) {
    sumSquared += array[i] * array[i];
  }

  return sqrt(sumSquared / length);
}

// Function to check if both signals hit above a threshold at least a certain number of times
bool checkSignalCondition(float signal1, float signal2, float lowerThreshold, float upperThreshold, int countRequired) {
  static int count1 = 0;
  static int count2 = 0;

  if (signal1 > lowerThreshold && signal1 < upperThreshold) {
    count1++;
  } else {
    count1 = 0;
  }

  if (signal2 > lowerThreshold && signal2 < upperThreshold) {
    count2++;
  } else {
    count2 = 0;
  }

  return (count1 >= countRequired) && (count2 >= countRequired);
}
