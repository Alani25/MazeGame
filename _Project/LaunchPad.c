// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//    DESIGNER NAME:  Bruce Link
//
//          VERSION:  0.2
//
//        FILE NAME:  LaunchPad.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//     This file contains a collection of functions for initializing and
//     configuring various hardware peripherals on the LP-MSPM0G3507 LaunchPad
//     and the CSC202 Expansion Board, including:
//       - LED control
//       - 7-segment display management
//       - 4x4 matrix keypad interfacing
//       - DIP switch reading
//       - I2C communication management
//       - PWM Motor Management
//       - DAC Management
//
//     This code is adapted from various Texas Instruments' LaunchPad
//     project template for the LP-MSPM0G3507, using C language and no RTOS.
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//     This code was developed for educational purposes as part of the CSC202
//     course and is provided "as is" without warranties of any kind, whether
//     express, implied, or statutory.
//
//     The author and organization do not warrant the accuracy, completeness, or
//     reliability of the code. The author and organization shall not be liable
//     for any direct, indirect, incidental, special, exemplary, or consequential
//     damages arising out of the use of or inability to use the code, even if
//     advised of the possibility of such damages.
//
//     Use of this code is at your own risk, and it is recommended to validate
//     and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//     You may use, edit, run or distribute this file as long as the above
//     copyright notice remains
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include <ti/devices/msp/peripherals/hw_iomux.h>
#include "LaunchPad.h"
#include "clock.h"


//-----------------------------------------------------------------------------
// Define symbolic constants used by program
//-----------------------------------------------------------------------------
#define PERIPHERIAL_PWR_UP_DELAY                       24
#define ACTIVE_LOW                                      0
#define ACTIVE_HIGH                                     1


//-----------------------------------------------------------------------------
// Define global variable and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold led configuration data
typedef struct
{
  uint8_t  port_id;
  uint32_t bit_mask;
  uint16_t pin_cm;
  uint8_t  polarity;
} gpio_struct;

// // Define a structure to hold led configuration data
typedef struct
{
  uint8_t  port_id;
  uint32_t bit_mask;
  uint16_t pin_cm;
  uint8_t  io_func;
} i2c_struct;


// Define the configuration data for the leds on the LP-MSPM0G3507
const gpio_struct lp_led_config_data[] = {
        {LP_LED_RED_PORT, LP_LED_RED_MASK, LP_LED_RED_IOMUX, ACTIVE_LOW},
        {LP_RGB_RED_PORT, LP_RGB_RED_MASK, LP_RGB_RED_IOMUX, ACTIVE_HIGH},
        {LP_RGB_GRN_PORT, LP_RGB_GRN_MASK, LP_RGB_GRN_IOMUX, ACTIVE_HIGH},
        {LP_RGB_BLU_PORT, LP_RGB_BLU_MASK, LP_RGB_BLU_IOMUX, ACTIVE_HIGH}
};

// Define the configuration data for the leds on the CSC202 Board
const gpio_struct led_config_data[] = {
        {LED0_PORT, LED0_MASK, LED0_IOMUX, ACTIVE_LOW},
        {LED1_PORT, LED1_MASK, LED1_IOMUX, ACTIVE_LOW},
        {LED2_PORT, LED2_MASK, LED2_IOMUX, ACTIVE_LOW},
        {LED3_PORT, LED3_MASK, LED3_IOMUX, ACTIVE_LOW},
        {LED4_PORT, LED4_MASK, LED4_IOMUX, ACTIVE_LOW},
        {LED5_PORT, LED5_MASK, LED5_IOMUX, ACTIVE_LOW},
        {LED6_PORT, LED6_MASK, LED6_IOMUX, ACTIVE_LOW},
        {LED7_PORT, LED7_MASK, LED7_IOMUX, ACTIVE_LOW}
};

// Define the configuration data for the enables on the CSC202 Board
const gpio_struct enable_controls[] = {
        {ENABLE_DIG0_PORT, ENABLE_DIG0_MASK, ENABLE_DIG0_IOMUX, ACTIVE_HIGH},
        {ENABLE_DIG1_PORT, ENABLE_DIG1_MASK, ENABLE_DIG1_IOMUX, ACTIVE_HIGH},
        {ENABLE_DIG2_PORT, ENABLE_DIG2_MASK, ENABLE_DIG2_IOMUX, ACTIVE_HIGH},
        {ENABLE_DIG3_PORT, ENABLE_DIG3_MASK, ENABLE_DIG3_IOMUX, ACTIVE_HIGH},
        {ENABLE_LED_PORT,  ENABLE_LED_MASK,  ENABLE_LED_IOMUX, ACTIVE_HIGH}
};

// Define the configuration data for the switches on the LaunchPad Board
const gpio_struct lp_switch_config_data[] = {
        {LP_SW1_PORT, LP_SW1_MASK, LP_SW1_IOMUX, ACTIVE_HIGH},
        {LP_SW2_PORT, LP_SW2_MASK, LP_SW2_IOMUX, ACTIVE_LOW}
};

// Define the configuration data for the switches on the CSC202 Board
const gpio_struct dip_switch_config_data[] = {
        {DIP_SW1_PORT, DIP_SW1_MASK, DIP_SW1_IOMUX, ACTIVE_LOW},
        {DIP_SW2_PORT, DIP_SW2_MASK, DIP_SW2_IOMUX, ACTIVE_LOW},
        {DIP_SW3_PORT, DIP_SW3_MASK, DIP_SW3_IOMUX, ACTIVE_LOW},
        {DIP_SW4_PORT, DIP_SW4_MASK, DIP_SW4_IOMUX, ACTIVE_LOW}
};

// Define the configuration data for the 4x4 Keypad on the CSC202 Board
const gpio_struct kp_col_config_data[] = {
        {KP_COL0_PORT, KP_COL0_MASK, KP_COL0_IOMUX, ACTIVE_HIGH},
        {KP_COL1_PORT, KP_COL1_MASK, KP_COL1_IOMUX, ACTIVE_HIGH},
        {KP_COL2_PORT, KP_COL2_MASK, KP_COL2_IOMUX, ACTIVE_HIGH},
        {KP_COL3_PORT, KP_COL3_MASK, KP_COL3_IOMUX, ACTIVE_HIGH}
};

// Define the configuration data for the 4x4 Keypad on the CSC202 Board
const gpio_struct kp_row_config_data[] = {
        {KP_ROW0_PORT, KP_ROW0_MASK, KP_ROW0_IOMUX, ACTIVE_HIGH},
        {KP_ROW1_PORT, KP_ROW1_MASK, KP_ROW1_IOMUX, ACTIVE_HIGH},
        {KP_ROW2_PORT, KP_ROW2_MASK, KP_ROW2_IOMUX, ACTIVE_HIGH},
        {KP_ROW3_PORT, KP_ROW3_MASK, KP_ROW3_IOMUX, ACTIVE_HIGH}
};

