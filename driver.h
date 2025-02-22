/*

  driver.h - driver code for RP2040 ARM processors

  Part of grblHAL

  Copyright (c) 2021 Terje Io

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

*/

//
// NOTE: do NOT change configuration here - edit my_machine.h instead!
//

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <stdbool.h>
#include <stdint.h>

#ifndef OVERRIDE_MY_MACHINE
#include "my_machine.h"
#endif

#include "grbl/driver_opts.h"

#define DIGITAL_IN(bit) (!!(sio_hw->gpio_in & bit))
#define DIGITAL_OUT(bit, on) { if(on) sio_hw->gpio_set = bit; else sio_hw->gpio_clr = bit; }
#define GPIO_IRQ_ALL (GPIO_IRQ_LEVEL_HIGH|GPIO_IRQ_LEVEL_LOW|GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL)

// Define GPIO mode options

#define GPIO_SHIFT0    0
#define GPIO_SHIFT1    1
#define GPIO_SHIFT2    2
#define GPIO_SHIFT3    3
#define GPIO_SHIFT4    4
#define GPIO_SHIFT5    5
#define GPIO_SHIFT6    6
#define GPIO_SHIFT7    7
#define GPIO_SHIFT8    8
#define GPIO_SHIFT9    9
#define GPIO_SHIFT10  10
#define GPIO_SHIFT11  11
#define GPIO_SHIFT12  12
#define GPIO_SHIFT13  13
#define GPIO_SHIFT14  14
#define GPIO_SHIFT15  15
#define GPIO_SHIFT16  16
#define GPIO_SHIFT17  17
#define GPIO_SHIFT18  18
#define GPIO_SHIFT19  19
#define GPIO_SHIFT20  20
#define GPIO_SHIFT21  21
#define GPIO_SHIFT22  22
#define GPIO_SHIFT23  23
#define GPIO_SHIFT24  24
#define GPIO_SHIFT25  25
#define GPIO_SHIFT26  26
#define GPIO_SHIFT27  27
#define GPIO_SHIFT28  28
#define GPIO_MAP      31
#define GPIO_DIRECT   33
#define GPIO_IOEXPAND 34
#define GPIO_INPUT    35
#define GPIO_OUTPUT   36
#define GPIO_PIO      37
#define GPIO_SR8      38
#define GPIO_SR16     39

#if EEPROM_ENABLE || KEYPAD_ENABLE || IOEXPAND_ENABLE || (TRINAMIC_ENABLE && TRINAMIC_I2C)
#define I2C_ENABLE 1
#else
#define I2C_ENABLE 0

  //I2C_PORT 1 // SCL_PIN = 27, SDA_PIN = 26
#endif
// Define timer allocations.

/*
#define RPM_COUNTER_N               3
#define RPM_COUNTER                 timer(RPM_COUNTER_N)
#define RPM_COUNTER_IRQn            timerINT(RPM_COUNTER_N)
#define RPM_COUNTER_IRQHandler      timerHANDLER(RPM_COUNTER_N)

#define RPM_TIMER_N                 2
#define RPM_TIMER                   timer(RPM_TIMER_N)
#define RPM_TIMER_IRQn              timerINT(RPM_TIMER_N)
#define RPM_TIMER_IRQHandler        timerHANDLER(RPM_TIMER_N)

#define PPI_TIMER_N                 2
#define PPI_TIMER                   timer(PPI_TIMER_N)
#define PPI_TIMER_IRQn              timerINT(PPI_TIMER_N)
#define PPI_TIMER_IRQHandler        timerHANDLER(PPI_TIMER_N)
*/

#ifdef BOARD_CNC_BOOSTERPACK
  #include "cnc_boosterpack_map.h"
#elif defined(BOARD_PICO_CNC)
  #include "pico_cnc_map.h"
#elif defined(BOARD_MY_MACHINE)
  #include "my_machine_map.h"
#else // default board
  #include "generic_map.h"
#endif

// Adjust STEP_PULSE_LATENCY to get accurate step pulse length when required, e.g if using high step rates.
// The default value is calibrated for 10 microseconds length.
// NOTE: step output mode, number of axes and compiler optimization settings may all affect this value.
#ifndef STEP_PULSE_LATENCY
#define STEP_PULSE_LATENCY 1.0f // microseconds
#endif

// End configuration

#if EEPROM_ENABLE == 0
#define FLASH_ENABLE 1
#else
#define FLASH_ENABLE 0
#endif

#if I2C_ENABLE && !defined(I2C_PORT)
#define I2C_PORT    1
#define I2C_SDA     26
#define I2C_SCL     27
#endif

#if TRINAMIC_ENABLE 
#ifndef TRINAMIC_MIXED_DRIVERS
#define TRINAMIC_MIXED_DRIVERS 1
#endif
#include "trinamic/trinamic.h"
#endif

#if MPG_MODE_ENABLE
#define SERIAL2_MOD
#endif

// End configuration

#if KEYPAD_ENABLE && !defined(KEYPAD_STROBE_PIN)
#error Keypad plugin not supported!
#endif

#if SDCARD_ENABLE && !defined(SD_CS_PIN)
#error SD card plugin not supported!
#endif

#ifndef STEP_PINMODE
#define STEP_PINMODE PINMODE_OUTPUT
#endif

#ifndef DIRECTION_PINMODE
#define DIRECTION_PINMODE PINMODE_OUTPUT
#endif

#ifndef STEPPERS_DISABLE_PINMODE
#define STEPPERS_DISABLE_PINMODE PINMODE_OUTPUT
#endif

typedef struct {
    pin_function_t id;
    pin_group_t group;
    uint8_t pin;
    uint32_t bit;
    uint8_t port;
    bool invert;
    pin_irq_mode_t irq_mode;
    volatile bool active;
    volatile bool debounce;
    pin_mode_t cap;
    ioport_interrupt_callback_ptr interrupt_callback;
    const char *description;
} input_signal_t;

typedef struct {
    pin_function_t id;
    pin_group_t group;
    uint8_t pin;
    uint32_t bit;
    uint8_t port;
    pin_mode_t mode;
    const char *description;
} output_signal_t;

typedef struct {
    uint8_t n_pins;
    union {
        input_signal_t *inputs;
        output_signal_t *outputs;
    } pins;
} pin_group_pins_t;

bool driver_init (void);

#if OUT_SHIFT_REGISTER
void board_init (pin_group_pins_t *aux_inputs, output_sr_t *reg);
#endif

void ioports_init (pin_group_pins_t *aux_inputs, pin_group_pins_t *aux_outputs);
void ioports_event (input_signal_t *input);
void pinEnableIRQ (const input_signal_t *input, pin_irq_mode_t irq_mode);

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */

// While waiting for CMSIS headers...:

static inline void __enable_irq(void)
{
  __asm volatile ("cpsie i" : : : "memory");
}

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */
static inline  void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}


#endif // __DRIVER_H__
