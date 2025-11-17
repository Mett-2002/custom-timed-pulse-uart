# Custom UART Communication with STM32F103C4

![Project Screenshot](project_screenshot.png)

This project implements a **software-based UART-like protocol** between two STM32F103C4 microcontrollers. Its main purpose is **educational**: to demonstrate how byte-level serial communication can be emulated in software using GPIOs and timing functions, without relying on hardware UART peripherals.

**UART (Universal Asynchronous Receiver/Transmitter)** is a standard serial communication protocol that transmits data one bit at a time, using **start and stop bits** to frame each byte, normally requiring precise hardware timers to maintain a fixed baud rate. In this project, UART behavior is **simulated using timing-based pulses**, because microsecond-resolution timers are not used.

Using `HAL_GetTick()` and `HAL_Delay()`, each bit is encoded as a millisecond-scale pulse, and the receiver decodes them via GPIO interrupts. This creates a **reliable, software-timed communication channel** that is fun to experiment with and great for understanding low-level serial communication.

---

## 🎯 Project Goals

* Provide an **engaging, educational experience** in serial communication and MCU programming.
* Explore **software-timed communication** and interrupt handling on STM32 MCUs.
* Demonstrate a **transmitter and receiver design** for byte-level data transfer in a simulated environment.
* Help learners **visualize UART timing, bit encoding, and decoding** without needing hardware peripherals.

---

## ⚡ Quick Overview

* **Transmitter:** Reads characters from Terminal 1, converts each to an 8-bit sequence, and sends timed pulses on a GPIO pin.
* **Receiver:** Uses edge-triggered interrupts and `HAL_GetTick()` to measure pulse durations, decode bits, reassemble bytes, and print characters on Terminal 2.
* **Purpose:** Demonstrates **software-timed serial communication** to work around timing limitations in Proteus/HAL.

---

## 📏 Protocol Specification (Timing)

|               Signal | Nominal Duration |
| -------------------: | ---------------: |
| Start bit (preamble) |           200 ms |
|     Bit `1` (active) |           150 ms |
|     Bit `0` (active) |            50 ms |
|    Inter-bit spacing |            50 ms |

**Notes:**

* Inter-bit spacing equals the `0` pulse duration to simplify decoding.
* Receiver measures the **active pulse width** (rising → falling) and classifies it as `1` or `0` using a tolerance window (±10 ms recommended).

---

## 🔧 Hardware & Wiring

* **MCUs:** Two STM32F103C4 devices (or Proteus MCU instances)
* **Terminals:** Terminal 1 = transmitter input; Terminal 2 = receiver output
* **Wiring:** Transmitter `data_out` GPIO → Receiver interrupt-capable GPIO; connect grounds
* **GPIO Configurations:**

  * `data_out`: open-drain with pull-up (idle = 1)
  * Receiver GPIO: pull-up enabled, interrupt on both edges

---

## 📡 Transmitter Design

**Setup:**

* UART reads characters from Terminal 1
* `data_out` configured as open-drain + pull-up (idle = 1)

**`send_char(int c)` Steps:**

1. Convert ASCII character `c` to an 8-bit array (`bits[]`)
2. Emit **start bit** (200 ms) to signal byte start
3. For each bit:

   * `1` → drive active for **150 ms**
   * `0` → drive active for **50 ms**
   * After each bit, drive idle for **50 ms**
4. Use `HAL_Delay(ms)` for timing

**Tips:**

* Optional short preamble (2–3 long pulses) for initial sync
* Add debug prints for timestamps or pulse durations to tune tolerance

---

## 🖥 Receiver Design

**Setup:**

* UART prints decoded characters to Terminal 2
* RX GPIO with pull-up and interrupt on rising & falling edges

**IRQ Handler & State:**

* Override `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` to timestamp edges
* Track `count` (initial `-1`) to detect start bit
* Use `bit_ready` flag to signal a pulse ready for processing

**`receive_char()` (high-level Steps):**

1. On `bit_ready`, compute pulse duration
2. Classify pulse:

   * ≈150 ms → `1`
   * ≈50 ms → `0`
   * Apply tolerance (±10 ms recommended)
3. Reassemble 8 bits into a byte, send to Terminal 2
4. Reset state for next byte

**Robustness:**

* Reset on out-of-range pulses
* Detect preamble to re-sync after errors

---

## ✅ Verification & Test Procedure

1. Flash transmitter firmware to MCU A and receiver firmware to MCU B (or run both in Proteus)
2. Wire `data_out` (A) → `EXTI` pin (B) and connect grounds
3. Open Terminal 1 (input) and Terminal 2 (output)
4. Type characters in Terminal 1; verify they appear in Terminal 2
5. Debug mismatches using `rise_time`, `count`, or `bits[]` and adjust tolerance

**Expected Result:** Characters typed on Terminal 1 appear identically on Terminal 2

---

## 🚀 How to Run

1. Compile & flash `main_tx.c` to MCU A and `main_rx.c` to MCU B
2. Wire `data_out` → `EXTI` pin and connect ground
3. Open Terminal 1 and Terminal 2
4. Type characters in Terminal 1; verify they appear on Terminal 2

---
