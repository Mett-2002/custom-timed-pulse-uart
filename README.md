# Custom UART Communication with STM32F103C4
![Project Screenshot](project_screenshot.png)
**Core technical challenge**  
This project implements a **custom UART-like protocol** between two STM32F103C4 microcontrollers using **timing-based pulses**. Proteus and the STM32 HAL provide only millisecond-resolution timing, so true microsecond timers are unavailable. The solution uses `HAL_GetTick()` and `HAL_Delay()` to encode and decode bit values as distinct millisecond-scale pulse widths, enabling reliable bit-level transmission in the simulator.

---

## Project goals (read first)
- Build a reproducible byte-level communication channel between two STM32F103C4 MCUs inside Proteus **without using hardware UART framing** or microsecond timers.
- Rely only on HAL timing functions (`HAL_Delay()` and `HAL_GetTick()`).
- Provide a clear transmitter and receiver design, a test procedure, and tuning guidance so others can reproduce the experiment.

---

## Quick summary
- **Transmitter:** Reads characters from Terminal 1, converts each character into an 8-bit sequence, and emits timed pulses on a GPIO pin.  
- **Receiver:** Uses an edge-triggered interrupt and `HAL_GetTick()` to measure pulse durations, decode bits, reassemble bytes, and print characters on Terminal 2.  
- **Why:** This is a practical workaround for Proteus/HAL timing limits that demonstrates software-timed serial communication.

---

## Protocol specification (timing)
| Signal              | Nominal duration |
|--------------------:|-----------------:|
| Start bit (preamble)| 200 ms           |
| Bit `1` (active)    | 150 ms           |
| Bit `0` (active)    | 50 ms            |
| Inter-bit spacing   | 50 ms            |

Notes:
- Inter-bit spacing equals the `0` pulse duration to simplify decoding.
- Receiver measures the active pulse (rising → falling) and classifies it as `1` or `0` using a tolerance window (see Tolerance & Reliability).

---

## Hardware & wiring
- **MCUs:** Two STM32F103C4 devices (or two Proteus MCU instances).  
- **Terminals:** Terminal 1 = transmitter input; Terminal 2 = receiver output.  
- **Wiring:** Transmitter `data_out` GPIO → Receiver interrupt-capable GPIO; connect grounds.  
- **GPIO configs:** `data_out` should be open-drain with pull-up (idle = 1). Receiver GPIO: pull-up enabled and interrupt-on-both-edges.

---

## Transmitter — configuration & algorithm
**Setup**
- Configure UART to read characters from Terminal 1.
- Configure `data_out` as open-drain + pull-up so idle logic is `1`.

**send_char(int c)** (high-level)
1. Read ASCII character `c` from UART.
2. Convert `c` to an 8-bit array `bits[]` (choose LSB-first or MSB-first; be consistent).
3. Emit a **start bit** (200 ms) to signal the receiver a byte is starting.
4. For each bit:
   - `1` → drive active for **150 ms**.
   - `0` → drive active for **50 ms**.
   - After the active period, drive **idle** for **50 ms**.
5. Use `HAL_Delay(ms)` for timing.

Implementation tips:
- Consider a short preamble (2–3 long pulses) for initial synchronization.
- Add debug prints (timestamps or pulse durations) to help tune tolerance.

---

## Receiver — configuration, IRQ handling & decoding
**Setup**
- Configure UART to print decoded characters to Terminal 2.
- Configure RX GPIO with internal pull-up and interrupt on rising & falling edges.

**IRQ handler & state**
- Override `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` to timestamp edges using `HAL_GetTick()`.
- Maintain `count` initialized to `-1` to detect the start bit first; `active` becomes `true` after the start is detected.
- Use a `bit_ready` flag set by the IRQ to indicate a measured pulse is ready for processing.

**receive_char()** (high-level)
1. On `bit_ready`, compute `rise_time = measured_pulse_ms - inter_bit_spacing_ms` if your measurement includes spacing.
2. Classify:
   - `rise_time` ≈ 150 ms → `1`
   - `rise_time` ≈ 50 ms → `0`
   - Use a tolerance (suggest ±10 ms initially).
3. Store bits until you have 8 bits; reconstruct the byte and send it to Terminal 2.
4. Reset state and wait for next start.

Robustness:
- If a measured pulse is out-of-range, reset and wait for next start bit.
- Detect preamble to re-sync after errors.

---

## Tolerance & reliability
- Start with **±10 ms** tolerance when distinguishing 150 ms vs 50 ms. Adjust based on observed jitter.
- Reliability strategies:
  - Send a preamble or sync word.
  - Send each byte multiple times and accept the majority.
  - Add an 8-bit checksum or simple CRC for integrity checking.

---

## Verification & test procedure
1. Flash transmitter firmware to MCU A and receiver firmware to MCU B (or instantiate both MCUs in Proteus).
2. Wire `data_out` (A) → `EXTI` pin (B) and connect GND.
3. Open Terminal 1 and Terminal 2.
4. Type characters in Terminal 1 and verify they appear in Terminal 2.
5. If mismatches occur, print debug values (`rise_time`, `count`, `bits[]`) and tune tolerance.

Expected result: characters typed on Terminal 1 appear identically on Terminal 2.

---

## Suggested file layout & key functions
Suggested files:
- `main_tx.c` — transmitter firmware (UART input, `send_char()`, pulse emission)
- `main_rx.c` — receiver firmware (EXTI callback override, `receive_char()`)
- Optional: project files / Makefile for each MCU

Key functions:
- `void send_char(int c)` — convert char to bits and send timed pulses.
- `void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` — timestamp edges and set `bit_ready`.
- `void receive_char(void)` — process `bit_ready` events and reconstruct bytes.

---

## How to run
1. Compile & flash `main_tx.c` to MCU A and `main_rx.c` to MCU B.  
2. Wire `data_out` → `EXTI` pin and share GND.  
3. Open Terminal 1 (input) and Terminal 2 (output).  
4. Type characters in Terminal 1; verify Terminal 2 shows the same characters.

---

