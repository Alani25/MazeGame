// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.1
//
//       FILE NAME:  LCD1602.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This module provides functions to initialize and control a 16x2 character
//    LCD (LCD1602) via I2C communication. It supports configuring the LCD for
//    a 4-bit interface, 2 lines, 5x7 characters, turning on/off the display,
//    setting the cursor, writing characters and strings, and clearing the
//    display.
//
//    NOTE: This code assumes that the IIC address is 0x27.
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
// *****************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "clock.h"
#include "lcd1602.h"
#include "LaunchPad.h"

//-----------------------------------------------------------------------------
// global signal to track status of backlight of LCD module
//-----------------------------------------------------------------------------
static uint8_t g_lcd_backlight_mode = 0;


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function configures the LCD for a 4-bit interface, 2 lines and 5x7 
//    character format. It turns on the LCD, turns the cursor off without 
//    blinking, clears the display, and returns the cursor to the 
//    home position.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//   uint32_t - Status code indicating success or failure of the operation.
//        0 if successful
//        1 if failure
// -----------------------------------------------------------------------------
uint32_t lcd1602_init(void)
{
  #define MAX_NUM_CMDS  (8)

  uint32_t status = 0;

  uint8_t lcd_init_code[MAX_NUM_CMDS] = {
    LCD_FUNCTION_SET_CMD   | LCD_8BIT_MODE,
    LCD_FUNCTION_SET_CMD   | LCD_8BIT_MODE,
    LCD_FUNCTION_SET_CMD   | LCD_8BIT_MODE,
    LCD_FUNCTION_SET_CMD   | LCD_4BIT_MODE,
    LCD_FUNCTION_SET_CMD   | LCD_4BIT_MODE | LCD_2_LINE_DISPLAY | LCD_5x8_DOTS,
    LCD_DISPLAY_CNTRL_CMD  | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF,
    LCD_ENTRY_MODE_SET_CMD | LCD_ADDR_INC_ENABLE | LCD_SHIFT_DISABLE,
    LCD_CLEAR_DISPLAY_CMD};

  // send the first 4 commands as a single I2C (nibble) transfer
  for (uint8_t index = 0; index < 4; index++)
  {
    status |= I2C_send1(LCD_IIC_ADDRESS, lcd_init_code[index] |
                      LCD_BACKLIGHT_ON | LCD_E_SIGNAL_HIGH | LCD_INSTR_REG);
    msec_delay(IIC_TIME_DELAY_2MS);

    status |= I2C_send1(LCD_IIC_ADDRESS, lcd_init_code[index] |
                        LCD_BACKLIGHT_ON | LCD_INSTR_REG);
    msec_delay(IIC_TIME_DELAY_2MS);
  } /* for */

  // Send the first 4 commands as a two I2C transfer
  for (uint8_t index = 4; index < MAX_NUM_CMDS; index++)
  {
    status |= lcd1602_write(LCD_IIC_ADDRESS, lcd_init_code[index], LCD_INSTR_REG);
    msec_delay(IIC_TIME_DELAY_2MS);
  } /* for */

  lcd_set_display_on();

  return (status);

} /* lcd1602_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function sends data to the LCD1602 display over the I2C interface. 
//    It sends the upper and lower nibbles of the provided data byte separately 
//    with the appropriate register select bit and backlight mode. Each nibble 
//    is sent with the enable signal toggled to latch the data.
//
// INPUT PARAMETERS:
//   iic_addr   - the 8-bit I2C address of the LCD1602 display.
//   data       - the byte of data to be sent to the LCD.
//   reg_select - an 8-bit value to indicate whether the data is to be written 
//                to the instruction registers (0) or data registers (1).
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   uint32_t - Status code indicating success or failure of the operation.
//        0 if successful
//        1 if failure
// -----------------------------------------------------------------------------
uint32_t lcd1602_write(uint8_t iic_addr, uint8_t data, uint8_t reg_select)
{
  uint32_t status = 0;
  uint8_t  upper_nibble = (data & UPPER_NIBBLE_MASK);
  uint8_t  lower_nibble = (data & LOWER_NIBBLE_MASK) << NIBBLE_SHIFT;

  upper_nibble |= g_lcd_backlight_mode | reg_select;
  lower_nibble |= g_lcd_backlight_mode | reg_select;

  // send upper nibble
  status = I2C_send1(iic_addr, upper_nibble | LCD_E_SIGNAL_HIGH);
  msec_delay(IIC_TIME_DELAY_2MS);

  status = I2C_send1(iic_addr, upper_nibble);
  msec_delay(IIC_TIME_DELAY_2MS);

  // send lower nibble
  status |= I2C_send1(iic_addr, lower_nibble | LCD_E_SIGNAL_HIGH);
  msec_delay(IIC_TIME_DELAY_2MS);

  status = I2C_send1(iic_addr, lower_nibble | reg_select);
  msec_delay(IIC_TIME_DELAY_2MS);

  return (status);
} /* lcd1602_write */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function turns off the backlight of the LCD1602 display by updating
//    the backlight mode state and sending the updated state over the I2C 
//    interface. It ensures that the display backlight is turned off.
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
void lcd_set_display_off(void)
{
  g_lcd_backlight_mode &= ~LCD_BACKLIGHT_ON;

  (void)I2C_send1(LCD_IIC_ADDRESS, g_lcd_backlight_mode);
  msec_delay(IIC_TIME_DELAY_2MS);

} /* lcd_set_display_on */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables the backlight on the LCD module. It updates the 
//    backlight mode state to turn on the backlight and sends the updated 
//    state over the I2C interface.
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
void lcd_set_display_on(void)
{
  g_lcd_backlight_mode |= LCD_BACKLIGHT_ON;

  (void)I2C_send1(LCD_IIC_ADDRESS, g_lcd_backlight_mode);
  msec_delay(IIC_TIME_DELAY_2MS);

} /* lcd_set_display_on */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function sets the DDRAM address of the LCD module where the data 
//    should be stored. No error checking is done to verify if the address is 
//    a valid DDRAM address.
//
// INPUT PARAMETERS:
//    address - An 8-bit value representing the DDRAM address.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_set_ddram_addr(uint8_t address)
{
  uint8_t command = address | LCD_SET_DDRAM_ADDR_CMD;

  (void)lcd1602_write(LCD_IIC_ADDRESS, command, LCD_INSTR_REG);

} /* lcd_set_ddram_addr */