// Define the configuration data for the IIC port on the CSC202 Board
const i2c_struct iic_config_data[] = {
        {I2C_SDA_PORT, I2C_SDA_MASK, I2C_SDA_IOMUX, I2C_SDA_PINCM_IOMUX_FUNC},
        {I2C_SCL_PORT, I2C_SCL_MASK, I2C_SCL_IOMUX, I2C_SCL_PINCM_IOMUX_FUNC}
};

// ----------------------------------------------------
// key codes for the 4x4 keypad matrix
// ----------------------------------------------------
uint8_t keycodes[] = { 0x7D,    // 0
                       0xEE,    // 1
                       0xED,    // 2
                       0xEB,    // 3
                       0xDE,    // 4
                       0xDD,    // 5
                       0xDB,    // 6
                       0xBE,    // 7
                       0xBD,    // 8
                       0xBB,    // 9
                       0xE7,    // A
                       0xD7,    // B
                       0xB7,    // C
                       0x77,    // D
                       0x7E,    // E (*)
                       0x7B     // F (#)
                    };

//-----------------------------------------------------------------------------
// Define global variable and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the GPIO peripherals on the MSPM0G3507
//    LaunchPad. It performs a reset of GPIO Ports A and B by writing to
//    their reset control registers. Then, it enables power to these GPIO
//    peripherals by setting the power enable registers. Finally, it
//    introduces a small delay to ensure that the GPIO peripherals have
//    sufficient time to power up.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void launchpad_gpio_init(void)
{
  // Reset two GPIO peripherals
  GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                         GPIO_RSTCTL_RESETSTKYCLR_CLR |
                         GPIO_RSTCTL_RESETASSERT_ASSERT);

  GPIOB->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                         GPIO_RSTCTL_RESETSTKYCLR_CLR |
                         GPIO_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to two GPIO peripherals
  GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
  GPIOB->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

  // Provide a small delay for gpio to power up
  clock_delay(PERIPHERIAL_PWR_UP_DELAY);

} /* launchpad_gpio_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function configures the pins connected to the LEDs on the
//    MSPM0G3507 LaunchPad. It sets the I/O multiplexer to connect the
//    GPIO pins to the LEDs and enables the GPIO functionality for those
//    pins. Depending on the port ID specified in the configuration data,
//    it sets the corresponding data output enable register for either
//    GPIO Port A or Port B. Finally, it turns off all the LEDs by
//    calling the `lp_leds_off` function for each LED.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lp_leds_init(void)
{
  uint32_t gpio_pincm = IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;

  for (uint8_t led_idx = 0; led_idx < MAX_NUM_LP_LEDS; led_idx++)
  {
    IOMUX->SECCFG.PINCM[lp_led_config_data[led_idx].pin_cm] = gpio_pincm;

    if (lp_led_config_data[led_idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 |= lp_led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 |= lp_led_config_data[led_idx].bit_mask;
    } /* else */
  }  /* for */

  lp_leds_off(LP_RED_LED1_IDX);
  lp_leds_off(LP_RGB_RED_LED_IDX);
  lp_leds_off(LP_RGB_GRN_LED_IDX);
  lp_leds_off(LP_RGB_BLU_LED_IDX);

} /* lp_leds_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns on a specified LED on the MSPM0G3507 LaunchPad
//    based on the given index. It checks the configuration data to determine
//    the port and polarity of the LED. If the LED is configured as active
//    high, it sets the corresponding bit in the data output register to turn
//    on the LED. If the LED is configured as active low, it clears the
//    corresponding bit in the data output register.
//
// INPUT PARAMETERS:
//    index - An 8-bit index of the LED to be turned on.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lp_leds_on(uint8_t index)
{

  if (lp_led_config_data[index].port_id == GPIO_PORTA)
  {
    if (lp_led_config_data[index].polarity == ACTIVE_HIGH)
    {
      GPIOA->DOUT31_0 |= lp_led_config_data[index].bit_mask;
    } /* if */
    else
    {
      GPIOA->DOUT31_0 &= ~lp_led_config_data[index].bit_mask;
    } /* else */
  } /* if PORTA */
  else
  {
    if (lp_led_config_data[index].polarity == ACTIVE_HIGH)
    {
      GPIOB->DOUT31_0 |= lp_led_config_data[index].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOUT31_0 &= ~lp_led_config_data[index].bit_mask;
    } /* else PORT B */
  } /* else */

} /* lp_leds_on */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns off a specified LED on the MSPM0G3507 LaunchPad
//    based on the given index. It checks the configuration data to determine
//    the port and polarity of the LED. If the LED is configured as active
//    high, it clears the corresponding bit in the data output register to turn
//    off the LED. If the LED is configured as active low, it sets the
//    corresponding bit in the data output register.
//
// INPUT PARAMETERS:
//    index - An 8-bit index of the LED to be turned off.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lp_leds_off(uint8_t index)
{

  if (lp_led_config_data[index].port_id == GPIO_PORTA)
  {
    if (lp_led_config_data[index].polarity == ACTIVE_HIGH)
    {
      GPIOA->DOUT31_0 &= ~lp_led_config_data[index].bit_mask;
    } /* if */
    else
    {
      GPIOA->DOUT31_0 |= lp_led_config_data[index].bit_mask;
    } /* else */
  } /* if PORTA */
  else
  {
    if (lp_led_config_data[index].polarity == ACTIVE_HIGH)
    {
      GPIOB->DOUT31_0 &= ~lp_led_config_data[index].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOUT31_0 |= lp_led_config_data[index].bit_mask;
    } /* else PORT B */
  } /* else */

} /* lp_leds_off */


