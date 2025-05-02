## Overview

Current single-axis and fixed solar panel installations capture only a fraction of potential solar energy, resulting in suboptimal power generation. The primary limitation of these solar panels is the inability to dynamically adjust panel orientation to match the sun's changing position throughout the day and across seasons. Existing two-axis tracking solutions are prohibitively expensive and power hungry, typically requiring two separate actuators for azimuth and elevation rotation.  

This report centers on the development of an efficient sun-tracking solar panel system. The objective is to develop a cost-effective single-actuator two-axis tracking mechanism that can dynamically optimize panel alignment, maximize energy capture, and reduce overall costs. 

In this introduction, we outline the fundamental purpose of our project: to enhance solar energy absorption by creating an affordable and efficient tracking system that overcomes the limitations of fixed and single-axis panels. The following sections will delve into the details of our requirements review, system architecture, prototype results, design analysis and research, project management, and final solution. 

This project implements a solar panel sun-tracking system using an Arduino UNO R4 Minima, DS3231 Real-time Clock Module , A4988 Stepper Motor Driver, Nema 17 Bipolar Stepper Motor, 2 KBT 12V 2600mAh rechargeable batteries, and a variation of Ken Willmott's SunPosition library (https://github.com/KenWillmott/SolarPosition).

It is composed of two main files:
- **SunTrackingMode** — tracks the sun daily, keeping up with the position of the sun.
- **SimulationMode** — Demonstrates the functionality of our system by cycling through rotations and simulating a day of tracking.

Both files control a mechanically geared solar tracker that utilizes a single actuator (Nema 17 Bipolar Stepper Motor).

Additionally, we have kept some of the older code files in the folder "Old_Prototyping_Code" in order to fully document out process.

Lastly, there remains **a folder named "Old_Protoypting_Code."** This folder is for the sole purpose of maintaining old files for the contributers to go back to document progress, and are not needed for any other reason.


## Hardware Used
- Arduino UNO R4 Minima
- DS3231 Real-time Clock Modul
- A4988 Stepper Motor Driver
- Nema 17 Stepper Motor Bipolar (L=48mm w/ Gear Ratio 27:1 Planetary Gearbox)
- 2 KBT 12 V 2600mAh rechargeable batteries (wired in series)
- Solar Panel Gear System with Dual-Axis Movement

<img width="650" alt="Screenshot 2025-04-28 at 11 05 34 AM" src="https://github.com/user-attachments/assets/b4560993-84a4-4762-a612-cb0d578cea58" />

## Physical Prototype

<img width="386" alt="Screenshot 2025-05-01 at 8 51 11 PM" src="https://github.com/user-attachments/assets/e544afe6-c2b2-41c5-969f-cab7d5157979" />


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

**Brief Overview:**
- Sun Position Tracking: Calculates real-time solar elevation and azimuth using a variation of Ken Willmott's SolarPosition algorithm.
- Motor Control: Adjusts solar panel position with A4988 stepper motor driver.
- Seasonal Adjustment: Automatically shifts tilt for spring, summer, fall, and winter.
- Daylight Saving Time: Automatically detects and adjusts between EST and EDT.
- Leap Year Awareness: Correctly accounts for leap years in date tracking.
- Night Mode: Sleeps the system when the sun sets and automatically wakes near sunrise.

**Functions:**
- Sleep(time)​ -> Puts our driver/motor to sleep for specified time​
- isLeapYear(year) -> determines if its a leap year
- returnToNorth() -> returns the motor to a northern facing position
- isDST(dateTime) -> adjusts utc_offset for daylight savings
- GetSEA(day of year, hour, minute, utc offset, lat, long)​ -> Determines sun elevation based on time and location​
- GetAZ(day of year, hour, minute, utc offset, lat, long)​ -> Determines sun elevation based on time and location​
- CalculateSunrise(day of year, utc offset, lat, long)​ -> Determines when sunrise will be for next day and how long the nightime will be​
- SeasonalChangeElevation(dayOfYear)​ -> Determines when to change elevation (when the season has officially changed)​
- AdjustMotorAz(suns azimuth position, panel azimuth direction)​ -> Compares suns position to solar panel and adjusts solar panel until within specific threshold
- RotatemotorSteps(steps, direction)​ -> Rotate motor a specific number of steps either counter clockwise or clockwise (elevation or azimuth)​

**Useage**
1. Follow Manual Setup Instructions
2. Add adjustments to code as needed (long/lat, timezone offset, etc.)
3. Upload code to Arduino
  
​


## SimulationMode

**Brief Overview:**
- Purpose: Demonstrates the system’s capability to adjust azimuth and elevation accurately, and validates full control of movement.
- Upon running, the user is prompted via the Arduino Serial Monitor to enter:
  - 1 for a full daily cycle simulation,
  - 2 for a full azimuth rotation,
  - 3 for a full elevation cycle.

- Option 1: Generalized Day Simulation
  - Elevates tracker halfway up.
  - Rotates 24.8° azimuth.
  - Elevates to full height.
  - Rotates 37.2° azimuth.
  - Waits at top position.
  - Rotates 24.8° azimuth.
  - Lowers to halfway position.
  - Rotates 24.8° azimuth.
  - Lowers to bottom position.
  - Rotates 24.8° azimuth.
  - Waits at bottom position.
  - Rotates azimuth back to the starting position.

- Option 2: Full Azimuth Rotation
  - Performs a complete 360° rotation in the azimuth direction.

- Option 3: Full Elevation Cycle

  - Elevates from the starting position to maximum height.
  - Lowers back down to the starting position.



**Functions:**
- Sleep(time) –> Puts the driver and motor into a low-power sleep mode for the specified duration (in milliseconds).
- azimuthCycle() –> Performs a full azimuth rotation of the system.
- elevationCycle() –> Performs a full elevation cycle of the system.
- daySimulation() –> Performs the afirmentioned day simulation of the system (the description of which can be found in the preceding paragraph.
- rotateMotor(steps, direction)  –> Rotates the stepper motor a specific number of steps in the desired direction (clockwise or counterclockwise) to control azimuth or elevation. 

**Useage**
1. Follow Manual Setup Instructions
2. Upload code to Arduino







