# ece5610-project - Self-Balancing Weight Scale

A self-balancing weight scale built around a Raspberry Pi Pico. A potentiometer senses a change in the beam's angle, a PID controller running at 100Hz computes a corrective velocity, and a stepper motor actuates a counterweight on a ball screw to balance the system.

<img src="https://i.imgur.com/Nmfapxe.jpeg" height="80%" width="80%" alt="Disk Sanitization Steps"/>

## Build & flash

The Pico SDK is resolved from `PICO_SDK_PATH`, not vendored.
 
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
 
cmake -B build -S .
cmake --build build
```
Hold `BOOTSEL`, plug in the Pico, and copy `build/ece5610-project.uf2` to the mounted mass-storage device.

Serial output is on USB CDC at 115200 baud.