//***************************************************************************
//***************************************************************************
//******        C202 Expansion Board LED Bar functions                 ******
//***************************************************************************
//***************************************************************************


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function configures the pins connected to the LEDs on the
//    CSC202 Expansion Board. It sets the I/O multiplexer to connect the
//    GPIO pins to the LEDs and enables the GPIO functionality for those
//    pins. Depending on the port ID specified in the configuration data,
//    it sets the corresponding data output enable register for either
//    GPIO Port A or Port B. Additionally, it performs a temporary setup
//    for a 7-segment display and ensures all LEDs are turned off at the end.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void led_init(void)
{
  uint32_t gpio_pincm = IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;

  for (uint8_t led_idx = 0; led_idx < MAX_NUM_LEDS; led_idx++)
  {
    // Set IOMUX for GPIO function
    IOMUX->SECCFG.PINCM[led_config_data[led_idx].pin_cm] = gpio_pincm;

    // Enable GPOIO output port
    if (led_config_data[led_idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 |= led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 |= led_config_data[led_idx].bit_mask;
    } /* else */
  }  /* for */

  // configure the LED board enable signals
  IOMUX->SECCFG.PINCM[enable_controls[LED_BAR_ENABLE_IDX].pin_cm] = gpio_pincm;
  GPIOA->DOE31_0 |= enable_controls[LED_BAR_ENABLE_IDX].bit_mask;

  // Ensure all the LEDs are off
  leds_off();

} /* led_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function asserts the enable signal, which is active high, to the 
//    LED Bar. When the LED bar is enabled, the LED can will illuminate 
//    based on the value when to them. LED Bar will display whatever value
//    that was previously written to it.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void led_enable(void)
{
  GPIOA->DOUT31_0 |= enable_controls[LED_BAR_ENABLE_IDX].bit_mask;
} /* led_enable */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function diasserts the enable signal, which is active high, to the 
//    LED Bar. When the LED bar is diasserts, the LED can will not illuminate 
//    based on the value when to them. Any value displayed to the LED Bar 
//    remains unchanged.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void led_disable(void)
{
  GPIOA->DOUT31_0 &= ~enable_controls[LED_BAR_ENABLE_IDX].bit_mask;
} /* led_disable */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns off all the LEDs on the CSC202 Expansion Board.
//    It iterates through the LED configuration data and determines the
//    port and polarity of each LED. If the LED is configured as active
//    high, it clears the corresponding bit in the data output register
//    to turn off the LED. If the LED is configured as active low, it sets
//    the corresponding bit in the data output register.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void leds_off(void)
{

  for (uint8_t led_idx = 0; led_idx < MAX_NUM_LEDS; led_idx++)
  {
    if (led_config_data[led_idx].port_id == GPIO_PORTA)
    {
      if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
      {
        GPIOA->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
      } /* if */
      else
      {
        GPIOA->DOUT31_0 |= led_config_data[led_idx].bit_mask;
      } /* else */
    } /* if PORTA */
    else
    {
      if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
      {
        GPIOB->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
      } /* if */
      else
      {
        GPIOB->DOUT31_0 |= led_config_data[led_idx].bit_mask;
      } /* else PORT B */
    } /* else */
  } /* for */

} /* leds_off */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns on LEDs on the CSC202 Expansion Board based on the
//    given value. For each LED, it checks if the corresponding bit in the
//    value is set. If the bit is set, it turns on the LED according to its
//    port and polarity configuration. If the bit is not set, it ensures the
//    LED is turned off.
//
// INPUT PARAMETERS:
//    value - A 32-bit value where each bit corresponds to an LED index.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void leds_on(uint32_t value)
{

  for (uint8_t led_idx = 0; led_idx < MAX_NUM_LEDS; led_idx++)
  {
    if ((value & (1 << led_idx)) == (1 << led_idx))
    {
      if (led_config_data[led_idx].port_id == GPIO_PORTA)
      {
        if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
        {
          GPIOA->DOUT31_0 |= led_config_data[led_idx].bit_mask;
        } /* if */
        else
        {
          GPIOA->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
        } /* else */
      } /* if */
      else
      {
        if (led_config_data[led_idx].port_id == GPIO_PORTA)
        {
          GPIOB->DOUT31_0 |= led_config_data[led_idx].bit_mask;
        } /* if */
        else
        {
          GPIOB->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
        } /* else */
      } /* else */
    } /* if */
    else
    {
      if (led_config_data[led_idx].port_id == GPIO_PORTA)
      {
        if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
        {
          GPIOA->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
        } /* if */
        else
        {
          GPIOA->DOUT31_0 |= led_config_data[led_idx].bit_mask;
        } /* else */
      } /* if */
      else
      {
        if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
        {
          GPIOB->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
        } /* if */
        else
        {
          GPIOB->DOUT31_0 |= led_config_data[led_idx].bit_mask;
        } /* else */
      } /* else */
    } /* else */
  } /* for */

} /* leds_on */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns on a specified LED on the CSC202 Expansion Board
//    based on the given index. It checks the configuration data to determine
//    the port and polarity of the LED. If the LED is configured as active
//    high, it sets the corresponding bit in the data output register to turn
//    on the LED. If the LED is configured as active low, it clears the
//    corresponding bit in the data output register.
//
//     This function does not perform error checking to  verify that the
//     led_idx parameter is valid entry into the led_config_data array.
//
// INPUT PARAMETERS:
//    led_idx - a 8-bit index value of the LED to turn on
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void led_on(uint8_t led_idx)
{

  if (led_config_data[led_idx].port_id == GPIO_PORTA)
  {
    if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
    {
      GPIOA->DOUT31_0 |= led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOA->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
    } /* else */
  } /* if */
  else
  {
    if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
    {
      GPIOB->DOUT31_0 |= led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
    } /* else */
  } /* else */

} /* led_on */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns off a specified LED on the CSC202 Expansion Board
//    based on the given index. It checks the configuration data to determine
//    the port and polarity of the LED. If the LED is configured as active
//    high, it clears the corresponding bit in the data output register to turn
//    off the LED. If the LED is configured as active low, it sets the
//    corresponding bit in the data output register.
//
//    This function does not perform error checking to verify that the
//    led_idx parameter is a valid entry in the led_config_data array.
//
// INPUT PARAMETERS:
//    led_idx - an 8-bit index value of the LED to turn off
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void led_off(uint8_t led_idx)
{

  if (led_config_data[led_idx].port_id == GPIO_PORTA)
  {
    if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
    {
      GPIOA->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOA->DOUT31_0 |= led_config_data[led_idx].bit_mask;
    } /* else */
  } /* if */
  else
  {
    if (led_config_data[led_idx].polarity == ACTIVE_HIGH)
    {
      GPIOB->DOUT31_0 &= ~led_config_data[led_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOUT31_0 |= led_config_data[led_idx].bit_mask;
    } /* else */
  } /* else */

} /* led_off */


//***************************************************************************
//***************************************************************************
//******              Seven Segment Display functions                  ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the GPIO pins for both the LEDs and the
//    7-segment display on the CSC202 Expansion Board. It configures the I/O
//    multiplexer to connect the GPIO pins for the LEDs and the 7-segment
//    display, enabling the GPIO functionality for those pins. The function
//    then turns off all LEDs and the 7-segment display by calling the
//    `leds_off` and `seg7_off` functions, respectively.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_init(void)
{
  uint32_t gpio_pincm = IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;

  for (uint8_t idx = 0; idx < MAX_NUM_LEDS; idx++)
  {
    IOMUX->SECCFG.PINCM[led_config_data[idx].pin_cm] = gpio_pincm;

    if (led_config_data[idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 |= led_config_data[idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 |= led_config_data[idx].bit_mask;
    } /* else */

  }  /* for */

  leds_off();

  for (uint8_t seg7_idx = 0; seg7_idx < 4; seg7_idx++)
  {
    IOMUX->SECCFG.PINCM[enable_controls[seg7_idx].pin_cm] = gpio_pincm;

    if (enable_controls[seg7_idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 |= enable_controls[seg7_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 |= enable_controls[seg7_idx].bit_mask;
    } /* else */
  }  /* for */

  seg7_off();

} /* seg7_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables a specified 7-segment display digit on the
//    CSC202 Expansion Board based on the given index. It checks the
//    configuration data to determine the port for the 7-segment display
//    digit. The function sets the corresponding bit in the data output
//    register to enable the digit.
//
// INPUT PARAMETERS:
//    seg7_idx - A 8-bit index of the 7-segment display digit to be enabled.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_dig_enable(uint8_t seg7_idx)
{
  if (enable_controls[seg7_idx].port_id == GPIO_PORTA)
  {
    GPIOA->DOUT31_0 |= enable_controls[seg7_idx].bit_mask;
  } /* if */
  else
  {
    GPIOB->DOUT31_0 |= enable_controls[seg7_idx].bit_mask;
  } /* else */

} /* seg7_dig_enable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function disables all LEDs and 7-segment display digits on the
//    CSC202 Expansion Board. It configures the I/O multiplexer to disconnect
//    the GPIO pins for the LEDs and the 7-segment display. The function clears
//    the corresponding bits in the data output enable register to disable
//    the LEDs and 7-segment display digits.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_disable(void)
{
  for (uint8_t idx = 0; idx < MAX_NUM_LEDS; idx++)
  {
    IOMUX->SECCFG.PINCM[led_config_data[idx].pin_cm] = IOMUX_PINCM_PF_OFS;

    if (led_config_data[idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 &= ~led_config_data[idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 &= ~led_config_data[idx].bit_mask;
    } /* else */

  }  /* for */

  leds_off();

  for (uint8_t seg7_idx = 0; seg7_idx < 4; seg7_idx++)
  {
    IOMUX->SECCFG.PINCM[enable_controls[seg7_idx].pin_cm] = IOMUX_PINCM_PF_OFS;

    if (enable_controls[seg7_idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOE31_0 &= ~enable_controls[seg7_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOE31_0 &= ~enable_controls[seg7_idx].bit_mask;
    } /* else */
  }  /* for */

} /* seg7_disable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns off all digits of the 7-segment display on the
//    CSC202 Expansion Board. It iterates over the configuration data for
//    the 7-segment display digits and clears the corresponding bits in
//    the data output registers for GPIO Port A or Port B, depending on
//    the digit's configuration.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_off(void)
{
  for (uint8_t seg7_idx = 0; seg7_idx < 4; seg7_idx++)
  {
    if (enable_controls[seg7_idx].port_id == GPIO_PORTA)
    {
      GPIOA->DOUT31_0 &= ~enable_controls[seg7_idx].bit_mask;
    } /* if */
    else
    {
      GPIOB->DOUT31_0 &= ~enable_controls[seg7_idx].bit_mask;
    } /* else */
  }
} /* seg7_off */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function updates the LEDs based on the provided value and enables
//    the specified digit of the 4-digit 7-segment display on the CSC202
//    Expansion Board. It first sets the state of the LEDs according to the
//    `value` parameter, then turns off all digits of the 7-segment display to
//    ensure only the requested digit (specified by `seg7_idx`) is enabled.
//
// INPUT PARAMETERS:
//    value    - An 8-bit value where each bit corresponds to an individual
//               LED or segments of the 7-segment display.
//
//    seg7_idx - An 8-bit index of the 7-segment display digit to be enabled.
//               This value should be within the range [0, 3], corresponding to
//               the four digits of the 7-segment display.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_on(uint8_t value, uint8_t seg7_idx)
{
  leds_on(value);

  seg7_off();

  seg7_dig_enable(seg7_idx);

} /* seg7_on */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays a hexadecimal digit on a specific digit of the
//    4-digit 7-segment display on the CSC202 Expansion Board. It uses a lookup
//    table of 7-segment display patterns to convert a hexadecimal index into
//    the appropriate 7-segment display pattern and enables the specified digit.
//
// INPUT PARAMETERS:
//    hex_idx  - An 8-bit index of the hexadecimal digit to be displayed. This
//               value should be within the range [0, 15], corresponding to
//               the hexadecimal digits 0-9 and A-F.
//    seg7_dig - An 8-bit index of the 7-segment display digit to be used for
//               display. This value should be within the range [0, 3],
//               corresponding to the four digits of the 7-segment display.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void seg7_hex(uint8_t hex_idx, uint8_t seg7_dig)
{
  const char hex_digits[] =
  {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
  };

  seg7_on(hex_digits[hex_idx], seg7_dig);

} /* seg7_hex */


//*****************************************************************************
//*****************************************************************************
//******                   Switch Reading functions                      ******
//*****************************************************************************
//*****************************************************************************


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DIP switches on the CSC202 Expansion Board
//    by configuring the GPIO pins connected to the switches. It enables the
//    input mode for the switches, connects the pins to the GPIO functionality,
//    and applies an inversion mask to read the switch states accurately.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lpsw_init(void)
{
  // // SW1 on LaunchPad is active high -- CONFLICTS WITH LED_EN
  // IOMUX->SECCFG.PINCM[lp_switch_config_data[LP_SW1_IDX].pin_cm] = 
  //                    IOMUX_PINCM_INENA_ENABLE| IOMUX_PINCM_PC_CONNECTED |
  //                    PINCM_GPIO_PIN_FUNC;

  // SW2 on LaunchPad is active low and requires pull-up
  IOMUX->SECCFG.PINCM[lp_switch_config_data[LP_SW2_IDX].pin_cm] = 
                     IOMUX_PINCM_INENA_ENABLE| IOMUX_PINCM_INV_ENABLE |
                     IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_PIPU_ENABLE | 
                     PINCM_GPIO_PIN_FUNC;

} /* lpsw_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the switchs (pushbuttons) on the 
//    LaunchPad Board and returns TRUE if the button is down.
//
// INPUT PARAMETERS:
//    sw_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is up
//    1 if pushbutton is down
// -----------------------------------------------------------------------------
bool is_lpsw_down(uint8_t sw_idx)
{
  bool sw_status = false;
  uint8_t sw_value = 0;

  if (lp_switch_config_data[sw_idx].port_id == GPIO_PORTA)
  {
    sw_value = ((GPIOA->DIN31_0 & lp_switch_config_data[sw_idx].bit_mask) ==
                  lp_switch_config_data[sw_idx].bit_mask);
  } /* if */
  else
  {
    sw_value = ((GPIOB->DIN31_0 & lp_switch_config_data[sw_idx].bit_mask) ==
                  lp_switch_config_data[sw_idx].bit_mask);
  } /* else */

  // Assumes 1 is returned when switch pressed - regardless of switch poslarity
  if (sw_value == 0x01)
  {
    sw_status = true;
  } /* if */

  return (sw_status);

} /* is_lpsw_down */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the switchs (pushbuttons) on the 
//    LaunchPad Board and returns TRUE if the button is up.
//
// INPUT PARAMETERS:
//    sw_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is down
//    1 if pushbutton is up
// -----------------------------------------------------------------------------
bool is_lpsw_up(uint8_t sw_idx)
{
  bool sw_status = false;
  uint8_t sw_value = 0;

  if (lp_switch_config_data[sw_idx].port_id == GPIO_PORTA)
  {
    sw_value = ((GPIOA->DIN31_0 & lp_switch_config_data[sw_idx].bit_mask) ==
                lp_switch_config_data[sw_idx].bit_mask);
  } /* if */
  else
  {
    sw_value = ((GPIOB->DIN31_0 & lp_switch_config_data[sw_idx].bit_mask) ==
                  lp_switch_config_data[sw_idx].bit_mask);
  } /* else */

  // Assumes 1 is returned when switch pressed - regardless of switch poslarity
  if (sw_value != 0x1)
  {
    sw_status = true;
  } /* if */

  return (sw_status);

} /* is_lpsw_up */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DIP switches on the CSC202 Expansion Board
//    by configuring the GPIO pins connected to the switches. It enables the
//    input mode for the switches, connects the pins to the GPIO functionality,
//    and applies an inversion mask to read the switch states accurately.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void dipsw_init(void)
{
  uint32_t gpio_pincm = IOMUX_PINCM_INENA_ENABLE| IOMUX_PINCM_PC_CONNECTED |
                        PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INV_MASK;

  for (uint8_t sw_idx = 0; sw_idx < MAX_NUM_DIPSW; sw_idx++)
  {
    IOMUX->SECCFG.PINCM[dip_switch_config_data[sw_idx].pin_cm] = gpio_pincm;
  }  /* for */

} /* dipsw_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the DIP switches on the CSC202
//    Expansion Board. It checks the value of each switch and assembles these
//    values into an 8-bit integer, where each bit represents the state of one
//    DIP switch. The state of each switch is determined by reading the GPIO
//    data register and checking against the configured bit mask for each 
//    switch.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    uint8_t - An 8-bit value where each bit represents the state of a DIP 
//              switch. A bit is set (1) if the corresponding switch is in 
//              the ON position, and cleared (0) if the switch is in the OFF 
//               position.
// -----------------------------------------------------------------------------
uint8_t dipsw_read(void)
{
  uint8_t switch_value = 0;
  uint8_t dip_value = 0;

  for (uint8_t sw_idx = 0; sw_idx < MAX_NUM_DIPSW; sw_idx++)
  {
    if (dip_switch_config_data[sw_idx].port_id == GPIO_PORTA)
    {
      switch_value = ((GPIOA->DIN31_0 & dip_switch_config_data[sw_idx].bit_mask) ==
                       dip_switch_config_data[sw_idx].bit_mask);
      dip_value |= switch_value << (MAX_NUM_DIPSW-1-sw_idx);
    } /* if */
    else
    {
      switch_value = ((GPIOB->DIN31_0 & dip_switch_config_data[sw_idx].bit_mask) ==
                       dip_switch_config_data[sw_idx].bit_mask);
      dip_value |= switch_value << (MAX_NUM_DIPSW-1-sw_idx);
    } /* else */

  }  /* for */

  return (dip_value);

} /* dipsw_read */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the pushbuttons on the CSC202
//    Expansion Board and returns TRUE if the button is down.
//
// INPUT PARAMETERS:
//    pb_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is up
//    1 if pushbutton is down
// -----------------------------------------------------------------------------
bool is_pb_down(uint8_t pb_idx)
{
  bool pb_status = false;
  uint8_t pb_value = 0;

  if (dip_switch_config_data[pb_idx].port_id == GPIO_PORTA)
  {
    pb_value = ((GPIOA->DIN31_0 & dip_switch_config_data[pb_idx].bit_mask) ==
                  dip_switch_config_data[pb_idx].bit_mask);
  } /* if */
  else
  {
    pb_value = ((GPIOB->DIN31_0 & dip_switch_config_data[pb_idx].bit_mask) ==
                  dip_switch_config_data[pb_idx].bit_mask);
  } /* else */

  // IOMUX has inversion enabled so used !polarity
  if (pb_value != dip_switch_config_data[pb_idx].polarity)
  {
    pb_status = true;
  }

  return (pb_status);

} /* is_pb_down */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the DIP switches on the CSC202
//    Expansion Board and returns TRUE if the button is up.
//
// INPUT PARAMETERS:
//    pb_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is down
//    1 if pushbutton is up
// -----------------------------------------------------------------------------
bool is_pb_up(uint8_t pb_idx)
{
  bool pb_status = false;
  uint8_t pb_value = 0;

  if (dip_switch_config_data[pb_idx].port_id == GPIO_PORTA)
  {
    pb_value = ((GPIOA->DIN31_0 & dip_switch_config_data[pb_idx].bit_mask) ==
                dip_switch_config_data[pb_idx].bit_mask);
  } /* if */
  else
  {
    pb_value = ((GPIOB->DIN31_0 & dip_switch_config_data[pb_idx].bit_mask) ==
                  dip_switch_config_data[pb_idx].bit_mask);
  } /* else */

  // IOMUX has inversion enabled so used polarity
  if (pb_value == dip_switch_config_data[pb_idx].polarity)
  {
    pb_status = true;
  }

  return (pb_status);

} /* is_pb_up */

