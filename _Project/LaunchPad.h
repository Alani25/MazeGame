// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.2
//
//       FILE NAME:  LaunchPad.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains a collection of functions for initializing and
//    configuring various hardware peripherals on the LP-MSPM0G3507 LaunchPad
//    and the CSC202 Expansion Board, including:
//       - LED control
//       - 7-segment display management
//       - 4x4 matrix keypad interfacing
//       - DIP switch reading
//       - I2C communication management
//       - PWM Motor Management
//       - DAC Management
//
//    This code is adapted from various Texas Instruments' LaunchPad
//    project template for the LP-MSPM0G3507, using C language and no RTOS.
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

#ifndef __LAUNCHPAD_H__
#define __LAUNCHPAD_H__

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------

// see ti_devices_msp_peripherals_hw_iomux__include hw_iomux.h
#define PINCM_GPIO_PIN_FUNC                              ((uint32_t)0x00000001U)


// Defines for LEDs on MSPM0G3507 LaunchPad board
#define MAX_NUM_LEDS                                                         (8)
#define LP_RED_LED1_IDX                                                      (0)
#define LP_RGB_RED_LED_IDX                                                   (1)
#define LP_RGB_GRN_LED_IDX                                                   (2)
#define LP_RGB_BLU_LED_IDX                                                   (3)

// Defines for LED Bar on CSC202 Expansion board
#define MAX_NUM_LP_LEDS                                                      (4)
#define LED_BAR_LD0_IDX                                                      (0)
#define LED_BAR_LD1_IDX                                                      (1)
#define LED_BAR_LD2_IDX                                                      (2)
#define LED_BAR_LD3_IDX                                                      (3)
#define LED_BAR_LD4_IDX                                                      (4)
#define LED_BAR_LD5_IDX                                                      (5)
#define LED_BAR_LD6_IDX                                                      (6)
#define LED_BAR_LD7_IDX                                                      (7)

// Defines for seven-segment display on CSC202 Expansion board
#define SEG7_SEG_A_IDX                                                       (0)
#define SEG7_SEG_B_IDX                                                       (1)
#define SEG7_SEG_C_IDX                                                       (2)
#define SEG7_SEG_D_IDX                                                       (3)
#define SEG7_SEG_E_IDX                                                       (4)
#define SEG7_SEG_F_IDX                                                       (5)
#define SEG7_SEG_G_IDX                                                       (6)
#define SEG7_SEG_H_IDX                                                       (7)

// Defines for 4x4 Keypad interface on CSC202 Expansion board
#define MAX_NUM_LPSW                                                         (2)
#define MAX_NUM_DIPSW                                                        (4)
#define MAX_NUM_KP_ROWS                                                      (4)
#define MAX_NUM_KP_COLS                                                      (4)

#define GPIO_PORTA                                                           (0)
#define GPIO_PORTB                                                           (1)

// Defines for RED LED1 on Launchpad
#define LP_LED_RED_PORT                                             (GPIO_PORTA)
#define LP_LED_RED_MASK                                                 (1 << 0)
#define LP_LED_RED_IOMUX                                          (IOMUX_PINCM1)

// Defines for RGB LED on Launchpad
#define LP_RGB_RED_PORT                                             (GPIO_PORTB)
#define LP_RGB_RED_MASK                                                (1 << 26)
#define LP_RGB_RED_IOMUX                                         (IOMUX_PINCM57)
#define LP_RGB_GRN_PORT                                             (GPIO_PORTB)
#define LP_RGB_GRN_MASK                                                (1 << 27)
#define LP_RGB_GRN_IOMUX                                         (IOMUX_PINCM58)
#define LP_RGB_BLU_PORT                                             (GPIO_PORTB)
#define LP_RGB_BLU_MASK                                                (1 << 22)
#define LP_RGB_BLU_IOMUX                                         (IOMUX_PINCM50)


