#include <SparkFun_ADXL345.h>
#include <Wire.h>
//#include <SparkFun_TB6612.h>

// Using GPIO Pins
const int AIN1 = 3;     // Direction pin 1
const int AIN2 = 4;     // Direction pin 2
const int PWMA = 5;     // PWM pin for speed control
const int STBY = 2;
const int CS = 12;
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

const int panel_limit_pin = 8; // other side wired to ground
bool panel_limit = LOW; // tracks limit switch

const float hoverDegree = 0; //degree
const float kp = .6;

// Initializing motor.
const int offsetA = 1;
//Motor levelingMotor = Motor(AIN1, AIN2, PWMA, offsetA, STBY);

int x, y, z;
float ax, ay, az;
float roll;
float pitch;

void setup() {
  Wire.begin();


  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(2);           // Give the range settings
  // Accepted values are 2g, 4g, 8g or 16g
  // Higher Values = Wider Measurement Range
  // Lower Values = Greater Sensitivity - Will hopefully use lower range
  // put your setup code here, to run once:
  //pinMode(limitSwitchPin, INPUT_PULLUP);

  //pinMode (outputA, INPUT);
  //pinMode (outputB, INPUT);

  pinMode (AIN1, OUTPUT);
  pinMode (AIN2, OUTPUT);
  pinMode (PWMA, OUTPUT);
  pinMode (STBY, OUTPUT);
  digitalWrite (STBY, HIGH);

  pinMode (CS, OUTPUT);

  digitalWrite (CS, HIGH);

  pinMode(panel_limit_pin, INPUT_PULLUP);

  //delay(500);
}

void loop() {

  panel_limit = digitalRead(panel_limit_pin);
  if (panel_limit == HIGH) {
    writeDataToPi("panel_limit:1"); // Panel limit switch is pressed
  }

  adxl.readAccel(&x, &y, &z);  // Get acceleration values

  // Convert raw values to g-force
  ax = x * 0.0039; // Scale factor for 2G range (3.9mg per LSB)
  ay = y * 0.0039;
  az = z * 0.0039;

  // Calculate angles (in degrees)
  roll  = atan2(ay, az) * 57.2958;  // Convert radians to degrees
  pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 57.2958;

  // Print the values
  //Serial.print("Roll: "); Serial.print(roll);
  //Serial.print("°  Pitch: "); Serial.print(pitch);
  //Serial.println("°");
  float dir1 = 1;
  if (pitch < 0) {
    dir1 = -1;
    pitch = pitch * -1;
  }
  if (pitch > 2) {
    driveMotor(255, dir1, pitch * kp);
  }
}
void driveMotor(int speed, int dir, int duration) {
  // Set direction
  if (dir == 1) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (dir == -1) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    // Stop if direction is invalid
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  analogWrite(PWMA, speed); // Set speed

  delay(duration);  // Run motor for set time

  // Stop motor
  analogWrite(PWMA, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}

void writeDataToPi(String message) {
  delay(500);
  //Serial.println(message)
}