//***************************************************************************
//***************************************************************************
//******              4x4 Keypad Managment Functions                    ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the keypad matrix on the CSC202 Expansion
//    Board. It configures the rows of the keypad as inputs with internal
//    pull-ups and the columns as outputs. This setup allows the detection
//    of key presses in the keypad matrix by reading the state of the rows
//    and setting the state of the columns.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void keypad_init(void)
{
  uint32_t gpio_kp_rows = (IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC |
                           IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PIPU_ENABLE);

  uint32_t gpio_kp_cols =  IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;

  for (uint8_t index = 0; index < MAX_NUM_KP_ROWS; index++)
  {
    IOMUX->SECCFG.PINCM[kp_row_config_data[index].pin_cm] = gpio_kp_rows;
  }  /* for */

  for (uint8_t index = 0; index < MAX_NUM_KP_COLS; index++)
  {
    IOMUX->SECCFG.PINCM[kp_col_config_data[index].pin_cm] = gpio_kp_cols;
    if (kp_col_config_data[index].port_id == GPIO_PORTA)
    {
        GPIOA->DOE31_0 |= kp_col_config_data[index].bit_mask;
    } /* if */
    else
    {
        GPIOB->DOE31_0 |= kp_col_config_data[index].bit_mask;
    } /* for */

  }  /* for */

} /* keypad_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the state of all the rows in the keypad matrix on
//    the CSC202 Expansion Board. It returns an 8-bit value where each bit
//    represents the state of a corresponding keypad row. A bit value of 1
//    indicates that the row is active (low), while a bit value of 0
//    indicates that the row is inactive (high). 
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    uint8_t - An 8-bit value where each bit represents the state of a keypad
//              row. The least significant bit corresponds to the first row,
//              and the most significant bit corresponds to the last row. A bit
//              value of 1 means the row is active, and a bit value of 0 means
//              the row is inactive.
// -----------------------------------------------------------------------------
uint8_t read_keyrow_data(void)
{
  uint8_t data = 0x00;
  uint8_t bit  = 0x00;

  for (uint8_t row_num = 0; row_num < MAX_NUM_KP_ROWS; row_num++)
  {
    if (kp_row_config_data[row_num].port_id == GPIO_PORTA)
    {
      bit  = (GPIOA->DIN31_0 & kp_row_config_data[row_num].bit_mask) == kp_row_config_data[row_num].bit_mask;
      data |= (bit << row_num);
    } /* if */
    else
    {
      bit  = (GPIOB->DIN31_0 & kp_row_config_data[row_num].bit_mask) == kp_row_config_data[row_num].bit_mask;
      data |= (bit << row_num);
    } /* else */
  } /* for */

  return (data);
} /* read_keyrow_data */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function writes a value to the columns of the keypad matrix on
//    the CSC202 Expansion Board. It sets the state of each column based
//    on the provided `data` byte, where each bit of the byte determines
//    whether the corresponding column is driven high or low. This is used
//    to scan the keypad matrix and detect which keys are pressed.
//
// INPUT PARAMETERS:
//    data - A byte value where each bit corresponds to the state of a
//           column in the keypad matrix. A bit value of 1 drives the
//           corresponding column high, and a bit value of 0 drives it low.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void write_keycol_data(uint8_t data)
{
  uint8_t mask = 0x01;
  uint8_t bit  = 0x00;

  for (uint8_t col_num = 0; col_num < MAX_NUM_KP_COLS; col_num++)
  {
    bit = (data & mask) >> col_num;
    mask <<= 1;
    if (kp_col_config_data[col_num].port_id == GPIO_PORTA)
    {
      GPIOA->DOUT31_0 &= ~kp_col_config_data[col_num].bit_mask;
      if (bit)
      {
        GPIOA->DOUT31_0 |= kp_col_config_data[col_num].bit_mask;
      } /* IF */
    } /* if */
    else
    {
      GPIOB->DOUT31_0 &= ~kp_col_config_data[col_num].bit_mask;
      if (bit)
      {
        GPIOB->DOUT31_0 |= kp_col_config_data[col_num].bit_mask;
      } /* IF */
    } /* else */
  } /* for */
} /* write_keycol_data */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function scans the keypad matrix on the CSC202 Expansion Board to
//    detect which key is currently pressed. It sequentially drives each column
//    and reads the state of the rows to determine if a keypress corresponds to
//    any of the defined keycodes. It returns the index of the pressed key, or
//    if no key is pressed, it returns the index of the next key to be checked.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    uint8_t - The index of the key that is pressed, which corresponds to the
//             key's position in the `keycodes` array. If no key is pressed, it
//             returns the index of the next key to be checked (in the range
//             0 to 15). If no key is detected, the return value will be 0x10,
//             indicating that no key is pressed.
// -----------------------------------------------------------------------------
uint8_t keypad_scan(void)
{
  uint8_t found = false;
  uint8_t data  = 0x00;
  uint8_t key   = 0;

  for (key = 0; ((key < 0x10) && !found); )
  {
      write_keycol_data(keycodes[key]);
      data = read_keyrow_data();

      // extract upper nibble and shift to lower mibble before comparing 
      if (data == (keycodes[key] & 0xF0) >> 4)
      {
          found = true;
      } /* if */
      else
      {
          key++;
      } /* else */
  } /* for */

  return (key);
} /* keypad_scan */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function wait here for a key to be pressed on the keypad matrix. 
//    The function only returns when a valid key is pressed.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    uint8_t - The index of the key that is pressed, which corresponds to the
//             key's position in the `keycodes` array. 
// -----------------------------------------------------------------------------
uint8_t getkey_pressed(void)
{
  uint8_t key   = 0;

  do 
  {
    key = keypad_scan();
  } while (key == 0x10);

  return (key);
} /* getkey_pressed */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function wait here until no key on the keypad matrix is pressed.
//    The function only returns when a no valid key is pressed.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void wait_no_key_pressed(void)
{
  uint8_t key;

  do 
  {
    key = keypad_scan();
  } while (key != 0x10);

} /* wait_no_key_pressed */


