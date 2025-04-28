## Overview

This project implements a solar panel sun-tracking system using an Arduino UNO R4 Minima, DS3231 Real-time Clock Module , A4988 Stepper Motor Driver, Nema 17 Bipolar Stepper Motor, 2 KBT 12V 2600mAh rechargeable batteries, and a variation of Ken Willmott's SunPosition library (https://github.com/KenWillmott/SolarPosition).

It is composed of two main files:
- **SunTrackingMode** — tracks the sun daily, keeping up with the position of the sun.
- **SimulationMode** — Demonstrates the functionality of our system by cycling through rotations and simulating a day of tracking.

Both files control a mechanically geared solar tracker that utilizes a single actuator (Nema 17 Bipolar Stepper Motor).

Additionally, we have kept some of the older code files in the folder "Old_Prototyping_Code" in order to fully document out process.


