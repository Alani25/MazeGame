// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  spi.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a collection of functions for initializing and 
//    configuring SPI1 module. The module is configrued for a 1.25 MHz 
//    SPI clock and the following settings:
//    - Clock polarity: Low (idle state)
//    - Clock phase: First Edge (data changes on trailing edge)
//    - Data frame format: Motorola 4-wire
//    - Data size: 8 bits
//    - Chip select: CS0 (optional, if needed)
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


#ifndef __SPI_H__
#define __SPI_H__



#define GPIO_PORTA                                                           (0)
#define GPIO_PORTB                                                           (1)

// Defines for SPI pins on Launchpad / CSC202 Expansion board
#define MAX_NUM_SPI_PINS                                                     (4)

#define LP_SPI_CLK_PORT                                             (GPIO_PORTB)
#define LP_SPI_CLK_MASK                                                 (1 << 9)
#define LP_SPI_CLK_IOMUX                                         (IOMUX_PINCM26)
#define LP_SPI_CLK_PFMODE                                                    (3)
#define LP_SPI_MOSI_PORT                                            (GPIO_PORTB)
#define LP_SPI_MOSI_MASK                                                (1 << 8)
#define LP_SPI_MOSI_IOMUX                                        (IOMUX_PINCM25)
#define LP_SPI_MOSI_PFMODE                                                   (3)
#define LP_SPI_MISO_PORT                                            (GPIO_PORTB)
#define LP_SPI_MISO_MASK                                                (1 << 7)
#define LP_SPI_MISO_IOMUX                                        (IOMUX_PINCM24)
#define LP_SPI_MISO_PFMODE                                                   (3)
#define LP_SPI_CS0_PORT                                             (GPIO_PORTB)
#define LP_SPI_CS0_MASK                                                 (1 << 6)
#define LP_SPI_CS0_IOMUX                                         (IOMUX_PINCM23)
#define LP_SPI_CS0_PFMODE                                                    (3)


// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void spi1_init(void);
void spi1_write_data(uint8_t data);
uint8_t  spi1_read_data(void);
void spi1_disable(void);


#endif /* __SPI_H__ */
