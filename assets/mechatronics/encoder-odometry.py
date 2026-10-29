import RPi.GPIO as GPIO
import time
import math

# GPIO Pin definitions (BCM mode)
IR_SENSOR_PIN1 = 2  # Define GPIO pin for the first IR sensor
IR_SENSOR_PIN2 = 3  # Define GPIO pin for the second IR sensor

# Constants
WHEEL_RADIUS = 0.1  # Radius of the wheels in meters
NUMBER_OF_MARKS = 20  # Number of marks on each wheel
WHEEL_BASE = 0.3  # Distance between the two wheels in meters

# Variables
mark_count1 = 0
mark_count2 = 0
last_time = 0
x, y, theta = 0.0, 0.0, 0.0

# Callback functions for GPIO interrupts
def count_marks1(channel):
    global mark_count1
    mark_count1 += 1

def count_marks2(channel):
    global mark_count2
    mark_count2 += 1

# Setup GPIO
def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(IR_SENSOR_PIN1, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    GPIO.setup(IR_SENSOR_PIN2, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    GPIO.add_event_detect(IR_SENSOR_PIN1, GPIO.RISING, callback=count_marks1)
    GPIO.add_event_detect(IR_SENSOR_PIN2, GPIO.RISING, callback=count_marks2)

# Main function
def loop():
    global mark_count1, mark_count2, x, y, theta, last_time

    last_time = time.time()
    while True:
        current_time = time.time()
        elapsed_time = current_time - last_time

        # Calculate distance traveled for both wheels
        circumference = 2 * math.pi * WHEEL_RADIUS
        distance_traveled1 = (mark_count1 / float(NUMBER_OF_MARKS)) * circumference
        distance_traveled2 = (mark_count2 / float(NUMBER_OF_MARKS)) * circumference

        # Calculate speed (distance/time) for both wheels
        if elapsed_time > 0:
            speed1 = distance_traveled1 / elapsed_time
            speed2 = distance_traveled2 / elapsed_time

        # Calculate change in orientation and position
        dl = distance_traveled1
        dr = distance_traveled2
        d_theta = (dr - dl) / WHEEL_BASE
        d_s = (dr + dl) / 2

        # Update position
        x += d_s * math.cos(theta + d_theta / 2)
        y += d_s * math.sin(theta + d_theta / 2)
        theta += d_theta

        # Normalize theta to be within -pi to pi
        theta = (theta + math.pi) % (2 * math.pi) - math.pi

        # Print distance, speed, and position for both wheels
        print(f"Wheel 1 - Distance Traveled: {distance_traveled1:.2f} m	Speed: {speed1:.2f} m/s")
        print(f"Wheel 2 - Distance Traveled: {distance_traveled2:.2f} m	Speed: {speed2:.2f} m/s")
        print(f"Robot Position - X: {x:.2f} m, Y: {y:.2f} m, Theta: {theta:.2f} rad\n")

        # Reset the counters for the next interval
        mark_count1 = 0
        mark_count2 = 0
        last_time = current_time

        # Delay for 1 second
        time.sleep(1)

# Cleanup GPIO on exit
def cleanup():
    GPIO.cleanup()