//***************************************************************************
//***************************************************************************
//******                    IIC Managment functions                    ******
//***************************************************************************
//***************************************************************************


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the I2C peripheral for communication with a
//    clock speed of 400kHz. It assumes GPIOA and GPIOB are reset and powered
//    on previously. The function configures the I2C module clock, sets up the
//    I2C bus timing, and enables the I2C peripheral for master mode operation.
//    It also configures the GPIO pins for I2C functionality and sets up the
//    required clock stretching and bus frequency settings. The I2C clock speed
//    is determined based on the SYSCLK frequency, which can vary depending on
//    the bus clock configuration (32MHz, 40MHz, or 80MHz).
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void I2C_init(void)
{
  uint32_t config_data = 0;

  // Resets I2C peripheral
  I2C_INST->GPRCM.RSTCTL = (I2C_RSTCTL_KEY_UNLOCK_W |
                  I2C_RSTCTL_RESETSTKYCLR_CLR | I2C_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to I2C peripheral
  I2C_INST->GPRCM.PWREN = (I2C_PWREN_KEY_UNLOCK_W | I2C_PWREN_ENABLE_ENABLE);

  // Configure GPIO Ports as alternate IC2 function
  for (uint8_t idx = 0; idx < MAX_NUM_I2C_BITS; idx++)
  {
    config_data = (IOMUX_PINCM_HIZ1_ENABLE | IOMUX_PINCM_INENA_ENABLE |
                   IOMUX_PINCM_PC_MASK | iic_config_data[idx].io_func);
    IOMUX->SECCFG.PINCM[iic_config_data[idx].pin_cm] = config_data;

  } /* for */

  // time for I2C to power up
  clock_delay(PERIPHERIAL_PWR_UP_DELAY);

  // Selects BUSCLK as clock source for IIC
  I2C_INST->CLKSEL = I2C_CLKSEL_BUSCLK_SEL_MASK;

  // Selects divide ratio of module clock
  I2C_INST->CLKDIV = I2C_CLKDIV_RATIO_DIV_BY_1;

  I2C_INST->MASTER.MCTR = 0x00;

  // Set the timer period for the SCL clock to 400kHz
  // period=clockPeriod*(1+MTPR)*10
  // bus clock=32MHz, make MTPR=7, frequency = 32MHz/80 = 400kHz
  // bus clock=40MHz, make MTPR=4, frequency = 20MHz/50 = 400kHz
  // bus clock=80MHz, make MTPR=9, frequency = 40MHz/100 = 400kHz
  I2C_INST->MASTER.MTPR = 9;

  // Setup IIC configuration options
  I2C_INST->MASTER.MCR = I2C_MCR_CLKSTRETCH_MASK;

  // Disable using interrupts, FIFO triggers not used
  I2C_INST->MASTER.MFIFOCTL = 0;

  // Configuration done, enable IIC
  I2C_INST->MASTER.MCR |= I2C_MCR_ACTIVE_ENABLE;
} /* I2C_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function fills the I2C transmit FIFO buffer with data from the
//    provided buffer. It writes a specified number of bytes to the FIFO and
//    checks if the FIFO is full during the process. If the FIFO is full,
//    the function returns a failure status; otherwise, it returns success.
//
// INPUT PARAMETERS:
//    buffer - Pointer to the data buffer that holds the bytes to be transmitted.
//    count  - The number of bytes to be written to the I2C transmit FIFO.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    1 if all bytes are written to the FIFO without issue,
//    0 if the FIFO was full before all bytes could be written.
// -----------------------------------------------------------------------------
uint32_t static I2C_fill_tx_fifo(uint8_t *buffer, uint16_t count)
{
  uint32_t ret_status = 1;

  for(uint8_t i = 0; i < count; i++)
  {
    if ((I2C1->MASTER.MFIFOSR & I2C_MFIFOSR_TXFIFOCNT_MASK) == 0)
    {
      // fail TxFifo full
      ret_status = 0;
    } /* if */

    I2C1->MASTER.MTXDATA = buffer[i];
  } /* for */

  return (ret_status);

} /*I2C_fill_tx_fifo */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function sends a single byte of data to a specified I2C slave device.
//    It sets up the I2C communication by filling the transmit FIFO with the data
//    byte, configures the I2C controller for the transmission, and waits for the
//    transmission to complete. The function also checks for errors during the
//    process and returns a status indicating success or failure.
//
//    The function performs the following operations:
//    - Fills the I2C transmit FIFO with one byte of data.
//    - Waits until the I2C controller is idle.
//    - Configures the I2C controller to send data to the specified slave device.
//    - Starts the I2C transmission with the appropriate control settings.
//    - Waits for the I2C transmission to complete and checks for errors.
//
// INPUT PARAMETERS:
//    slave  - The 7-bit address of the I2C slave device to which data is sent.
//             The address is shifted left by 1 to fit the I2C address format.
//
//    data1  - The single byte of data to be transmitted to the slave device.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    1 if the transmission was successful without errors,
//    0 if there was an error during the transmission process.
// -----------------------------------------------------------------------------
uint32_t I2C_send1(uint8_t slave, uint8_t data1)
{
  uint32_t I2C_error;
  uint32_t ret_status = 1;

  if(I2C_fill_tx_fifo(&data1, 1) == 0)
    return 0;

  // wait until I2C controller idle (IDLE bit = 1)
  while((I2C1->MASTER.MSR & I2C_MSR_IDLE_MASK) == I2C_MSR_IDLE_CLEARED);

  // bit 7-1 address
  // bit 0 direction=0 transmit
  I2C1->MASTER.MSA = (slave << 1);

  // bits 27-16 MBLEN =1 (length)
  // bit 3 ACK, bit 2 STOP=1, bit 1 START=1, bit 0 BURSTRUN=1
  //I2C1->MASTER.MCTR = 0x00010007;
  I2C1->MASTER.MCTR = (0x00010000 |  I2C_MCTR_ACK_ENABLE |
                       I2C_MCTR_STOP_ENABLE | I2C_MCTR_START_ENABLE |
                       I2C_MCTR_BURSTRUN_ENABLE);

  // wait until not I2C controller FSM is not busy
  while((I2C1->MASTER.MSR & I2C_MSR_BUSY_MASK) == I2C_MSR_BUSY_SET);

  // check for error or if lost arbitration or no ack
  if (I2C1->MASTER.MSR & (I2C_MSR_ARBLST_SET | I2C_MSR_ERR_SET))
  {
	  // help debugging
    I2C_error = I2C1->MASTER.MSR;
    ret_status = 0;
  } /* if */

  // wait until I2C controller idle (IDLE bit = 1)
  while((I2C1->MASTER.MSR & I2C_MSR_IDLE_MASK) == I2C_MSR_IDLE_CLEARED);

  return (ret_status);
} /* I2C_send1 */




