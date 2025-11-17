# Custom UART Communication with STM32F103C4

![Project Screenshot](project_screenshot.png)

This project implements a **custom UART-like protocol** between two STM32F103C4 microcontrollers. UART (Universal Asynchronous Receiver/Transmitter) is a standard protocol for serial communication using start/stop bits and a fixed baud rate. Here, we emulate UART behavior using **timing-based pulses** because Proteus and the STM32 HAL only provide millisecond-resolution timing—microsecond timers are unavailable. Using `HAL_GetTick()` and `HAL_Delay()`, bit values are encoded and decoded as distinct millisecond-scale pulse widths, enabling reliable byte-level communication in the simulator.

---

## Project Goals

- Implement a reproducible byte-level communication channel between two STM32F103C4 MCUs **without hardware UART or microsecond timers**.  
- Use only HAL timing functions (`HAL_Delay()` and `HAL_GetTick()`).  
- Provide a clear transmitter and receiver design, along with a test procedure and tuning guidance.

---

## Quick Overview

- **Transmitter:** Reads characters from Terminal 1, converts each to an 8-bit sequence, and sends timed pulses on a GPIO pin.  
- **Receiver:** Uses edge-triggered interrupts and `HAL_GetTick()` to measure pulse durations, decode bits, reassemble bytes, and print characters on Terminal 2.  
- **Purpose:** Demonstrates software-timed serial communication to work around timing limitations in Proteus/HAL.

---

## Protocol Specification (Timing)

| Signal               | Nominal Duration |
|---------------------:|----------------:|
| Start bit (preamble) | 200 ms          |
| Bit `1` (active)     | 150 ms          |
| Bit `0` (active)     | 50 ms           |
| Inter-bit spacing    | 50 ms           |

**Notes:**  
- Inter-bit spacing equals the `0` pulse duration to simplify decoding.  
- Receiver measures active pulse width (rising → falling) and classifies it as `1` or `0` using a tolerance window.

---

## Hardware & Wiring

- **MCUs:** Two STM32F103C4 devices (or Proteus MCU instances).  
- **Terminals:** Terminal 1 = transmitter input; Terminal 2 = receiver output.  
- **Wiring:** Transmitter `data_out` GPIO → Receiver interrupt-capable GPIO; connect grounds.  
- **GPIO Configs:** `data_out` as open-drain with pull-up (idle = 1). Receiver GPIO: pull-up enabled, interrupt on both edges.

---

## Transmitter Design

**Setup:**  
- UART reads characters from Terminal 1.  
- `data_out` configured as open-drain + pull-up (idle = 1).  

**send_char(int c):**  
1. Convert ASCII character `c` to an 8-bit array (`bits[]`).  
2. Emit **start bit** (200 ms) to signal byte start.  
3. For each bit:  
   - `1` → drive active for **150 ms**  
   - `0` → drive active for **50 ms**  
   - After each bit, drive idle for **50 ms**  
4. Use `HAL_Delay(ms)` for timing.

**Tips:**  
- Optional short preamble (2–3 long pulses) for initial sync.  
- Add debug prints for timestamps or pulse durations to tune tolerance.

---

## Receiver Design

**Setup:**  
- UART prints decoded characters to Terminal 2.  
- RX GPIO with pull-up and interrupt on rising & falling edges.

**IRQ Handler & State:**  
- Override `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` to timestamp edges.  
- Track `count` (initial `-1`) to detect start bit.  
- Use `bit_ready` flag to signal a pulse is ready for processing.

**receive_char() (high-level):**  
1. On `bit_ready`, compute pulse duration.  
2. Classify pulse:  
   - ≈150 ms → `1`  
   - ≈50 ms → `0`  
   - Apply tolerance (±10 ms recommended).  
3. Reassemble 8 bits into a byte, send to Terminal 2.  
4. Reset state for next byte.

**Robustness:**  
- Reset on out-of-range pulses.  
- Detect preamble to re-sync after errors.

---

## Verification & Test Procedure

1. Flash transmitter firmware to MCU A and receiver firmware to MCU B (or run both in Proteus).  
2. Wire `data_out` (A) → `EXTI` pin (B) and connect grounds.  
3. Open Terminal 1 (input) and Terminal 2 (output).  
4. Type characters in Terminal 1; verify they appear in Terminal 2.  
5. Debug mismatches using `rise_time`, `count`, or `bits[]` and adjust tolerance.

**Expected Result:** Characters typed on Terminal 1 appear identically on Terminal 2.

---

## How to Run

1. Compile & flash `main_tx.c` to MCU A and `main_rx.c` to MCU B.  
2. Wire `data_out` → `EXTI` pin and share ground.  
3. Open Terminal 1 and Terminal 2.  
4. Type characters in Terminal 1; verify Terminal 2 shows them.

---
