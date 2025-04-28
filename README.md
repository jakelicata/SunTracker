## Overview

This project implements a solar panel sun-tracking system using an Arduino UNO R4 Minima, DS3231 Real-time Clock Module , A4988 Stepper Motor Driver, Nema 17 Bipolar Stepper Motor, 2 KBT 12V 2600mAh rechargeable batteries, and a variation of Ken Willmott's SunPosition library (https://github.com/KenWillmott/SolarPosition).

It is composed of two main files:
- **SunTrackingMode** — tracks the sun daily, keeping up with the position of the sun.
- **SimulationMode** — Demonstrates the functionality of our system by cycling through rotations and simulating a day of tracking.

Both files control a mechanically geared solar tracker that utilizes a single actuator (Nema 17 Bipolar Stepper Motor).

Additionally, we have kept some of the older code files in the folder "Old_Prototyping_Code" in order to fully document out process.


## Hardware Used
- Arduino UNO R4 Minima
- DS3231 Real-time Clock Modul
- A4988 Stepper Motor Driver
- Nema 17 Stepper Motor Bipolar (L=48mm w/ Gear Ratio 27:1 Planetary Gearbox)
- 2 KBT 12 V 2600mAh rechargeable batteries (wired in series)
- Solar Panel Gear System with Dual-Axis Movement

<img width="650" alt="Screenshot 2025-04-28 at 11 05 34 AM" src="https://github.com/user-attachments/assets/b4560993-84a4-4762-a612-cb0d578cea58" />

- ADD A COUPLE SOLID WORKS DRAWING


## Setup

(may require hand cranking for initial steups)

**SunTrackingMode**
- Ensure system is facing North (the normal of the solar panel)
- Ensure lines of the 6-bar linkage system are aligned
- Ensure that the azimuth rotation is not between "clicks"
  
**SimulationMode** 
- Ensure lines of the 6-bar linkage system are aligned
- Ensure that the azimuth rotation is not between "clicks"

</br>
</br>
</br>

- **YES** (to below)
<img width="350" alt="Screenshot 2025-04-28 at 11 33 14 AM" src="https://github.com/user-attachments/assets/c3ef35f0-657a-4fe1-9f7f-d2d59ae5f542" />
</br>
<img width="350" alt="Screenshot 2025-04-28 at 11 36 01 AM" src="https://github.com/user-attachments/assets/a7876df6-b09f-459d-8385-1e366a7fda58" />



- **NO** (to below)
<img width="350" alt="Screenshot 2025-04-28 at 11 33 20 AM" src="https://github.com/user-attachments/assets/aa8e0242-6d74-448e-a358-9a2d4ee48af6" />




## Warning/Tips

Mechanical Variability:
- Actual steps for rotation/elevation may drift over time due to wear, temperature or environmental distrubances.

Manual Changes:
- If making changes to code, ensure that system is tested again and everything is recalibrated (adjustments in steps may be needed)

Overheating:
- When running simulation mode, ensure to not run too frequently to avoid motor driver from overheating.



## SunTrackingMode


## SimulationMode


## Future Improvements
- More elevation changes throughout year (potentially even daily elevation adjustment)
- Finetuning of mechanical system to allow for more accuracy






