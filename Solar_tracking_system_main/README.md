## Solar Tracking System

##  Introduction
This project is a **Solar Tracking System** designed to harness the maximum amount of energy from the sun.Traditional solar panels are fixed in a single position, which limits their ability to capture solar energy throughout the day.

##  How It Works
The system operates on a closed-loop control principle:
1.  **Sensing:** Two Light Dependent Resistors (LDRs) serve as sensors to detect sunlight intensity on the East and West sides].
2.  **Processing:** An Arduino Uno acts as the control unit.It compares the resistance values from the two LDRs.
3.  **Actuation:** If one LDR receives more light than the other, the Arduino signals the Servo Motor to rotate the panel toward the brighter side until the light is balanced.

##  Components Required
To build this project, you need the following hardware ]:
* **Microcontroller:** Arduino Uno 
* **Actuator:** Servo Motor 
* **Sensors:** 2x Light Dependent Resistors (LDRs) ]
* **Power:** Solar Panel
* **Resistors:** 2x 10k Ohm resistors (for voltage dividers)
* **Connection:** Jumper Wires
