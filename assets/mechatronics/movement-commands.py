import serial

arduino = serial.Serial('/dev/ttyACM0', 9600)

def send_command(command):
    arduino.write(command.encode())

# Concise movement commands
def move_forward():
    send_command('MOVE_FORWARD')

def move_backward():
    send_command('MOVE_BACKWARD')

def move_left():
    send_command('MOVE_LEFT')

def move_right():
    send_command('MOVE_RIGHT')

def rotate_left():
    send_command('ROTATE_LEFT')

def rotate_right():
    send_command('ROTATE_RIGHT')
