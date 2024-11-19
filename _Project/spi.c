// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  spi.c
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

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "ti/devices/msp/peripherals/hw_iomux.h"
#include "ti/devices/msp/peripherals/hw_spi.h"
#include "spi.h"



// Define a structure to hold led configuration data
typedef struct
{
  uint8_t  port_id;
  uint32_t bit_mask;
  uint16_t pin_cm;
  uint8_t  mode;
} spi_struct;


// Define the configuration data for the leds on the LP-MSPM0G3507
const spi_struct lp_spi_config_data[] = {
    {LP_SPI_CLK_PORT,  LP_SPI_CLK_MASK,  LP_SPI_CLK_IOMUX,  LP_SPI_CLK_PFMODE},
    {LP_SPI_MOSI_PORT, LP_SPI_MOSI_MASK, LP_SPI_MOSI_IOMUX, LP_SPI_MOSI_PFMODE},
    {LP_SPI_MISO_PORT, LP_SPI_MISO_MASK, LP_SPI_MISO_IOMUX, LP_SPI_MISO_PFMODE},
    {LP_SPI_CS0_PORT,  LP_SPI_CS0_MASK,  LP_SPI_CS0_IOMUX,  LP_SPI_CS0_PFMODE}
};



//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the SPI1 module for a 1.25 MHz SPI clock. It
//    performs a reset on the SPI1 module, enables power, configures the
//    necessary IOMUX settings, selects the clock source, sets the clock
//    division ratio, and configures the control registers for SPI1.
//
//    The SPI1 module is configured with the following settings:
//    - Clock polarity: Low (idle state)
//    - Clock phase: First edge
//    - Data frame format: Motorola 4-wire
//    - Data size: 8 bits
//    - Chip select: CS0
//    - Clock source: System clock
//    - Clock division: 8
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
void spi1_init(void)
{

  // Reset SPI1
  SPI1->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
					    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
					    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to SPI1
  SPI1->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | 
                       GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);
  
  IOMUX->SECCFG.PINCM[LP_SPI_CLK_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CLK_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MOSI_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_MOSI_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MISO_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                     IOMUX_PINCM_INENA_ENABLE | LP_SPI_MISO_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CS0_PFMODE);

  // Select BusClk (SysClk) source for SPI module
  SPI1->CLKSEL = (SPI_CLKSEL_SYSCLK_SEL_ENABLE | SPI_CLKSEL_MFCLK_SEL_DISABLE |
                  SPI_CLKSEL_LFCLK_SEL_DISABLE);

  // Set clock division
  SPI1->CLKDIV = SPI_CLKDIV_RATIO_DIV_BY_8;

  #define PD0_CPUCLK_CLKDIV   2     // PD0 BUSCLK is half of CPUCLK
  #define PD1_CPUCLK_CLKDIV   1     // PD1 BUSCLK is same as CPUCLK

  // Both SPI modules are on PD1 
  uint32_t bus_clock = get_bus_clock_freq() / PD1_CPUCLK_CLKDIV;

  // Set clock prescaler to get final SPI clock frequency
  // SPIClk = (BusClock / (CLKDIV * (SCR+1)*2) = 40MHz/(8 * (1+1) * 2) = 1.25MHz
  if (bus_clock == 16E6)
  {
    SPI1->CLKCTL = SPI_CLKCTL_SCR_MINIMUM;
  } /* if */
  else if (bus_clock == 40E6)
  {
    SPI1->CLKCTL = 1;
  } /* else if */
  else 
  {
    SPI1->CLKCTL = 2;
  } /* else */

  // Configure SPI control register 0
  SPI1->CTL0 = (SPI_CTL0_CSCLR_DISABLE | SPI_CTL0_CSSEL_CSSEL_0 | 
                SPI_CTL0_SPH_FIRST | SPI_CTL0_SPO_LOW | 
                SPI_CTL0_PACKEN_DISABLED | SPI_CTL0_FRF_MOTOROLA_4WIRE | 
                SPI_CTL0_DSS_DSS_8);

  // Configure SPI control register 1
  SPI1->CTL1 = (SPI_CTL1_RXTIMEOUT_MINIMUM | SPI_CTL1_REPEATTX_DISABLE |
                SPI_CTL1_CDMODE_MINIMUM | SPI_CTL1_CDENABLE_DISABLE |
                SPI_CTL1_PTEN_DISABLE | SPI_CTL1_PES_DISABLE | 
                SPI_CTL1_PREN_DISABLE | SPI_CTL1_MSB_ENABLE |
                SPI_CTL1_POD_DISABLE | SPI_CTL1_CP_ENABLE | 
                SPI_CTL1_LBM_DISABLE | SPI_CTL1_ENABLE_ENABLE);

} /* spi1_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function disables the SPI1 module by clearing the enable bit. 
//    It does not change any of the configurations of the SPI module.
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
void spi1_disable(void)
{
  SPI1->CTL1 &= ~SPI_CTL1_ENABLE_MASK;
} /* spi1_disable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function writes a single character to the SPI1 data register. It 
//    waits until the transmit FIFO is not full before writing the data, 
//    ensuring thatthe SPI module is ready to accept new data. Note that 
//    this is a blocking function and can hang if the TX FIFO remains full.
//
// INPUT PARAMETERS:
//   cycles: a 8-bit to be written to the SPI1 data register.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void spi1_write_data(uint8_t data)
{
  // Wait here until TX FIFO is not full
  while((SPI1->STAT & SPI_STAT_TNF_MASK) == SPI_STAT_TNF_FULL); 
  SPI1->TXDATA = data;
} /* spi1_write_data */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads an 8-bit data value from the SPI1 data register. It 
//    waits until the receive FIFO is not empty before reading the data, 
//    ensuring that there is valid data to be read. Note that this is a 
//    blocking function and can hang if the RX FIFO remains empty.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    uint8_t - The 8-bit data value read from the SPI1 data register.
// -----------------------------------------------------------------------------
uint8_t spi1_read_data(void)
{
  // Wait here until RX FIFO is not empty
  while((SPI1->STAT & SPI_STAT_RFE_MASK) == SPI_STAT_RFE_EMPTY); 
  return (SPI1->RXDATA);
} /* spi1_read_data */