// Defines for LEDs LD0 to LD7 (seg7 segmentas a-h) on CSC202 expansion board
#define LED0_PORT                                                   (GPIO_PORTA)
#define LED0_MASK                                                      (1 << 28)
#define LED0_IOMUX                                                (IOMUX_PINCM3)
#define LED1_PORT                                                   (GPIO_PORTA)
#define LED1_MASK                                                      (1 << 31)
#define LED1_IOMUX                                                (IOMUX_PINCM6)
#define LED2_PORT                                                   (GPIO_PORTB)
#define LED2_MASK                                                      (1 << 20)
#define LED2_IOMUX                                               (IOMUX_PINCM48)
#define LED3_PORT                                                   (GPIO_PORTB)
#define LED3_MASK                                                      (1 << 13)
#define LED3_IOMUX                                               (IOMUX_PINCM30)
#define LED4_PORT                                                   (GPIO_PORTA)
#define LED4_MASK                                                      (1 << 12)
#define LED4_IOMUX                                               (IOMUX_PINCM34)
#define LED5_PORT                                                   (GPIO_PORTA)
#define LED5_MASK                                                      (1 << 13)
#define LED5_IOMUX                                               (IOMUX_PINCM35)
#define LED6_PORT                                                   (GPIO_PORTA)
#define LED6_MASK                                                      (1 << 16)
#define LED6_IOMUX                                               (IOMUX_PINCM38)
#define LED7_PORT                                                   (GPIO_PORTA)
#define LED7_MASK                                                      (1 << 17)
#define LED7_IOMUX                                               (IOMUX_PINCM39)

// Defines for Enables for LEDs and Seg7
#define SEG7_DIG0_ENABLE_IDX                                                 (0)
#define SEG7_DIG1_ENABLE_IDX                                                 (1)
#define SEG7_DIG2_ENABLE_IDX                                                 (2)
#define SEG7_DIG3_ENABLE_IDX                                                 (3)
#define LED_BAR_ENABLE_IDX                                                   (4)

#define ENABLE_DIG0_PORT                                            (GPIO_PORTA)
#define ENABLE_DIG0_MASK                                               (1 << 24)
#define ENABLE_DIG0_IOMUX                                        (IOMUX_PINCM54)
#define ENABLE_DIG1_PORT                                            (GPIO_PORTA)
#define ENABLE_DIG1_MASK                                               (1 << 25)
#define ENABLE_DIG1_IOMUX                                        (IOMUX_PINCM55)
#define ENABLE_DIG2_PORT                                            (GPIO_PORTA)
#define ENABLE_DIG2_MASK                                               (1 << 26)
#define ENABLE_DIG2_IOMUX                                        (IOMUX_PINCM59)
#define ENABLE_DIG3_PORT                                            (GPIO_PORTA)
#define ENABLE_DIG3_MASK                                               (1 << 27)
#define ENABLE_DIG3_IOMUX                                        (IOMUX_PINCM60)
#define ENABLE_LED_PORT                                             (GPIO_PORTA)
#define ENABLE_LED_MASK                                                (1 << 18)
#define ENABLE_LED_IOMUX                                         (IOMUX_PINCM40)

// Define indexes for Switches (SW1) and Pushbutton on CSC202 Expansion Board
#define LP_SW1_IDX                                                           (0)
#define LP_SW2_IDX                                                           (1)

// Defines for switches (actually Pushbuttons) on LaunchPad Board
#define LP_SW1_PORT                                                 (GPIO_PORTA)
#define LP_SW1_MASK                                                    (1 << 18)
#define LP_SW1_IOMUX                                            (IOMUX_PINCM40)
#define LP_SW2_PORT                                                 (GPIO_PORTB)
#define LP_SW2_MASK                                                    (1 << 21)
#define LP_SW2_IOMUX                                             (IOMUX_PINCM49)

// Define indexes for Switches (SW1) and Pushbutton on CSC202 Expansion Board
#define DIP_SW1_IDX                                                          (0)
#define DIP_SW2_IDX                                                          (1)
#define DIP_SW3_IDX                                                          (2)
#define DIP_SW4_IDX                                                          (3)
#define PB1_IDX                                                              (2)
#define PB2_IDX                                                              (3)

// Defines for DIP4 switches
#define DIP_SW1_PORT                                                (GPIO_PORTB)
#define DIP_SW1_MASK                                                   (1 << 19)
#define DIP_SW1_IOMUX                                            (IOMUX_PINCM45)
#define DIP_SW2_PORT                                                (GPIO_PORTA)
#define DIP_SW2_MASK                                                   (1 << 22)
#define DIP_SW2_IOMUX                                            (IOMUX_PINCM47)
#define DIP_SW3_PORT                                                (GPIO_PORTB)
#define DIP_SW3_MASK                                                   (1 << 18)
#define DIP_SW3_IOMUX                                            (IOMUX_PINCM44)
#define DIP_SW4_PORT                                                (GPIO_PORTA)
#define DIP_SW4_MASK                                                   (1 << 15)
#define DIP_SW4_IOMUX                                            (IOMUX_PINCM37)

