/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F7 led blink sample (trace via NONE).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

// ----- LED definitions ------------------------------------------------------

// Ports numbers are: PA=0, PB=1, PC=2, PD=3, PE=4, PF=5, PG=6, PH=7, PI=8.

#if defined(STM32F746xx)

// For this to be used, define STM32F746_EVAL in the Symbols Settings tab.
#if defined(STM32F746_EVAL)

//#warning "Assume a STM32F746G-EVAL board, PF12 & PB7, active low."

#define BLINK_PORT_NUMBER_GREEN   (5)
#define BLINK_PIN_NUMBER_GREEN    (12)

#define BLINK_PORT_NUMBER_RED     (1)
#define BLINK_PIN_NUMBER_RED      (7)

#define BLINK_ACTIVE_LOW          (true)

BlinkLed blinkLeds[2] =
  {
    { BLINK_PORT_NUMBER_GREEN, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER_RED, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
  };

#else

#warning "Assume a STM32F746G-DISCO board, PI1, active high."

#define BLINK_PORT_NUMBER         (8)
#define BLINK_PIN_NUMBER_GREEN    (1)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
  };

#endif

#else

#warning "Unknown board, assume PA5, active high."

#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW },
  };

#endif

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
  // Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  Timer timer;
  timer.start ();

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].powerUp ();
    }

  uint32_t seconds = 0;

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOn ();
    }

  // First second is long.
  timer.sleep (Timer::FREQUENCY_HZ);

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOff ();
    }

  timer.sleep (BLINK_OFF_TICKS);

  ++seconds;
  trace_printf ("Second %u\n", seconds);

  if ((sizeof(blinkLeds) / sizeof(blinkLeds[0])) > 1)
    {
      // Blink individual LEDs.
      for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
        {
          blinkLeds[i].turnOn ();
          timer.sleep (BLINK_ON_TICKS);

          blinkLeds[i].turnOff ();
          timer.sleep (BLINK_OFF_TICKS);

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }

      // Blink binary.
      while (1)
        {
          for (size_t l = 0; l < (sizeof(blinkLeds) / sizeof(blinkLeds[0]));
              ++l)
            {
              blinkLeds[l].toggle ();
              if (blinkLeds[l].isOn ())
                {
                  break;
                }
            }
          timer.sleep (Timer::FREQUENCY_HZ);

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }
      // Infinite loop, never return.
    }
  else
    {
      while (1)
        {
          blinkLeds[0].turnOn ();
          timer.sleep (BLINK_ON_TICKS);

          blinkLeds[0].turnOff ();
          timer.sleep (BLINK_OFF_TICKS);

          ++seconds;
          trace_printf ("Second %u\n", seconds);
        }
      // Infinite loop, never return.
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
