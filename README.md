# Serial Port Connection in C++

This document provides guidance on how to **connect and interact with a serial port** using C++ on Linux systems. The instructions and examples are specifically tested on **NVIDIA TX1 and TX2 boards running Ubuntu**.

## Overview

Serial communication is commonly used for interacting with hardware devices such as microcontrollers, sensors, and modems. This document covers the following:

1. **Setting up a serial port.**
2. **Configuring baud rate and other settings.**
3. **Reading from and writing to the serial port.**

## Requirements

- **A Linux-based system (tested on Ubuntu).**
- **NVIDIA TX1 or TX2 board.**
- **A C++ compiler such as `g++`.**

## How to Use

1. **Identify the serial port on your device** (e.g., `/dev/ttyUSB*`).
