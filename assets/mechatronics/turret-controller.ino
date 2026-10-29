#include <Pixy2.h>  // Pixy2 camera library
#include <PID_v1.h>  // PID control library
#include <Servo.h>  // Servo library

// Motor control pin definitions
#define enA 5  // Motor enable pin (connected to L293 enA)
#define in1 6  // Motor direction control pin 1
#define in2 7  // Motor direction control pin 2

// Global variable and object declarations
int led = 2;      // Placeholder for LED pin (unused in this version)
Pixy2 pixy;       // Pixy2 object to interact with the camera

bool turningRight;

Servo servoLeft;
Servo servoRight;

const int blue = 2;
const int red = 1;
const int target = blue;

const int buttonPin = 3;  // Define the pin where the button is connected
int reloadLeft = 52;
int readyLeft = 147;
int fireLeft = 162;

int sequenceState = 3;

int reloadRight = 130;
int readyRight = 33;
int fireRight = 18;

int stepDelay = 12;
unsigned long stepMillis = 0;
unsigned long currentMillis = millis();
unsigned long lastShotMillis;
unsigned long lastShotTimeDelta;

const long interval = 200;  // The interval time (in milliseconds) between servo movements

// PID variables and parameters
const int CENTER_X = 159;  // Assuming a resolution of 320x200, the center X position is at 159
int error = 0;
double Setpoint, Input, Output;
double Kp = 0.7, Ki = 0.05, Kd = 0.3;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  Serial.begin(115200);        // Initialize serial communication for debugging
  Serial.print("Starting...\n");

  servoLeft.attach(9);  // Attach the servos to digital pins 9 and 10
  servoRight.attach(10);
 
  // Initial servo positions (optional: can be used to set a default starting position)
  servoLeft.write(readyLeft);
  servoRight.write(readyRight);

  // Configure motor control pins
  pinMode(enA, OUTPUT);       // Set enA as output
  pinMode(in1, OUTPUT);       // Set in1 as output
  pinMode(in2, OUTPUT);       // Set in2 as output

  digitalWrite(enA, HIGH);    // Enable motor driver
  delay(1000);                // Short delay for initialization

  pixy.init();                // Initialize the Pixy2 camera

  // Setpoint is the center of the field of view
  Setpoint = 0;
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-150, 150); // Set limits for the PID output to reduce jerky movements
}

void loop()
{
  int i;  // Loop variable

  // Request Pixy2 to detect blocks (objects in its view)
  pixy.ccc.getBlocks();

  bool target_indicator = false;  // Flag to indicate if a target is detected

  // Check if any blocks (objects) are detected
  if (pixy.ccc.numBlocks)
  {
    int areas[pixy.ccc.numBlocks];   // Array to store areas of detected blocks
    int x_vals[pixy.ccc.numBlocks];  // Array to store x-coordinates of detected blocks

    // Iterate through all detected blocks
    for (i = 0; i < pixy.ccc.numBlocks; i++)
    {
      /*Serial.print("  block ");     // Debugging: Print block index
      Serial.print(i);
      Serial.print(": ");
      */
      pixy.ccc.blocks[i].print();   // Debugging: Print block details

      // Check if the block matches the desired signature (e.g., red balloon)
      if (pixy.ccc.blocks[i].m_signature == target)
      {
        target_indicator = true;  // Set flag to true, indicating a target is detected

        // Calculate the block's area and x-coordinate
        int area = pixy.ccc.blocks[i].m_width * pixy.ccc.blocks[i].m_height;
        int x_pos = pixy.ccc.blocks[i].m_x;

        areas[i] = area;   // Store the area in the array
        x_vals[i] = x_pos; // Store the x-coordinate in the array
        
        // Find the block with the largest area (assumed to be the closest target)
        int max_area = 0;
        int max_area_pos;
        for (i = 0; i < pixy.ccc.numBlocks; i++)
        {
          if (areas[i] >= max_area)
          {
            max_area = areas[i];       // Update the largest area found
            max_area_pos = i;          // Update the index of the largest block
          }
        }

        int x_pos_max = pixy.ccc.blocks[max_area_pos].m_x;  // X-coordinate of the largest block

        // Calculate the error from the center
        error = x_pos_max - CENTER_X;
        Input = error;

        // Compute PID output
        myPID.Compute();

        // Control the motor based on PID output (reverse direction logic)
        if (Output > 0)
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
        }
        else if (Output < 0)
        {
          digitalWrite(in1, HIGH);
          digitalWrite(in2, LOW);
        }
        else
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
        }

        // Set motor speed
        analogWrite(enA, constrain(abs(Output), 0, 150));  // Reduce speed for smoother movements

        currentMillis = millis();

        // Track and perform the firing sequence simultaneously
        lastShotTimeDelta = (currentMillis - lastShotMillis);
        if ((error > -20 && error < 20) && lastShotTimeDelta > 3000)  // If the target is within a certain range of the center
        {
          Serial.print("fired");
          fireAndReloadSequence();
        }
      }
      else
      {
        areas[i] = 0;  // Set area to 0 for blocks not matching the signature
        // If no target is detected, stop the motor
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(enA,80);
      }
    }

  }
  else
  {
    // If no target is detected, stop the motor
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enA,80);
  }
}