//***************************************************************************
//***************************************************************************
//******               Motor PWM Managment functions                   ******
//***************************************************************************
//***************************************************************************


//-----------------------------------------------------------------------------
// DESCRIPTION:
//		This function configures the IOMUX to drive a motor using PWM. The motor 
//		control is based on the assumption that one side of the L293D IC is used. 
//		This function repurposes LED0 for TIMA0 output 3, which is connected to 
//		the ENable pin of the L293D. LED1 and LED2 are configured as general-purpose 
//		outputs to control the direction of the motor, and these pins are connected 
//		to the IN pins of the L293D.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// -----------------------------------------------------------------------------
void motor0_init(void)
{
  // Set PA28 (LD0) for TIMA0_C3
  IOMUX->SECCFG.PINCM[LED0_IOMUX] = IOMUX_PINCM3_PF_TIMA0_CCP3 | 
                                    IOMUX_PINCM_PC_CONNECTED;
  GPIOA->DOESET31_0 = LED0_MASK;

  // Set PA31 (LD1) for output
  IOMUX->SECCFG.PINCM[LED1_IOMUX] = PINCM_GPIO_PIN_FUNC | 
                                    IOMUX_PINCM_PC_CONNECTED;
  GPIOA->DOESET31_0 = LED1_MASK;

  // Set PB20 (LD2) for output
  IOMUX->SECCFG.PINCM[LED2_IOMUX] = PINCM_GPIO_PIN_FUNC | 
                                    IOMUX_PINCM_PC_CONNECTED;
  GPIOA->DOESET31_0 = LED2_MASK;

} /* motor0_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//		This function configures Timer A0 as an up counter to generate a 
//		PWM signal. The timer is set to operate at 200 kHz without generating 
//		interrupts, continuously restarting when it reaches the specified 
//		terminal value (load).
//
// INPUT PARAMETERS:
//		uint32_t load_value - The terminal count value at which the timer resets.
//		uint32_t compare_value - The value at which the timer compares and 
//		                         toggles the PWM output.
//
// OUTPUT PARAMETERS:
//		none
//
// RETURN:
//		none
// -----------------------------------------------------------------------------
void motor0_pwm_init(uint32_t load_value, uint32_t compare_value)
{
  // Reset TIMA0
  TIMA0->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
                           GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
                           GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to TIMA0
  TIMA0->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | 
                          GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);

  TIMA0->CLKSEL = (GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE | 
                   GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |  
                   GPTIMER_CLKSEL_LFCLK_SEL_DISABLE);

  TIMA0->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;

  // set the pre-scale count value that divides select clock by PCNT+1
  // TimerClock = BusCock / (DIVIDER * (PRESCALER))
  // 200,000 Hz = 40,000,000 Hz / (8 * (24 + 1))
  TIMA0->COMMONREGS.CPS = GPTIMER_CPS_PCNT_MASK & 0x18;

  // Set action for compare
  // On Zero, set output HIGH; On Compares up, set output LOW
  TIMA0->COUNTERREGS.CCACT_23[1] = (GPTIMER_CCACT_23_FENACT_DISABLED | 
        GPTIMER_CCACT_23_CC2UACT_DISABLED | GPTIMER_CCACT_23_CC2DACT_DISABLED |
        GPTIMER_CCACT_23_CUACT_CCP_LOW | GPTIMER_CCACT_23_CDACT_DISABLED | 
        GPTIMER_CCACT_23_LACT_DISABLED | GPTIMER_CCACT_23_ZACT_CCP_HIGH);

  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & (load_value - 1);

  // set timer compare value
  TIMA0->COUNTERREGS.CC_23[1] = GPTIMER_CC_23_CCVAL_MASK & compare_value;

  // set compare control for PWM func with output initially low
  TIMA0->COUNTERREGS.OCTL_23[1] = (GPTIMER_OCTL_23_CCPIV_LOW | 
                GPTIMER_OCTL_23_CCPOINV_NOINV | GPTIMER_OCTL_23_CCPO_FUNCVAL);
  //
  TIMA0->COUNTERREGS.CCCTL_23[1] = GPTIMER_CCCTL_23_CCUPD_IMMEDIATELY;


  // When enabled load 0, set timer to count up
  TIMA0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CVAE_ZEROVAL | 
                 GPTIMER_CTRCTL_REPEAT_REPEAT_1 | GPTIMER_CTRCTL_CM_UP;

  TIMA0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

  // No interrupt is required
  TIMA0->CPU_INT.IMASK = GPTIMER_CPU_INT_IMASK_L_CLR;

  // set C0 as output
  TIMA0->COMMONREGS.CCPD =(GPTIMER_CCPD_C0CCP3_OUTPUT | 
         GPTIMER_CCPD_C0CCP2_INPUT | GPTIMER_CCPD_C0CCP1_INPUT | 
         GPTIMER_CCPD_C0CCP0_INPUT);;

} /* motor0_pwm_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function adjusts the PWM signal by setting the timer's threshold 
//    based on the given duty cycle percentage. The threshold is percentage 
//    of the terminal count (load value) of the timer.
//
// INPUT PARAMETERS:
//    duty_cycle - an 8-bit value that represents the desired duty cycle 
//                 percentage (0-100) used to calculate the timer's threshold.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void motor0_set_pwm_dc(uint8_t duty_cycle)
{
  uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * duty_cycle) / 100;

  TIMA0->COUNTERREGS.CC_23[1] = GPTIMER_CC_23_CCVAL_MASK & threshold;
} /* motor_set_pwm */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function adjusts the PWM signal by setting the timer's threshold 
//    based on the given timer count value.
//
// INPUT PARAMETERS:
//    count - a 32-bit count value used to set the timer's threshold.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void motor0_set_pwm_count(uint32_t count)
{
  TIMA0->COUNTERREGS.CC_23[1] = GPTIMER_CC_23_CCVAL_MASK & count;
} /* motor0_set_pwm_count */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables the timer, allowing the PWM signal generation 
//    to begin.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// -----------------------------------------------------------------------------
void motor0_pwm_enable(void)
{
    TIMA0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
} /* motor_pwm_enable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function disables the timer, allowing the PWM signal generation 
//    to begin.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// -----------------------------------------------------------------------------
void motor0_pwm_disable(void)
{
    TIMA0->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_MASK);
} /* motor_pwm_disable */



