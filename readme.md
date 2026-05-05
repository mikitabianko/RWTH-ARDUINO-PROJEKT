# RWTH Arduino Projekt — Handheld Game Console
A custom-built handheld game console based on **Arduino (AVR/Compatible)**, featuring a modular application framework, a pseudo-3D raycasting engine, and a hardware abstraction layer.
## Overview
This project is an experimental embedded system developed at **RWTH Aachen**, combining:
- **Low-level hardware control**: Direct interfacing with OLED displays and custom input hardware.
- **Custom Game Engine**: Features a fixed-point math library and a Doom-style 3D raycasting engine.
- **Modular Architecture**: An OOP-based "kernel" that manages app states (Menu vs. Games).
---
## **Features**
### Core System
- **Tick-based Runtime**: Managed game loop with configurable update and render periods.
- **App Orchestrator**: Uses a `Runtime` manager to hot-swap between different games and the main menu.
- **Fixed-Point Math**: Optimized `fix16` arithmetic for high-performance calculations on 8-bit MCUs.
### Hardware Abstraction Layer (HAL)
- **Display Abstraction**: Interface-based design supporting `SH110X` OLED controllers.
- **Advanced Input Handling**:
    - **Joystick**: Analog processing with software-defined deadzones.
    - **Analog Button Ladder**: Supports 5 buttons on a single ADC pin using a resistor ladder.
    - **Software Debouncing**: Built-in timing for reliable button state transitions (`pressed`, `released`, `held`).
### Game Engine
- **Raycasting Engine (Game1)**: A pseudo-3D engine using DDA (Digital Differential Analyzer) algorithm. 
- **Precomputed Look-Up Tables (LUTs)**: Sin/Cos tables and delta-distance arrays to maximize FPS.
- **Dynamic Shading**: Distance-based pixel shading for the 3D environment.
### Games
- **Game1**: 3D Raycaster demo.
- **Snake**: Classic Nokia-style snake game.
- **Dino**: Chrome-inspired endless runner.
- **Flappy**: Flappy Bird clone.
- **TicTacToe & Casino**: Logic and chance-based mini-games.
---
## Architecture

```
+----------------------------+
|   Games (Dino, Snake, etc) |
+----------------------------+
|   App Framework (App.h)    |
+----------------------------+
|   Core (Runtime & System)  |
+----------------------------+
|   HAL / Drivers (SH110X)   |
+----------------------------+
|   Hardware (Arduino/MCU)   |
+----------------------------+
```
## Hardware
- **Board**: Arduino Uno R3 / Nano or compatible AVR board.
- **Input**:
    - 2-axis Analog Joystick (Pins A0, A1).
    - Digital Joystick Button (Pin 10).
    - 5-button Analog Resistor Ladder (Pin A2).
- **Output**:
    - **Display**: SH110X OLED (128x64) via **I2C** (Address: 0x3C).
## Getting Started
### Requirements
- [PlatformIO](https://platformio.org/) (Recommended) or Arduino IDE.
- Required Libraries: (Refer to `platformio.ini` for specific dependencies).
### Build & Upload
``` bash
# Using PlatformIO
pio run
pio run --target upload
```
## Project Structure
- `/src/core` — `System` (HAL init), `Runtime` (Loop manager), `App` (Base class).
- `/src/drivers` — Implementations for SH110X, Analog Buttons, and Digital Buttons.
- `/src/games` — Logic for all 6+ integrated games.
- `/src/utils` — `Fp.h` Fixed-point math utilities.
- `Config.h` — Centralized pinout and hardware thresholds.
