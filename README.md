# USB Power & Current Meter Dongle (v1.0)

A custom-designed, inline USB power, voltage, and current meter built from scratch—covering schematic capture, PCB layout in KiCad, hand-soldering assembly, and bare-metal C firmware development for the STM32G030 microcontroller.

<img width="3008" height="4000" alt="power" src="https://github.com/user-attachments/assets/4082ea0b-b179-4c6a-a2f0-2c27cce8578e" />


---

## Key Features

* **High-Precision Sensing:** Uses the Texas Instruments **INA226** bi-directional current and power monitor with a 16-bit Delta-Sigma ADC over a **10 mOhm (0.01 Ohm)** shunt resistor.
* **Hardware Signal Filtering:** Configured for hardware-level averaging of **64 samples** (register value 0x4727), effectively smoothing out high-frequency switching noise and pulsed current draws typical of modern USB SMPS chargers.
* **Hardware Power Calculation:** Internal calibration register (CAL = 5120) enables direct hardware multiplication for current (0.1 mA/LSB) and power (2.5 mW/LSB) readings without burdening the Cortex-M0+ core, which lacks a hardware FPU.
* **Non-Blocking Firmware Architecture:** Fully asynchronous main loop driven by HAL_GetTick() state timers with zero HAL_Delay() calls during normal operation.
* **Low-Power Optimization:** Utilizes the Cortex-M0+ Sleep Mode via the __WFI() (Wait For Interrupt) instruction at the end of every loop cycle, keeping the CPU core asleep for ~95% of the time between SysTick interrupts.
* **Live Diagnostics:** Dual status LEDs indicating real-time I2C bus health for both the OLED display (0x3C) and the INA226 sensor (0x40) (solid = OK, rapid 10 Hz blinking = device disconnected/NACK).
* **Crisp UI:** 128x64 **SSD1306 OLED** display refreshed at 5 Hz (200 ms intervals) for optimal readability.

---

## Hardware Architecture & Specifications

| Parameter | Specification |
| :--- | :--- |
| **Microcontroller** | STM32G030 (ARM Cortex-M0+) |
| **Current/Voltage Sensor** | TI INA226 (I2C Address: 0x40) |
| **Shunt Resistor** | 10 mOhm (0.01 Ohm) |
| **Display** | SSD1306 OLED 128x64 (I2C Address: 0x3C) |
| **Passive Footprint Size** | **0805** (selected specifically for manual soldering) |
| **Toolchain** | STM32CubeMX, CMake, Ninja, arm-none-eabi-gcc |

---

## Hardware Bring-Up, Assembly & Design Notes (Rev 1.0)

This board was designed with manual assembly and ease of debugging in mind:

1. **Hand-Soldering Friendly (0805 Passives):**  
   All passive components (resistors, capacitors, inductors) were intentionally chosen in the **0805 package**, allowing the entire PCB to be assembled cleanly using a standard **pencil soldering iron** without requiring a reflow oven or hot-air station.
2. **Power Tree & Unpopulated LDO Stage (U3, R12, C9, C10 - DNP):**  
   The initial schematic included an LDO linear voltage regulator stage (U3 along with R12, C9, and C10) downstream of the step-down buck converter. During step-by-step hardware bring-up, testing of the assembled switching converter revealed a stable output of **3.35 V**. Because this is well within the safe operating specifications for the STM32G030, INA226, and SSD1306, the LDO stage was left unpopulated (**DNP**) and bypassed.
3. **External Crystal Oscillator (HSE vs. HSI):**  
   Although the external crystal oscillator and its load capacitors were soldered onto the board, the HSE clock source was left inactive in the firmware. The MCU runs reliably on its internal **HSI RC oscillator (16 MHz)**, which provides more than enough timing accuracy for I2C communication and UI refresh intervals.
4. **Mechanical Notes for Rev 2.0 (OLED Mounting Holes):**  
   The four corner mounting holes originally placed for mechanical standoffs under the OLED module proved unnecessary. Soldering the display via its standard THT pin header provides rigid, more than sufficient mechanical stability. These mounting holes will be removed in Revision 2.0 to reduce the overall PCB footprint.

---

## Gallery

### KiCad Design & 3D Render
<img width="1111" height="517" alt="front_routing" src="https://github.com/user-attachments/assets/82c2ddd6-d86c-4f08-a2da-2e481a1e6bb4" />

<img width="1105" height="507" alt="back_routing" src="https://github.com/user-attachments/assets/1b9abc8c-150a-4368-9747-65091e8daf9d" />

<img width="1192" height="522" alt="3D_model" src="https://github.com/user-attachments/assets/2f409499-44f4-4ea0-a0c8-950e26cee9e7" />


### Assembled Hardware 
<img width="3008" height="4000" alt="front" src="https://github.com/user-attachments/assets/861b33ac-2e14-49c0-aaf3-81346e05d9b7" />

<img width="3008" height="4000" alt="back" src="https://github.com/user-attachments/assets/e66ab65d-1de0-4713-8b36-601215c87caf" />


