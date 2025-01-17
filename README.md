# README Documentation

## Overview
This project provides a serial communication system that interfaces with a device to configure its frame rates, read responses, and log data. The system uses threads for asynchronous reading and parsing of incoming serial data. The application also logs critical operations and device responses to a timestamped file for debugging and analysis.

## Features

### Serial Communication
- Communicates with a device via a serial port (`/dev/ttyUSB0`) using a configurable baud rate (default: 115200).
- Supports sending predefined commands for configuration and frame rate settings.

### Frame Rate Configuration
- Commands to set the device frame rates:
  - 1000 Hz, 500 Hz, 250 Hz, 125 Hz, 100 Hz.
- Enter and exit configuration modes.

### Threaded Processing
- **Read Thread**: Reads incoming serial data and writes it to a circular buffer.
- **Parse Thread**: Processes data from the buffer and logs information such as distance and configuration changes.

### Logging
- Data is logged into a timestamped file in the format:  
  `./data_<YYYY>_<MMDD>_<HHMMSS>.txt`.
- Logs include:
  - Frame rate changes.
  - Configuration mode transitions.
  - Distance measurements with timestamps.

### Checksum Validation
- Ensures data integrity by calculating and verifying checksum values.

### Command-Line Interface
- Prompts the user to input frame rate settings and configuration commands dynamically.

## Setup and Execution

### Dependencies
- **C++11 or later.**
- Required libraries:
  - `pthread` (for multithreading).
  - `unistd.h` and `fcntl.h` (for low-level file handling).
  - `time.h` (for timestamping).

### Compilation
Compile the code using `g++` with the necessary flags:
```bash
g++ -o serial_comm main.cpp circularBuffer.cpp serial_port.cpp -lpthread