// Defines for Keypad switches
#define KP_COL0_PORT                                                (GPIO_PORTB)
#define KP_COL0_MASK                                                   (1 << 12)
#define KP_COL0_IOMUX                                            (IOMUX_PINCM29)
#define KP_COL1_PORT                                                (GPIO_PORTB)
#define KP_COL1_MASK                                                   (1 << 17)
#define KP_COL1_IOMUX                                            (IOMUX_PINCM43)
#define KP_COL2_PORT                                                (GPIO_PORTB)
#define KP_COL2_MASK                                                   (1 << 15)
#define KP_COL2_IOMUX                                            (IOMUX_PINCM32)
#define KP_COL3_PORT                                                (GPIO_PORTB)
#define KP_COL3_MASK                                                   (1 << 16)
#define KP_COL3_IOMUX                                            (IOMUX_PINCM33)
#define KP_ROW0_PORT                                                (GPIO_PORTB)
#define KP_ROW0_MASK                                                   (1 << 23)
#define KP_ROW0_IOMUX                                            (IOMUX_PINCM51)
#define KP_ROW1_PORT                                                (GPIO_PORTA)
#define KP_ROW1_MASK                                                    (1 << 8)
#define KP_ROW1_IOMUX                                            (IOMUX_PINCM19)
#define KP_ROW2_PORT                                                (GPIO_PORTB)
#define KP_ROW2_MASK                                                    (1 << 4)
#define KP_ROW2_IOMUX                                            (IOMUX_PINCM17)
#define KP_ROW3_PORT                                                (GPIO_PORTB)
#define KP_ROW3_MASK                                                    (1 << 1)
#define KP_ROW3_IOMUX                                            (IOMUX_PINCM13)


// Define indexes for Switches (SW1) and Pushbutton on CSC202 Expansion Board
#define NO_KEY_PRESSED                                                    (0x10)

// Defines for IIC bus interface
#define MAX_NUM_I2C_BITS                                                     (2)
#define I2C_INST                                                            I2C1
#define I2C_INST_IRQHandler                                      I2C1_IRQHandler
#define I2C_INST_INT_IRQN                                          I2C1_INT_IRQn
#define I2C_BUS_SPEED_HZ                                                  400000
#define I2C_SDA_PORT                                                (GPIO_PORTB)
#define I2C_SDA_MASK                                                    (1 << 3)
#define I2C_SDA_IOMUX                                            (IOMUX_PINCM16)
#define I2C_SDA_PINCM_IOMUX_FUNC                     (IOMUX_PINCM16_PF_I2C1_SDA)
#define I2C_SCL_PORT                                                (GPIO_PORTB)
#define I2C_SCL_MASK                                                    (1 << 2)
#define I2C_SCL_IOMUX                                            (IOMUX_PINCM15)
#define I2C_SCL_PINCM_IOMUX_FUNC                     (IOMUX_PINCM15_PF_I2C1_SCL)





// --------------------------------------------------------------------------
// Prototype for Launchpad support functions
// --------------------------------------------------------------------------
void launchpad_gpio_init(void);

void lp_leds_init(void);
void lp_leds_on(uint8_t index);
void lp_leds_off(uint8_t index);

void led_init(void);
void led_enable(void);
void led_disable(void);
void leds_off(void);
void leds_on(uint32_t value);
void led_on(uint8_t led_idx);
void led_off(uint8_t led_idx);

void seg7_init(void);
void seg7_off(void);
void seg7_on(uint8_t value, uint8_t seg7_dig);
void seg7_hex(uint8_t hex, uint8_t seg7_dig);

void dipsw_init(void);
uint8_t dipsw_read(void);
bool is_pb_down(uint8_t pb_idx);
bool is_pb_up(uint8_t pb_idx);

void lpsw_init(void);
bool is_lpsw_down(uint8_t pb_idx);
bool is_lpsw_up(uint8_t pb_idx);


void keypad_init(void);
uint8_t keypad_scan(void);
uint8_t getkey_pressed(void);
void wait_no_key_pressed(void);


void I2C_init(void);
uint8_t I2C_recv1(uint8_t slave);
uint16_t I2C_recv2(uint8_t slave);
uint32_t I2C_send1(uint8_t slave, uint8_t data);


void motor0_init(void);
void motor0_pwm_init(uint32_t load_value, uint32_t compare_value);
void motor0_pwm_enable(void);
void motor0_pwm_disable(void);
void motor0_set_pwm_dc(uint8_t duty_cycle);
void motor0_set_pwm_count(uint32_t count);

#endif // __LAUNCHPAD_H__
