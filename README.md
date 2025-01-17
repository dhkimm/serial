# Viewpro Camera Serial Control System

## Overview
This project provides a way to control a Viewpro camera via serial communication. Using predefined commands, the camera's gimbal movement and zoom levels can be controlled with precise instructions sent through a serial port.

## Features

### Gimbal Control
* Control gimbal movement:
  - Move left, right, up, down.
  - Stop gimbal motion.
* Additional functionalities:
  - Start and stop tracking.
  - Move the gimbal to a predefined home point.
  - Point to a specific tracking position.

### Zoom Control
* Supports predefined zoom levels:
  - 1x, 2x, 3x, 5x, 10x, 15x, 20x.

### Serial Communication
* Communicates with the camera via `/dev/ttyUSB0` at a baud rate of 115200.
* Uses predefined command arrays sent directly to the serial port.

## Setup and Execution

### Dependencies
* **C++11 or later.**
* Required libraries:
  - `pthread` (for multithreading).
  - `unistd.h` and `fcntl.h` (for low-level file handling).
  - A custom `Serial_port` library for handling serial communication.

### Compilation
Compile the gimbal and zoom control programs:
```bash
g++ -o gimbal_control gimbal_control.cpp serial_port.cpp -lpthread
g++ -o zoom_control zoom_control.cpp serial_port.cpp -lpthread
