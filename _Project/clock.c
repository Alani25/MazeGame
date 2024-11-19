// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  clock.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a collection of functions for initializing and 
//    configuring various hardware peripherals on the LP-MSPM0G3507 LaunchPad 
//    These functions provide support for clock initialization, delay 
//    generation, and SysTick timer setup.
//
//    This code is based the the following Texas Instruments' LaunchPad 
//    project templates for the LP-MSPM0G3507:
//      - sysctl_hfxt_run_LP_MSPM0G3507_nortos_ticlang
//      - systick_periodic_timer_LP_MSPM0G3507_nortos_ticlang
//      - sysctl_mclk_syspll_LP_MSPM0G3507_nortos_ticlang
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    This code was developed for educational purposes as part of the CSC202 
//    course and is provided "as is" without warranties of any kind, whether 
//    express, implied, or statutory.
//
//    The author and organization do not warrant the accuracy, completeness, or
//    reliability of the code. The author and organization shall not be liable
//    for any direct, indirect, incidental, special, exemplary, or consequential
//    damages arising out of the use of or inability to use the code, even if
//    advised of the possibility of such damages.
//
//    Use of this code is at your own risk, and it is recommended to validate
//    and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"


//-----------------------------------------------------------------------------
// global signal to track status of bus clock
//-----------------------------------------------------------------------------
uint32_t volatile g_bus_clock_freq = 32000000; 

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function returns current configured bus clock frequency for the 
//   Launchpad development board
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   current bus clock frequency as a uint32 value
// -----------------------------------------------------------------------------
uint32_t get_bus_clock_freq(void)
{
  return g_bus_clock_freq;
} /* get_bus_clock_freq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the system clock on for the LP-MSPM03507 
//    LaunchPad to achieve a target frequency of 40 MHz by configuring 
//    various clock sources, dividers, and control registers. 
//
//    The function sets the MCLK to use HFCLK as the source, with a UDIV of 
//    2 to achieve the desired 40 MHz clock frequency. The function also 
//    ensures that the BOR threshold is at its minimum level for reliable
//    operation.
//
//    NOTE: The function uses busy-wait loops to check the status of various
//          clock sources and configurations. These loops may cause the 
//          program to be stuck in an infinite loop if the hardware status 
//          flags are not set as expected.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void clock_init_40mhz(void)
{

  // Configure the SYSOSC configuration register to set the Target operating 
  // frequency (FREQ) for the system oscillator (SYSOSC)
  SYSCTL->SOCLOCK.SYSOSCCFG &= ~SYSCTL_SYSOSCCFG_FREQ_MASK;
  SYSCTL->SOCLOCK.SYSOSCCFG |=  SYSCTL_SYSOSCCFG_FREQ_SYSOSCBASE;
  

  // Set HFXT Range Select for the High-frequency clock (HFCLK) configuration
  SYSCTL->SOCLOCK.HFCLKCLKCFG &= ~SYSCTL_HFCLKCLKCFG_HFXTRSEL_MASK;
  SYSCTL->SOCLOCK.HFCLKCLKCFG |=  SYSCTL_HFCLKCLKCFG_HFXTRSEL_RANGE32TO48;

  // Clear and set HFXTTIME to specify the HFXT startup time in 64us resolution
  // 64us * 10 = 640 us
  SYSCTL->SOCLOCK.HFCLKCLKCFG &= ~SYSCTL_HFCLKCLKCFG_HFXTTIME_MASK;
  SYSCTL->SOCLOCK.HFCLKCLKCFG |=  0x00000000A;

  // Set HFXTEN to enable the high frequency crystal oscillator
  SYSCTL->SOCLOCK.HSCLKEN |= SYSCTL_HSCLKEN_HFXTEN_ENABLE;

  // Set HFCLKFLTCHK to enable the HFCLK startup monitor
  SYSCTL->SOCLOCK.HFCLKCLKCFG |= SYSCTL_HFCLKCLKCFG_HFCLKFLTCHK_ENABLE;

  // Wait until HSCLKGOOD is set to indict HFCLK
  // started correctly. NOTE Possible infinite loop if fails
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HFCLKGOOD_MASK) != 
          SYSCTL_CLKSTATUS_HFCLKGOOD_TRUE);

  // Configure the HSCLK source (SYSPLL or HFCLK).
  SYSCTL->SOCLOCK.HSCLKCFG = SYSCTL_HSCLKCFG_HSCLKSEL_HFCLKCLK;

  // Wait here until HSCLKGOOD is set to indict HSCLK
  // started correctly. NOTE Possible infinite loop if fails
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HSCLKGOOD_MASK) != 
          SYSCTL_CLKSTATUS_HSCLKGOOD_TRUE);

  // Configrue USEHSCLK, together with USELFCLK, to sets the MCLK source policy.
  // Set USEHSCLK to use HSCLK (HFCLK or SYSPLL) as the MCLK source in 
  // RUN and SLEEP modes.
  SYSCTL->SOCLOCK.MCLKCFG &= ~SYSCTL_MCLKCFG_USEHSCLK_MASK;
  SYSCTL->SOCLOCK.MCLKCFG |=  SYSCTL_MCLKCFG_USEHSCLK_ENABLE;

  // Wait until HSCLKMUX is set to indict MCLK is currently sourced from the 
  // highspeed clock (HSCLK). NOTE Possible infinite loop if fails
  while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != 
          SYSCTL_CLKSTATUS_HSCLKMUX_HSCLK);

  // MDIV may be used to divide the MCLK frequency when MCLK is sourced 
  // from SYSOSC.Note: MDIV=0h corresponds to /1 (no divider).
  SYSCTL->SOCLOCK.MCLKCFG &= ~SYSCTL_MCLKCFG_MDIV_MASK;

  // Configure UDIV to specify ULPCLK divider when MCLK is sourced from HSCLK. 
  // UDIV has no effect when MCLK is sourced from SYSOSC or LFCLK.
  SYSCTL->SOCLOCK.MCLKCFG &= ~SYSCTL_MCLKCFG_UDIV_MASK;
  SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_UDIV_DIVIDE2;

  // Configure FLASHWAIT for the number of flash wait states when MCLK is 
  // sourced from HSCLK. FLASHWAIT has no effect when MCLK is sourced 
  // from SYSOSC or LFCLK.
  SYSCTL->SOCLOCK.MCLKCFG &= ~SYSCTL_MCLKCFG_FLASHWAIT_MASK;
  SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_FLASHWAIT_WAIT2;

  // Configure EXCLKSRC for the source of the CLK_OUT external clock
  // output block. ULPCLK and MFPCLK require the CLK_OUT divider (EXCLKDIVEN) 
  // to be enabled
  SYSCTL->SOCLOCK.GENCLKCFG &= ~SYSCTL_GENCLKCFG_EXCLKSRC_MASK;
  SYSCTL->SOCLOCK.GENCLKCFG |=  SYSCTL_GENCLKCFG_EXCLKSRC_HFCLK;

  // Configure EXCLKDIVVAL for the divider value for the divider in the 
  // CLK_OUT external clock output block
  SYSCTL->SOCLOCK.GENCLKCFG &= ~SYSCTL_GENCLKCFG_EXCLKDIVVAL_MASK;
  SYSCTL->SOCLOCK.GENCLKCFG |= SYSCTL_GENCLKCFG_EXCLKDIVVAL_DIV4;

  // Set EXCLKDIVEN to enable the divider function of the CLK_OUT external 
  // clock output block
  SYSCTL->SOCLOCK.GENCLKCFG &= ~SYSCTL_GENCLKCFG_EXCLKDIVEN_MASK;
  SYSCTL->SOCLOCK.GENCLKCFG |= SYSCTL_GENCLKCFG_EXCLKDIVEN_ENABLE;

  // Set EXCLKEN to enable the CLK_OUT external clock output block
  SYSCTL->SOCLOCK.GENCLKEN |= SYSCTL_GENCLKEN_EXCLKEN_ENABLE;

  // Configure the BOR threshold selection.  The LEVEL specifies the desired 
  // BOR threshold and BOR mode.
  SYSCTL->SOCLOCK.BORTHRESHOLD = SYSCTL_BORTHRESHOLD_LEVEL_BORMIN;

  // Wait here until all clock status (HSCLKGOOD, LFOSCGOOD, and HFCLKGOOD)
  // are set by hardware to indict HSCLK, LFOSC, and SYSPLL are OK and ready
  // NOTE Possible infinite loop if fails
  while ((SYSCTL->SOCLOCK.CLKSTATUS & (SYSCTL_CLKSTATUS_HSCLKGOOD_MASK | 
          SYSCTL_CLKSTATUS_LFOSCGOOD_MASK | SYSCTL_CLKSTATUS_HFCLKGOOD_MASK)) !=
          (SYSCTL_CLKSTATUS_HSCLKGOOD_TRUE | SYSCTL_CLKSTATUS_LFOSCGOOD_TRUE | 
          SYSCTL_CLKSTATUS_HFCLKGOOD_TRUE));

  //SCB->SCR System control register, used to control low-power mode behavior
  // bit 2, 0 is sleep, 1 is deep sleep
  // bit 1, 0 do not sleep when returning to Thread mode, 1 enter
  // sleep/deepsleep on returning from ISR to thread
  SCB->SCR &= ~(0x04);            // sleep
  SCB->SCR &= ~(0x02);            // no sleep on exit
  
  // update the bus clock frequency
  g_bus_clock_freq = 40000000;

} /* clock_init_40mhz */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function provides a delay of approximately the specified number of 
//    cycles by using a busy-wait loop. The delay is implemented by 
//    decrementing a counter in a loop until it reaches zero.
//
//    The delay calculation accounts for instruction fetch and decode cycles as 
//    well as branch overhead. The actual delay time will depend on the clock 
//    frequency of the processor.
//
// INPUT PARAMETERS:
//   cycles: a 32-bit number of delay cycles to achieve. The effective delay  
//           is approximately (cycles - 2) due to loop overhead and  
//           instruction execution.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void clock_delay(uint32_t cycles)
{
  // The .syntax unified directive is used to switch the ARM assembler to 
  // "unified" assembly language syntax. This syntax mode combines the older 
  // ARM and Thumb assembly languages into a single, consistent syntax for all 
  // instruction types.
  #ifdef __GNUC__
      __asm(".syntax unified");
  #endif

  // There will be a 2 cycle delay here to fetch & decode instructions
  // if branch and linking to this function

  //Subtract 2 net cycles for constant offset: +2 cycles for entry jump,
  // +2 cycles for exit, -1 cycle for a shorter loop cycle on the last loop,
  // -1 for this instruction 

    __asm volatile(
          "            SUBS  R0, R0, #2; \n"
          "Clock_Loop: SUBS  R0, R0, #4; \n" // C=1 if no overflow
          "            NOP;              \n" // C=0 when R0 passes through 0
          "            BHS   Clock_Loop; \n"
                  /* Return: 2 cycles */
    );
} /* clock_delay */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function accepts a parameter specifying the number of milliseconds 
//    to delay. It calculates the appropriate number of cycles for the `clock_delay` 
//    function and uses a loop to achieve the requested delay time by calling 
//    `clock_delay` repeatedly until the delay is complete.
//
//    NOTE: The actual delay time may be slightly longer than requested due 
//          to the overhead of function calls and loop processing. The delay 
//          calculation assumes that `clock_delay` creates approximately 1 
//          microsecond of delay per cycle.
// 
// INPUT PARAMETERS:
//   ms_delay_count: a 32-bit number for the milliseconds to delay. The 
//                   function uses this value to determine the total delay
//                   duration by calling `clock_delay` in a loop.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void msec_delay(uint32_t ms_delay_count)
{
  // each call to clock_delay is count cycles
  uint32_t count = g_bus_clock_freq/1000;
  
  while(ms_delay_count)
  {
    clock_delay(count);
    ms_delay_count--;
  } /* while */

} /* clock_delay1ms */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes the SysTick timer with a specified period for periodic 
//    interrupts or delays. Configures SysTick to use the core clock, sets 
//    the reload value, and enables the timer and its interrupt. Set up 
//    the SysTick interrupt priority and ensure the timer is disabled during 
//    configuration.
//
// INPUT PARAMETERS:
//   period - a 32-bit valkue that represents the number clock cycles for
//            SysTick interrupts, period must be > 0.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void sys_tick_init(uint32_t period)
{
  // Ensure SysTick is disabled
  sys_tick_disable();

  // Clear and set priority of SysTick interrupt 
  SCB->SHP[1] &= ~(0xC0000000) | (1 << 30);

  // Reconfigure the SysTick
  SysTick->LOAD = period - 1;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | 
                  SysTick_CTRL_ENABLE_Msk;

} /* sys_tick_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function resets the SysTick timer's current value register (VAL) 
//    to 0. This is typically used to restart the SysTick timer with the 
//    current load value. This function ensures that the SysTick timer 
//    starts counting from 0 again.
//
//    This function does not modify the SysTick timer configuration or the 
//    load value. It only resets the current value register, which does not 
//    affect the timer's operation or interrupt status.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void sys_tick_reset(void)
{
  SysTick->VAL  = 0;
} /* sys_tick_reset */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function is used to disable the SysTick timer when it is no longer 
//    needed or before reconfiguring the SysTick settings. The function also 
//    clears the current value register to ensure that no pending SysTick 
//    interrupts are triggered.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void sys_tick_disable(void)
{
  SysTick->VAL  = 0;
  SysTick->LOAD = 0;
  SysTick->CTRL = 0;
} /* sys_tick_disable */