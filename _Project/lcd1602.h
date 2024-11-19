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

#ifndef __LCD1602_H__
#define __LCD1602_H__

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// I2C Bus address for the LCD1602 module
// Update this address if module has a different IIC address
//----------------------------------------------------------------------------
#define LCD_IIC_ADDRESS 0x27

// Define LCD size (16 characters x 2 lines)
#define CHARACTERS_PER_LCD_LINE                                             (16)
#define LINES_PER_LCD                                                        (2)
#define TOTAL_CHARACTERS_PER_LCD         (LINES_PER_LCD*CHARACTERS_PER_LCD_LINE)
#define LCD_LINE1_ADDR                                                    (0x00)
#define LCD_LINE2_ADDR                                                    (0x40)

//# Define LCD line numbers
#define LCD_LINE_NUM_1                                                       (0)
#define LCD_LINE_NUM_2                                                       (1)

// Define LCD character positions
#define LCD_CHAR_POSITION_1                                                  (0)
#define LCD_CHAR_POSITION_2                                                  (1)
#define LCD_CHAR_POSITION_3                                                  (2)
#define LCD_CHAR_POSITION_4                                                  (3)
#define LCD_CHAR_POSITION_5                                                  (4)
#define LCD_CHAR_POSITION_6                                                  (5)
#define LCD_CHAR_POSITION_7                                                  (6)
#define LCD_CHAR_POSITION_8                                                  (7)
#define LCD_CHAR_POSITION_9                                                  (8)
#define LCD_CHAR_POSITION_10                                                 (9)
#define LCD_CHAR_POSITION_11                                                (10)
#define LCD_CHAR_POSITION_12                                                (11)
#define LCD_CHAR_POSITION_13                                                (12)
#define LCD_CHAR_POSITION_14                                                (13)
#define LCD_CHAR_POSITION_15                                                (14)
#define LCD_CHAR_POSITION_16                                                (15)

#define IIC_TIME_DELAY_2MS                                                     2
#define NIBBLE_SHIFT                                                           4
#define UPPER_NIBBLE_MASK                                                   0xF0
#define LOWER_NIBBLE_MASK                                                   0x0F

//# Define IIC Port Expander bit positions
#define READ_ENABLE                                                          0x2
#define WRITE_ENABLE                                                         0x0
#define LATCH_ENABLE                                                         0x4
#define LATCH_DISABLE                                                        0x0

//# Define LCD1602 Register Select (RS) values
#define LCD_INSTR_REG                                                        0x0
#define LCD_DATA_REG                                                         0x1

// LCD1602 control mode for RS (Instruction/Data), RW, and E (Latch)
#define LCD_E_SIGNAL_HIGH                                                 (1<<2)
#define LCD_BACKLIGHT_ON                                                  (1<<3)
#define LCD_RS_BIT_MASK                                                     0x01
#define LCD_RW_BIT_MASK                                                     0x02
#define LCD_EN_BIT_MASK                                                     0x04
#define LCD_LED_BIT_MASK                                                    0x08

// LCD1602 commands
#define LCD_CLEAR_DISPLAY_CMD                                               0x01
#define LCD_RETURN_HOME_CMD                                                 0x02
#define LCD_ENTRY_MODE_SET_CMD                                              0x04
#define LCD_DISPLAY_CNTRL_CMD                                               0x08
#define LCD_CURSOR_SHIFT_CMD                                                0x10
#define LCD_FUNCTION_SET_CMD                                                0x20
#define LCD_SET_CGRAM_ADDR_CMD                                              0x40
#define LCD_SET_DDRAM_ADDR_CMD                                              0x80

// Bits for display entry mode command
#define LCD_ENTRY_RIGHT                                                     0x00
#define LCD_ENTRY_LEFT                                                      0x02
#define LCD_ENTRY_SHIFT_INC                                                 0x01
#define LCD_ENTRY_SHIFT_DEC                                                 0x00

// Bits for display on/off control command
#define LCD_BLINK_ON                                                        0x01
#define LCD_BLINK_OFF                                                       0x00
#define LCD_CURSOR_ON                                                       0x02
#define LCD_CURSOR_OFF                                                      0x00
#define LCD_DISPLAY_ON                                                      0x04
#define LCD_DISPLAY_OFF                                                     0x00

// Bits for display/cursor shift control command
#define LCD_MOVE_RIGHT                                                      0x04
#define LCD_MOVE_LEFT                                                       0x00
#define LCD_DISPLAY_MOVE                                                    0x08
#define LCD_CURSOR_MOVE                                                     0x00

// Bits for function set command
#define LCD_8BIT_MODE                                                       0x10
#define LCD_4BIT_MODE                                                       0x00
#define LCD_2_LINE_DISPLAY                                                  0x08
#define LCD_1_LINE_DISPLAY                                                  0x00
#define LCD_5x10_DOTS                                                       0x04
#define LCD_5x8_DOTS                                                        0x00

// Bits for entry mode set command
#define LCD_SHIFT_DISABLE                                                   0x00
#define LCD_SHIFT_ENABLE                                                    0x01
#define LCD_ADDR_DEC_ENABLE                                                 0x00
#define LCD_ADDR_INC_ENABLE                                                 0x02




// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
uint32_t lcd1602_init(void);
void lcd_clear(void);
void lcd_set_ddram_addr(uint8_t address);
void lcd_write_char(uint8_t character);
void lcd_write_string(const char *string);
uint32_t lcd1602_write(uint8_t iic_addr, uint8_t data, uint8_t reg_select);
void lcd_set_display_on(void);
void lcd_set_display_off(void);
void hex_to_lcd(uint8_t hex_value);
int8_t hex_to_ascii(uint8_t hex_value);

void lcd_write_byte(uint8_t byte);
void lcd_write_doublebyte(uint16_t doublebyte);
void lcd_write_quadbyte(uint32_t word);


#endif /* __LCD1602_H__ */
