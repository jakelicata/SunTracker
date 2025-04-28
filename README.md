## Overview

This project implements a solar panel sun-tracking system using an Arduino UNO R4 Minima, DS3231 Real-time Clock Module , A4988 Stepper Motor Driver, Nema 17 Bipolar Stepper Motor, 2 KBT 12V 2600mAh rechargeable batteries, and a variation of Ken Willmott's SunPosition library (https://github.com/KenWillmott/SolarPosition).

It is composed of two main files:
- **SunTrackingMode** — tracks the sun daily, keeping up with the position of the sun.
- **SimulationMode** — Demonstrates the functionality of our system by cycling through rotations and simulating a day of tracking.

Both files control a mechanically geared solar tracker that utilizes a single actuator (Nema 17 Bipolar Stepper Motor).

Additionally, we have kept some of the older code files in the folder "Old_Prototyping_Code" in order to fully document out process.


## Hardware Used
- Arduino Uno R3
- A4988 Stepper Motor Driver
•	DS3231 RTC Module
•	Nema 17 Stepper Motor (17HS19-1684S-PG27 with 26.85:1 gearbox)
•	Solar Panel Mount with Dual-Axis Movement
•	External 12V or 24V power supply (for motor)

<img width="784" alt="Screenshot 2025-04-28 at 11 05 34 AM" src="https://github.com/user-attachments/assets/b4560993-84a4-4762-a612-cb0d578cea58" />



