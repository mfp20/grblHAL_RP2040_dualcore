## grblHAL RP2040 dual core driver

## 1. Introduction
**This is a development version, not for users. It can literally set your house on fire.**

This is a stripped down version of [grblHAL driver for the Raspberry Pi Pico RP2040 processor](https://github.com/grblHAL/RP2040) made to take advantage of core1. All not necessary features (ie: bluetooth, eeprom, keypad, sdcard) have been removed to make the development more comfortable.

It is made to work in conjunction with [grblHAL core offloaded](https://github.com/mfp20/grblHAL_core_offloaded).

## 2. Changes

Added methods to move data around the cores.

```c
status_code_t push_motion_data(char *block, char *message);
bool pop_motion_data(char *block, char *message);
bool push_core_data(int argc, char *argv[]);
bool pop_core_data(int argc, char *argv[]);
```

## 3. Notes

For more info have a look at the [original discussion](https://github.com/grblHAL/core/discussions/34).