// Turret motor control functions
void turretStop(){ // Function to stop the turret motor
  digitalWrite(in1, LOW);  // Stop turret motor by setting both control pins to LOW
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}

void fireAndReloadSequence() {
  // Fire position
  sequenceState = 1;
  moveServosSimultaneously(servoLeft.read(), fireLeft, servoRight.read(), fireRight);
  waitForMovement();  // Wait until the servo movement is done
  lastShotMillis = millis();
  
  // Reload position
  sequenceState = 2;
  moveServosSimultaneously(servoLeft.read(), reloadLeft, servoRight.read(), reloadRight);

  // Ready position
  sequenceState = 3;
  moveServosSimultaneously(servoLeft.read(), readyLeft, servoRight.read(), readyRight);
}

// Function to move both servos simultaneously, 1 degree at a time
void moveServosSimultaneously(int currentLeftPos, int targetLeftPos, int currentRightPos, int targetRightPos) {
  int leftStep = (currentLeftPos < targetLeftPos) ? 1 : -1;
  int rightStep = (currentRightPos < targetRightPos) ? 1 : -1;

  // **Set the initial step time for control of timing**
  unsigned long nextStepMillis = millis();  

  while (currentLeftPos != targetLeftPos || currentRightPos != targetRightPos) {
    // **Update currentMillis inside the loop to track each step’s timing accurately**
    unsigned long currentMillis = millis();  

    // **Move both servos only when the step delay has passed**
    if (currentMillis >= nextStepMillis) {
      // Move left servo if it hasn't reached the target
      if (currentLeftPos != targetLeftPos) {
        currentLeftPos += leftStep;
        servoLeft.write(currentLeftPos);
      }

      // Move right servo if it hasn't reached the target
      if (currentRightPos != targetRightPos) {
        currentRightPos += rightStep;
        servoRight.write(currentRightPos);
      }

      // **Schedule the next step after the delay has passed**
      nextStepMillis = currentMillis + stepDelay;
    }
    int i;
        // Continue tracking while moving servos
      
      // Request Pixy2 to detect blocks (objects in its view)
      pixy.ccc.getBlocks();

      bool target_indicator = false;  // Flag to indicate if a target is detected

      // Check if any blocks (objects) are detected
      if (pixy.ccc.numBlocks)
      {
        int areas[pixy.ccc.numBlocks];   // Array to store areas of detected blocks
        int x_vals[pixy.ccc.numBlocks];  // Array to store x-coordinates of detected blocks

        // Iterate through all detected blocks
        for (i = 0; i < pixy.ccc.numBlocks; i++)
        {
          Serial.print("  block ");     // Debugging: Print block index
          Serial.print(i);
          Serial.print(": ");
          pixy.ccc.blocks[i].print();   // Debugging: Print block details

          // Check if the block matches the desired signature (e.g., red balloon)
          if (pixy.ccc.blocks[i].m_signature == target)
          {
            target_indicator = true;  // Set flag to true, indicating a target is detected

            // Calculate the block's area and x-coordinate
            int area = pixy.ccc.blocks[i].m_width * pixy.ccc.blocks[i].m_height;
            int x_pos = pixy.ccc.blocks[i].m_x;

            areas[i] = area;   // Store the area in the array
            x_vals[i] = x_pos; // Store the x-coordinate in the array

             // Find the block with the largest area (assumed to be the closest target)
            int max_area = 0;
            int max_area_pos;
            for (i = 0; i < pixy.ccc.numBlocks; i++)
            {
              if (areas[i] >= max_area)
              {
                max_area = areas[i];       // Update the largest area found
                max_area_pos = i;          // Update the index of the largest block
              }
            }

            int x_pos_max = pixy.ccc.blocks[max_area_pos].m_x;  // X-coordinate of the largest block

            // Calculate the error from the center
            error = x_pos_max - CENTER_X;
            Input = error;

            // Compute PID output
            myPID.Compute();

            // Control the motor based on PID output (reverse direction logic)
            if (Output > 0 && sequenceState != 1)
            {
              digitalWrite(in1, LOW);
              digitalWrite(in2, HIGH);
            }
            else if (Output < 0 && sequenceState != 1)
            {
              digitalWrite(in1, HIGH);
              digitalWrite(in2, LOW);
            }
            else
            {
              digitalWrite(in1, LOW);
              digitalWrite(in2, LOW);
            }

            // Set motor speed
            analogWrite(enA, constrain(abs(Output), 0, 150));  // Reduce speed for smoother movements
            
          }
          else
          {
            areas[i] = 0;  // Set area to 0 for blocks not matching the signature
              
              
            // If no target is detected, stop the motor
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            analogWrite(enA,80);
              
            }
        }

       
      }
      else
      {
        // If no target is detected, stop the motor
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(enA,80);
      }
  }
}

// Function to handle the timing of servo movements and avoid delays
void waitForMovement() {
  unsigned long startMillis = millis();  
  while (millis() - startMillis < interval) {
    // Do nothing here, just wait for the interval time to pass
  }
}