//***************************************************************************
//***************************************************************************
//******                    DAC Managment functions                    ******
//***************************************************************************
//***************************************************************************


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DAC12 peripheral for ...
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void dac_init(void)
{
  // Resets DAC12 peripheral
  DAC0->GPRCM.RSTCTL = (I2C_RSTCTL_KEY_UNLOCK_W |
                  I2C_RSTCTL_RESETSTKYCLR_CLR | I2C_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to DAC12 peripheral
  DAC0->GPRCM.PWREN = (I2C_PWREN_KEY_UNLOCK_W | I2C_PWREN_ENABLE_ENABLE);

  // time for DAC12 to power up
  clock_delay(PERIPHERIAL_PWR_UP_DELAY);

  // Selects CTL0 register for DAC
  DAC0->CTL0 = DAC12_CTL0_DFM_BINARY | DAC12_CTL0_RES__12BITS | 
               DAC12_CTL0_ENABLE_CLR;

  // Selects CTL1 register for DAC
  DAC0->CTL1 = DAC12_CTL1_OPS_OUT0 | DAC12_CTL1_REFSN_VSSA |  
               DAC12_CTL1_REFSP_VDDA | DAC12_CTL1_AMPHIZ_HIZ | 
               DAC12_CTL1_AMPEN_ENABLE;

  // Selects CTL2 register for DAC
  DAC0->CTL2 = DAC12_CTL2_DMATRIGEN_CLR | DAC12_CTL2_FIFOTRIGSEL_STIM |
              DAC12_CTL2_FIFOTH_LOW | DAC12_CTL2_FIFOEN_CLR;

  // Selects CTL3 register for DAC
  DAC0->CTL3 = DAC12_CTL3_STIMCONFIG__500SPS | DAC12_CTL3_STIMEN_CLR;

} /* dac_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DAC12 peripheral for ...
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void dac_enable(void)
{
  DAC0->CTL0 |= DAC12_CTL0_ENABLE_SET;
} /* dac_enable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DAC12 peripheral for ...
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void dac_disable(void)
{
  DAC0->CTL0 &= ~DAC12_CTL0_ENABLE_MASK;
} /* dac_disable */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the DAC12 peripheral for ...
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void dac_write_data(uint16_t data)
{
  DAC0->DATA0 = data;
} /* dac_write_data */