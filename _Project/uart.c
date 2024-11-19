// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  uart.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This module provides functions to initialize and control UART0 on 
//    the MSPM0G3507 Launchpad development board. UART0 is configured  
//    for a baud rate of 115200, no parity, 8 data bits, and 1 stop bit 
//    (115200N81).
//
//    Pin Configuration:
//        - UART0 Tx: PA.10 (connected to XDS Rx)
//         - UART0 Rx: PA.11 (connected to XDS Tx)
//
//    Required Jumper Configuration:
//      - J25: Connects PA10 to XDS_UART
//      - J26: Connects PA11 to XDS_UART
//
//    Note: The code for this module is adapted from TI's sample project 
//    'uart_rw_multibyte_fifo_poll_LP_MSPM0G3507_nortos_ticlang'.
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
//    and adapt the code to your specific application and hardware requirements.
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
#include <ti/devices/msp/m0p/mspm0g350x.h>
#include "uart.h"
#include "clock.h"


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes and enables the UART0 peripheral for 
//    serial communication with a specified baud rate using 8-bit, no parity 
//    and 1 stop bit. This function also configures the GPIO pins for UART0 
//    transmission and reception. 
//
// INPUT PARAMETERS:
//   baud_rate  : 32-bit value for the desired baud rate for UART. 
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void UART_init(uint32_t baud_rate)
{
  // Configure UART0 Reset Control Register 
  UART0->GPRCM.RSTCTL = UART_RSTCTL_KEY_UNLOCK_W | 
                        UART_RSTCTL_RESETSTKYCLR_CLR | 
                        UART_RSTCTL_RESETASSERT_ASSERT;
  
  // Enable power to UART0 peripheral
  UART0->GPRCM.PWREN = UART_PWREN_KEY_UNLOCK_W | UART_PWREN_ENABLE_ENABLE;

   // time for uart to power up
   clock_delay(24);
  
  // configure PA11 and PA10 as alternate UART0 function
  IOMUX->SECCFG.PINCM[IOMUX_PINCM21]  = IOMUX_PINCM_PC_MASK | 
                                        IOMUX_PINCM21_PF_UART0_TX;
  IOMUX->SECCFG.PINCM[IOMUX_PINCM22]  = IOMUX_PINCM_INENA_ENABLE | 
                                        IOMUX_PINCM_PC_MASK | 
                                        IOMUX_PINCM22_PF_UART0_RX;

  // Configrue the clock source for UART0
  UART0->CLKSEL = UART_CLKSEL_BUSCLK_SEL_ENABLE | 
                  UART_CLKSEL_MFCLK_SEL_DISABLE | UART_CLKSEL_LFCLK_SEL_DISABLE;

  // Configrue the divide ratio of the functional clock
  UART0->CLKDIV = UART_CLKDIV_RATIO_DIV_BY_1;

  // Configure UART0 but do not enable it yet
  UART0->CTL0 = UART_CTL0_FEN_ENABLE | UART_CTL0_HSE_OVS16 | 
                UART_CTL0_CTSEN_DISABLE | UART_CTL0_CTSEN_ENABLE | 
                UART_CTL0_RTS_CLR | UART_CTL0_MODE_UART | 
                UART_CTL0_TXE_ENABLE| UART_CTL0_RXE_ENABLE | 
                UART_CTL0_LBE_DISABLE | UART_CTL0_ENABLE_DISABLE;

  #define OVERSAMPLING        16    // UART_CTL0_HSE_OVS16 set above in CTL0
  #define PD0_CPUCLK_CLKDIV   2     // UART0-2 BUSCLK is half of CPUCLK
  #define PD1_CPUCLK_CLKDIV   1     // UART3 BUSCLK is same as CPUCLK

  // Calculate integer (IBRD) and fractional (FBRD) for the desired baud 
  // rate given UART is configrued for 16x oversampling and CLKDIV=1
  uint32_t uart_clock = get_bus_clock_freq() / PD0_CPUCLK_CLKDIV;     // Really returns CPU_CLK
  uint32_t ibrd = uart_clock / (OVERSAMPLING * baud_rate);
  uint32_t remainder = uart_clock % (OVERSAMPLING * baud_rate);
  uint32_t fbrd = ((remainder * 64 / (OVERSAMPLING * baud_rate)) + 0.5);

  UART0->IBRD = ibrd;
  UART0->FBRD = fbrd;
 
  // Any changes to the baud-rate divisor must be followed by a 
  // write to the UARTLCRH register  
  UART0->LCRH = UART_LCRH_WLEN_DATABIT8 | UART_LCRH_STP2_DISABLE | 
                UART_LCRH_EPS_ODD | UART_LCRH_PEN_DISABLE | 
                UART_LCRH_BRK_DISABLE;

  // Now enable UART0
  UART0->CTL0 |= UART_CTL0_ENABLE_ENABLE;
} /* UART_init */



//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads a single character from the UART0 receiver. It waits
//    until there is data available in the receiver FIFO (RXFE is clear) and
//    then returns the received character.
//
//    Call this function to read a character from the UART0 receiver.
//    This function blocks execution until a character is available, which
//    means it will wait indefinitely if no data is received.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   The character read from the UART0 receiver
// -----------------------------------------------------------------------------
char UART_in_char(void)
{
  while((UART0->STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET);

  return((char)(UART0->RXDATA));
} /* UART_in_char */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function transmits a single character via UART0. It waits until
//    there is space available in the transmitter FIFO (TXFF is clear) and
//    then writes the character to the UART0 transmitter data register.
//
//    This function blocks execution until the transmitter FIFO has space,
//    which means it will wait indefinitely if the FIFO is full.
//
// INPUT PARAMETERS:
//   data - letter is an 8-bit ASCII character to be transferred
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void UART_out_char(char data)
{
  // wait if TxFifo full
  while((UART0->STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET);

  UART0->TXDATA = data;
} /* UART_out_char */
