# USB Power & Current Meter Dongle (v1.0)

A custom-designed, inline USB power, voltage, and current meter built from scratch—covering schematic capture, PCB layout in KiCad, hand-soldering assembly, and bare-metal C firmware development for the STM32G030 microcontroller.

# Working Device
<img width="3008" height="4000" alt="power" src="https://github.com/user-attachments/assets/3050ac83-bd66-4253-883c-587a068c4582" />


---

## Key Features

* **High-Precision Sensing:** Uses the Texas Instruments **INA226** bi-directional current and power monitor with a 16-bit $\Delta\Sigma$ ADC over a **$10\text{ m}\Omega$ ($0.01\text{ }\Omega$)** shunt resistor[cite: 1].
* **Hardware Signal Filtering:** Configured for hardware-level averaging of **64 samples** (`0x4727`), effectively smoothing out high-frequency switching noise and pulsed current draws typical of modern USB SMPS chargers.
* **Hardware Power Calculation:** Internal calibration register (`CAL = 5120`) enables direct hardware multiplication for current ($0.1\text{ mA/LSB}$) and power ($2.5\text{ mW/LSB}$) readings without burdening the Cortex-M0+ core (which lacks a hardware FPU).
* **Non-Blocking Firmware Architecture:** Fully asynchronous main loop driven by `HAL_GetTick()` state timers—zero `HAL_Delay()` calls during runtime.
* **Low-Power Optimization:** Utilizes the Cortex-M0+ Sleep Mode via the `__WFI()` (*Wait For Interrupt*) instruction at the end of every loop cycle, keeping the CPU core asleep for ~95% of the time between `SysTick` interrupts.
* **Live Diagnostics:** Dual status LEDs indicating real-time $\text{I}^2\text{C}$ bus health for both the OLED display (`0x3C`) and the INA226 sensor (`0x40`) (solid = OK, rapid 10 Hz blinking = device disconnected/NACK).
* **Crisp UI:** $128 \times 64$ **SSD1306 OLED** display refreshed at 5 Hz ($200\text{ ms}$ intervals) for optimal human-eye readability.

---

## Hardware Architecture & Specifications

| Parameter | Specification |
| :--- | :--- |
| **Microcontroller** | STM32G030 (ARM Cortex-M0+) |
| **Current/Voltage Sensor** | TI INA226 ($\text{I}^2\text{C}$ Address: `0x40`) |
| **Shunt Resistor ($R_{\text{shunt}}$)** | $10\text{ m}\Omega$ ($0.01\text{ }\Omega$) |
| **Display** | SSD1306 OLED $128 \times 64$ ($\text{I}^2\text{C}$ Address: `0x3C`) |
| **Passive Footprint Size** | **0805** (selected specifically for manual soldering) |
| **Toolchain** | STM32CubeMX, CMake, Ninja, `arm-none-eabi-gcc` |

---

## Hardware Bring-Up, Assembly & Design Notes (Rev 1.0)

This board was designed with manual assembly and ease of debugging in mind:

1. **Hand-Soldering Friendly (0805 Passives):**  
   All passive components (resistors, capacitors, inductors) were intentionally chosen in the **0805 package**, allowing the entire PCB to be assembled cleanly using a standard **pencil soldering iron** without requiring a reflow oven or hot-air station.
2. **Power Tree & Unpopulated LDO Stage (`U3`, `R12`, `C9`, `C10` - DNP):**  
   The initial schematic included an LDO linear voltage regulator stage (`U3` along with `R12`, `C9`, and `C10`) downstream of the step-down buck converter[cite: 1]. During step-by-step hardware bring-up, testing of the assembled switching converter revealed a stable output of **$3.35\text{ V}$**. Because this is well within the safe operating specifications for the STM32G030, INA226, and SSD1306, the LDO stage was left unpopulated (**DNP**) and bypassed.
3. **External Crystal Oscillator (HSE vs. HSI):**  
   Although the external crystal oscillator and its load capacitors were soldered onto the board, theHSE clock source was left inactive in the firmware. The MCU runs reliably on its internal **HSI RC oscillator ($16\text{ MHz}$)**, which provides more than enough timing accuracy for $\text{I}^2\text{C}$ communication and UI refresh intervals.
4. **Mechanical Notes for Rev 2.0 (OLED Mounting Holes):**  
   The four corner mounting holes[cite: 1] originally placed for mechanical standoffs under the OLED module proved unnecessary. Soldering the display via its standard THT pin header provides rigid, more than sufficient mechanical stability. These mounting holes will be removed in Revision 2.0 to reduce PCB footprint area.

---

## Gallery

### KiCad Design & 3D Render
| Top Routing (Front) | Bottom Routing (Back) | 3D PCB Render |
| :---: | :---: | :---: |
| ![KiCad Top](docs/images/kicad_top.jpg) | ![KiCad Bottom](docs/images/kicad_bottom.jpg) | ![KiCad 3D](docs/images/kicad_3d.jpg) |

### Assembled Hardware & Live Operation
| Assembled PCB (Top) | Assembled PCB (Bottom) | Live Measurement |
| :---: | :---: | :---: |
| ![PCB Top](docs/images/pcb_top.jpg) | ![PCB Bottom](docs/images/pcb_bottom.jpg) | ![Working OLED](docs/images/working_oled.jpg) |

---

## Firmware Build Instructions

The project uses a modern CMake + Ninja build system generated via STM32CubeMX.

```bash
# 1. Configure the CMake build directory
cmake -B build/Debug -G Ninja

# 2. Compile and link the firmware
cmake --build build/Debug
