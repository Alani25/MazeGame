//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  uart.h
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This module provides functions to initialize and control UART0 on 
//    the MSPM0G3507 Launchpad development board. UART0 is configured  
//    for a baud rate of 115200, no parity, 8 data bits, and 1 stop bit 
//    (115200N81).
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


#ifndef __UART_H__
#define __UART_H__



// --------------------------------------------------------------------------
// Prototype for Launchpad support functions
// --------------------------------------------------------------------------

void UART_init(uint32_t baud_rate);
char UART_in_char(void);
void UART_out_char(char data);



#endif /* __UART_H__ */