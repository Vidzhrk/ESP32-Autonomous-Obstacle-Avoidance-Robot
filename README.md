# Overview

This project implements an autonomous 4-wheel mobile robot using an ESP32 microcontroller.
The robot detects obstacles using ultrasonic sensors and automatically chooses a safe path by scanning left and right directions using servo-mounted sensors.

The system supports:

Automatic obstacle avoidance

Manual Bluetooth control

Servo-based environment scanning

Real-time motor speed control using PWM


# Objectives

Build an intelligent navigation robot

Implement sensor-based decision making

Integrate embedded systems + control logic

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


