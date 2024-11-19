// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  clock.h
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


#ifndef __CLOCK_H__
#define __CLOCK_H__


// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void clock_init_80mhz(int enablePA14);
void clock_init_40mhz(void);
void clock_init(uint32_t freq);
uint32_t get_bus_clock_freq(void);
void clock_delay(uint32_t cycles) __attribute__((noinline));
void msec_delay(uint32_t ms);

void sys_tick_init(uint32_t period);
void sys_tick_disable(void);
void sys_tick_reset(void);

#endif /* __CLOCK_H__ */