//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function accepts a single character and writes it into the DDRAM at
//    the current address. No error checking is done to verify if the character
//    is valid for the LCD to display.
//
// INPUT PARAMETERS:
//    character - An 8-bit value representing the character to be displayed.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_write_char(uint8_t character)
{

  (void)lcd1602_write(LCD_IIC_ADDRESS, character, LCD_DATA_REG);

} /* lcd_write_char */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function writes a string to the DDRAM on LCD module. Each character 
//    of the string is written to the LCD one by one. This function performs
//    no error checking to ensure the string is displayed properly on the LCD.
//
// INPUT PARAMETERS:
//    string    - A pointer (address) to the null-terminated string to be 
//                written to the DDRAM
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_write_string(const char *string)
{
    // for each character in string, write it to the LCD module
    while (*string != '\0')
    {
        lcd_write_char(*string++);
    } /* while */

} /* lcd_write_string */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function sends the clear display command to the LCD module to clear
//    the LCD display.
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
void lcd_clear(void)
{

  (void)lcd1602_write(LCD_IIC_ADDRESS, LCD_CLEAR_DISPLAY_CMD, LCD_INSTR_REG);

} /* lcd_clear */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function takes an 8-bit hexadecimal value and returns the ASCII
//    representation of the lower nibble. It converts the lower nibble to its
//    ASCII equivalent, where values 0-9 map to '0'-'9' and values A-F map to
//    'A'-'F'.
//
// INPUT PARAMETERS:
//    hex_value - An 8-bit hexadecimal value from which the lower nibble is 
//                extracted and converted to an ASCII character.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    The ASCII representation of the lower nibble of the input hexadecimal
//    value.
// -----------------------------------------------------------------------------
int8_t hex_to_ascii(uint8_t hex_value)
{
  int8_t  ascii_value  = 0x00;
  uint8_t lower_nibble = hex_value & LOWER_NIBBLE_MASK;

  if (lower_nibble <= 9) 
  {
      ascii_value = '0' + lower_nibble;
  } /* if */
  else 
  {
      ascii_value = 'A' + (lower_nibble - 10); 
  } /* else */

  return (ascii_value);
} /* hex_to_ascii */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function takes an 8-bit hexadecimal value and writes the ASCII
//    representation of its lower nibble to an LCD display. It first calls
//    the `hex_to_ascii` function to convert the lower nibble of the input 
//    hexadecimal value to its ASCII equivalent. Then, it passes the ASCII 
//    character to the `lcd_write_char` function to display it on the LCD.
//
// INPUT PARAMETERS:
//    hex_value - The input 8-bit hexadecimal value from which the lower nibble
//                is extracted and converted to an ASCII character for display.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void hex_to_lcd(uint8_t hex_value)
{

  lcd_write_char(hex_to_ascii(hex_value));

} /* hex_to_lcd */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts an 8-bit byte to its ASCII string representation,
//    padding it to 3 characters, right-justified, with leading spaces if
//    the number has fewer than 3 digits. The ASCII string is stored in the
//    provided character array and is null-terminated.
//
// INPUT PARAMETERS:
//    byte    - The input 8-bit number to be converted to ASCII.
//    string  - Pointer to a character array where the ASCII string 
//              representation will be stored. It should have enough space to 
//              hold 3 characters plus the null terminator.
//
// OUTPUT PARAMETERS:
//    string  - Three numeric ASCII character, NULL terminated, string of 
//              the converted number.
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void byte_to_ascii(uint8_t byte, char* string) 
{
  // Extract each digit of the byte and convert it to ASCII
  for (uint8_t digit_idx = 0; (digit_idx < 3); digit_idx++) 
  {
    // Always convert first digit even if it is byte is 0
    if ((byte != 0) || (digit_idx == 0))
    {
      // Convert digit to ASCII
      string[2-digit_idx] = '0' + (byte % 10);
    
      // Move to the next digit
      byte /= 10;
    } /* if */
    else
    {
      string[2-digit_idx] = ' ';
    } /* else */
  } /* for */
  
} /* byte_to_ascii */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts a 16-bit unsigned integer (double byte) to its  
//    ASCII string representation, padding it to 5 characters, right-justified, 
//    with no leading '0's. The ASCII string is stored in the provided character 
//    array. The string will represent the decimal value of the 16-bit integer.
//
// INPUT PARAMETERS:
//    doublebyte - The 16-bit unsigned integer to be converted to ASCII.
//    string     - Pointer to a character array where the ASCII string
//                 representation will be stored. It should have enough space
//                 to hold 5 characters plus the null terminator.
//
// OUTPUT PARAMETERS:
//    string     - Five numeric ASCII characters, NULL terminated, string of 
//                 the converted number.
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void doublebyte_to_ascii(uint16_t doublebyte, char* string) 
{
  // Extract each digit of the double byte and convert it to ASCII
  for (uint8_t digit_idx = 0; (digit_idx < 5); digit_idx++) 
  {
    // Always convert first digit even if it is doublebyte is 0
    if ((doublebyte != 0) || (digit_idx == 0))
    {
      // Convert digit to ASCII
      string[4-digit_idx] = '0' + (doublebyte % 10);
    
      // Move to the next digit
      doublebyte /= 10;
    } /* if */
    else
    {
      string[4-digit_idx] = ' ';
    } /* else */
  } /* for */
  
} /* doublebyte_to_ascii */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts a 16-bit unsigned integer (double byte) to its  
//    ASCII string representation, padding it to 5 characters, right-justified, 
//    with no leading '0's. The ASCII string is stored in the provided character 
//    array. The string will represent the decimal value of the 16-bit integer.
//
// INPUT PARAMETERS:
//    doublebyte - The 16-bit unsigned integer to be converted to ASCII.
//    string     - Pointer to a character array where the ASCII string
//                 representation will be stored. It should have enough space
//                 to hold 10 characters plus the null terminator.
//
// OUTPUT PARAMETERS:
//    string     - Ten numeric ASCII characters, NULL terminated, string of 
//                 the converted number.
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void quadbyte_to_ascii(uint32_t quadbyte, char* string) 
{
  // Extract each digit of the double byte and convert it to ASCII
  for (uint8_t digit_idx = 0; (digit_idx < 10); digit_idx++) 
  {
    // Always convert first digit even if it is quadbyte is 0
    if ((quadbyte != 0) || (digit_idx == 0))
    {
      // Convert digit to ASCII
      string[9-digit_idx] = '0' + (quadbyte % 10);
    
      // Move to the next digit
      quadbyte /= 10;
    } /* if */
    else
    {
      string[9-digit_idx] = ' ';
    } /* else */
  } /* for */
  
} /* quadbyte_to_ascii */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts an 8-bit byte to its binary ASCII string
//    representation and writes the string to an LCD display using the 
//    `lcd_write_string` function. The string represents the byte in a 
//    3-character binary format, right-justified with leading spaces.
//
// INPUT PARAMETERS:
//    byte - The input 8-bit byte value to be converted to a binary ASCII string
//           and written to the LCD.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_write_byte(uint8_t byte)
{
  // reserve space for up to 3 characters
  char string_buffer[] = "   "; 
  
  // Convert byte to binary ASCII string
  byte_to_ascii(byte, string_buffer);
  
  // Write binary ASCII string to LCD
  lcd_write_string(string_buffer);
  
} /* lcd_write_byte */

// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts a 16-bit double byte to its binary ASCII string
//    representation and writes it to an LCD display using lcd_write_string.
//
// INPUT PARAMETERS:
//    doublebyte - The input 16-bit double byte value to be converted to binary 
//                 ASCII string and written to the LCD.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_write_doublebyte(uint16_t doublebyte)
{
  // reserve space for up to 5 characters
  char string_buffer[] = "     ";
  
  // Convert double byte to binary ASCII string
  doublebyte_to_ascii(doublebyte, string_buffer);
  
  // Write binary ASCII string to LCD
  lcd_write_string(string_buffer);

} /* lcd_write_doublebyte */


// -----------------------------------------------------------------------------
// DESCRIPTION
//    This function converts a 16-bit double byte to its binary ASCII string
//    representation and writes it to an LCD display using lcd_write_string.
//
// INPUT PARAMETERS:
//    doublebyte - The input 16-bit double byte value to be converted to binary 
//                 ASCII string and written to the LCD.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void lcd_write_quadbyte(uint32_t quadbyte)
{
  // reserve space for up to 10 characters
  char string_buffer[] = "          ";
  
  // Convert double byte to binary ASCII string
  quadbyte_to_ascii(quadbyte, string_buffer);
  
  // Write binary ASCII string to LCD
  lcd_write_string(string_buffer);

} /* lcd_write_quadbyte */
