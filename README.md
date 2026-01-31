# Overview

This project is a smart 4-wheel robotic vehicle built using Espressif Systems ESP32 that supports both:
1)Autonomous obstacle avoidance
2)Manual Bluetooth control
The robot can independently navigate obstacles using ultrasonic sensors and also be manually driven from a phone using Bluetooth commands.

This combines:
Embedded systems
Real-time control
Sensor fusion
Robotics navigation logic


# Objectives
Build an intelligent navigation robot
Implement sensor-based decision making
Integrate embedded systems + control logic

 # Key Features

1) Automatic Mode
Moves forward continuously
Measures front & back distance using ultrasonic sensors
Stops when obstacle detected
Moves slightly backward
Servo rotates sensor left & right
Chooses direction with maximum free space
Turns and continues navigation

2) Manual Mode (Bluetooth)
Controlled wirelessly from phone

Commands:
a → Forward
b → Backward
l → Left
r → Right
c → Brake
Adjustable speed using PWM

3) Extra:
Dual ultrasonic sensing (front + back)
Servo scanning
PWM motor speed control
State-machine based logic
Mode switching using interrupt pin

# Hardware Components
Component	Quantity
ESP32	1
L298N Motor Driver	2
DC Motors	4
Ultrasonic Sensors	2
Servo Motors	2
Battery Pack	1
Robot Chassis	1

# Pin Configuration

Motors
Function	GPIO
IN pins	23, 21, 14, 26
IN pins	22, 5, 27, 25
PWM enable	19, 18, 32, 33

Sensors
Device	GPIO
Front Trig	2
Front Echo	4
Back Trig	35
Back Echo	15

Servos
Servo	GPIO
US1	12
US2	13


