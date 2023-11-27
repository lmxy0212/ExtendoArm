
const int A0_pin = A0;  // Analog input pin for muscle 1
const int A1_pin = A1;  // Analog input pin for muscle 2
const int samples = 10;  // Number of samples
const float lowerThresholdFilter = 0.9;  // Lower threshold for filtering
const float upperThresholdFilter = 1.2;  // Upper threshold for filtering
const int peakCountRequired = 3;  // Number of peaks required
const unsigned long peakDetectionTime = 3500;  // Time frame for peak detection (in milliseconds)


double valorSensor0, valorSensor1;
double valorEmg0, valorEmg1;
double norm0,norm1;
double v_rec0[samples], v_rec1[samples];  // Arrays to store rectified values // 4000 milliseconds = 4 seconds
int num = 0;  // Sample counter


unsigned long peakStartTime = 0;
int peakCount0 = 0;
int peakCount1 = 0;

void setup() {
  Serial.begin(9600);
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
  valorEmg1 = map(valorSensor1, 0, 1023, 0, 300); //#don't edit!
  norm1 = valorEmg1- 89; //#don't edit!

  // Rectification of channel A1
  v_rec1[num] = abs(norm1);


  if (num == (samples - 1)) {
    // Calculate RMS for each channel for each sample
    float rms0 = calculateRMS(v_rec0, samples);
    float rms1 = calculateRMS(v_rec1, samples);
    
    // // Check if RMS values are within the specified range
    // if (rms0 > lowerThresholdFilter && rms0 < upperThresholdFilter){
    //   peakCount0++;
    // }
    // if (rms1 > lowerThresholdFilter && rms1 < upperThresholdFilter){
    //   peakCount1++;
    // }

    // // Check if the time frame for peak detection has elapsed
    // if (millis() - peakStartTime >= peakDetectionTime) {  
    //   // Reset counts and time
    //   peakCount0 = 0;
    //   peakCount1 = 0;
    //   peakStartTime = millis();
    // }

    // // Check if both signals hit above 0.9 and below 1.2 at least three times within 3.5 seconds
    // if (peakCount0 >= 3 && peakCount1 >= peakCountRequired) {
    //   Serial.println("Condition Met: Both signals hit above 0.9 and below 1.2 at least three times within 3.5 seconds!");
    //   Serial.println("Min:0,Max:3");
    //   Serial.println(rms0);
    //   Serial.println(rms1);
    // } else {
    //   Serial.println("Condition Not Met");
    //   Serial.println("Min:0,Max:3");
    //   Serial.println(rms0);
    //   Serial.println(rms1);
    // }

    // Output or further processing of the features
    Serial.println("Min:0,Max:3");
    Serial.println(rms0);
    //Serial.print(volt0);
    Serial.print("  ");
    //Serial.println(norm0);
    //Serial.println(norm1);
    //Serial.println(v_rec1[num]);
    Serial.println (rms1);

    // Reset the sample counter
    num = 0;
  }

  num = num + 1;
  delay(4);
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
