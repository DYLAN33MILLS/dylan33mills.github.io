#include <Arduino.h>
#define MOTOR_PWM 255

const int AIN1 = 4;  // Front-left wheel
const int AIN2 = 3;
const int PWMA = 2;

const int motor1IN1 = 4;
const int motor1IN2 = 3;
const int motor1PWM = 2;

const int BIN1 = 5;  // Front-right wheel
const int BIN2 = 6;
const int PWMB = 7;

const int motor2IN1 = 5;
const int motor2IN2 = 6;
const int motor2PWM = 7;

const int CIN1 = 10; // Rear-left wheel
const int CIN2 = 9;
const int PWMC = 8;

const int motor3IN1 = 10;
const int motor3IN2 = 9;
const int motor3PWM = 8;

const int DIN1 = 11; // Rear-right wheel
const int DIN2 = 12;
const int PWMD = 13;

const int motor4IN1 = 11;
const int motor4IN2 = 12;
const int motor4PWM = 13;

void setup() {
    // Initialize motor control pins as outputs
    pinMode(motor1IN1, OUTPUT);
    pinMode(motor1IN2, OUTPUT);
    pinMode(motor1PWM, OUTPUT);
    pinMode(motor2IN1, OUTPUT);
    pinMode(motor2IN2, OUTPUT);
    pinMode(motor2PWM, OUTPUT);
    pinMode(motor3IN1, OUTPUT);
    pinMode(motor3IN2, OUTPUT);
    pinMode(motor3PWM, OUTPUT);
    pinMode(motor4IN1, OUTPUT);
    pinMode(motor4IN2, OUTPUT);
    pinMode(motor4PWM, OUTPUT);

    // Initialize serial communication
    Serial.begin(9600);
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');

        if (command == "MOVE_FORWARD") {
            moveForward();
        } else if (command == "MOVE_BACKWARD") {
            moveBackward();
        } else if (command == "MOVE_LEFT") {
            moveLeft();
        } else if (command == "MOVE_RIGHT") {
            moveRight();
        } else if (command == "ROTATE_LEFT") {
            rotateLeft();
        } else if (command == "ROTATE_RIGHT") {
            rotateRight();
        } else if (command == "EXPLORE") {
            explore();
        } else if (command == "STOP"){
          Stop();
        }
        
    }
}

void moveForward() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, LOW);
    digitalWrite(motor1IN2, HIGH);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, LOW);
    digitalWrite(motor2IN2, HIGH);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, HIGH);
    digitalWrite(motor3IN2, LOW);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, HIGH);
    digitalWrite(motor4IN2, LOW);
}

void moveBackward() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, HIGH);
    digitalWrite(motor1IN2, LOW);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, HIGH);
    digitalWrite(motor2IN2, LOW);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, LOW);
    digitalWrite(motor3IN2, HIGH);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, LOW);
    digitalWrite(motor4IN2, HIGH);
}

void moveLeft() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, LOW);
    digitalWrite(motor1IN2, HIGH);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, HIGH);
    digitalWrite(motor2IN2, LOW);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, HIGH);
    digitalWrite(motor3IN2, LOW);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, LOW);
    digitalWrite(motor4IN2, HIGH);
}

void moveRight() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, HIGH);
    digitalWrite(motor1IN2, LOW);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, LOW);
    digitalWrite(motor2IN2, HIGH);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, LOW);
    digitalWrite(motor3IN2, HIGH);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, HIGH);
    digitalWrite(motor4IN2, LOW);
}

void rotateLeft() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, LOW);
    digitalWrite(motor1IN2, HIGH);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, HIGH);
    digitalWrite(motor2IN2, LOW);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, LOW);
    digitalWrite(motor3IN2, HIGH);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, HIGH);
    digitalWrite(motor4IN2, LOW);
}

void rotateRight() {
    analogWrite(motor1PWM, MOTOR_PWM);
    digitalWrite(motor1IN1, HIGH);
    digitalWrite(motor1IN2, LOW);

    analogWrite(motor2PWM, MOTOR_PWM);
    digitalWrite(motor2IN1, LOW);
    digitalWrite(motor2IN2, HIGH);

    analogWrite(motor3PWM, MOTOR_PWM);
    digitalWrite(motor3IN1, HIGH);
    digitalWrite(motor3IN2, LOW);

    analogWrite(motor4PWM, MOTOR_PWM);
    digitalWrite(motor4IN1, LOW);
    digitalWrite(motor4IN2, HIGH);
}

void explore() {
    // Set timestamps
    String command = Serial.readStringUntil('\n');
    unsigned long base_time = millis();
    //Randomly choose amount of time to turn
    int turn_time = random(0, 2000);
    int command_time = 0;
    //While pi tells arduino to keep exploring
    while (command == "EXPLORE"){
      while (command_time < turn_time){
        rotateRight();
        command = Serial.readStringUntil('\n');
        //delay(100);
        unsigned long current_time = millis();
        command_time = base_time - current_time;
      }
      moveForward();
      command = Serial.readStringUntil('\n');
    }

}

void rotateLeftSlowly() {
    analogWrite(motor1PWM, 128);  // Set speed to half
    digitalWrite(motor1IN1, LOW);
    digitalWrite(motor1IN2, HIGH);

    analogWrite(motor2PWM, 128);
    digitalWrite(motor2IN1, LOW);
    digitalWrite(motor2IN2, HIGH);

    analogWrite(motor3PWM, 128);
    digitalWrite(motor3IN1, HIGH);
    digitalWrite(motor3IN2, LOW);

    analogWrite(motor4PWM, 128);
    digitalWrite(motor4IN1, HIGH);
    digitalWrite(motor4IN2, LOW);
}

void Stop() {
  analogWrite(PWMA, 0); analogWrite(PWMB, 0); analogWrite(PWMC, 0); analogWrite(PWMD, 0);
  
  digitalWrite(motor1IN1, HIGH);      digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor1IN2, LOW);     digitalWrite(motor2IN2, LOW);
 
  digitalWrite(motor3IN1, LOW);      digitalWrite(motor4IN1, LOW);
  digitalWrite(motor3IN2, HIGH);     digitalWrite(motor4IN2, HIGH);
}
