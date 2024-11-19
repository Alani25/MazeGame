//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a ...
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "LaunchPad.h"
#include "adc.h"
#include "clock.h"
#include "lcd1602.h"
#include "spi.h"
#include "uart.h"
#include <math.h>

/*

#include "LaunchPad.h"
#include "adc.h"
#include "clock.h"
#include "lcd1602.h"
#include "spi.h"
#include "uart.h"
#include <math.h>

added in the math library to use the pow function


*/

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void UART_write_string(const char *string);

void config(void);

void initMap(const char *string);

void displayMaze(const char *string);

void move_joystick(void);

// void run_project(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

volatile int playerPos = 24; // player position on map by default


// Define a structure to hold different data types

int main(void) {
  clock_init_40mhz();    //
  launchpad_gpio_init(); //

  I2C_init(); //

  lcd1602_init(); //
  dipsw_init();   //
  lpsw_init();    //
                  //   keypad_init();
      
  seg7_init();

  seg7_off();

  UART_init(115200); // trying baud rate of 115200

//   spi1_init();
//   IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] =
//       IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;
//   GPIOB->DOE31_0 |= LP_SPI_CS0_MASK;
//   GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;

  //   OPA0_init();
 ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA); // Initialize ADC
 config();

    // char maze1[] =  "       #" //  0- 7
    //                 " # ### #" //  8-15
    //                 " # #   #" // 16-23
    //                 "   # ###" // 24-31
    //                 " ###    " // 32-39 END of maze here
    //                 " # # # #" // 40-47
    //                 " # ### #" // 48-55
    //                 "    #  #";// 56-63
    

    // bool done = false;
    // while(!done){
    //     UART_write_string("\n\n Maze 1 \n\n");
    //     displayMaze(maze1);

    //     char inputChar = UART_in_char(); // inputted character
    //     switch (inputChar) {
    //         case 'w':
    //         // move up
    //         if(playerPos>7&&maze1[playerPos-8]!='#')
    //             playerPos-=8;
    //         break;

    //         case 's':
    //         // move down
    //         if(playerPos<56&&maze1[playerPos+8]!='#')
    //             playerPos+=8;
    //         break;

    //         case 'd':
    //         // move right
    //         if((playerPos+1)%8!=0&&maze1[playerPos+1]!='#')
    //             playerPos++;
    //         break;

    //         case 'a':
    //         // move left
    //         if(playerPos%8!=0&&maze1[playerPos-1]!='#')
    //             playerPos--;
    //         break;

    //         case 'r':
    //         // reset position
    //         playerPos = 24;
    //         break;
        
    //     }
    //     if((playerPos+1)%8==0){
    //         done = true;
    //     }
    // }

    // displayMaze(maze1);
    // UART_write_string("\n\n MAZE 1 COMPLETE \n\n");
  while(true)
  {
    move_joystick();
  }

  
  while (1);

} /* main */


// Keep all functions below this point


void displayMaze(const char *string) {
    int index = 0;
  while (*string != '\0') {
    
    if(index%8==0)
        UART_out_char('\n');

    if(playerPos==index)
        UART_out_char('X');
    else
        UART_out_char(*string);
    
    // increment index & string char
    index++;
    string++;
    
  }
  UART_out_char('\n');

}

void move_joystick(void)
{
    uint16_t adc_result = 2400;

    uint16_t adc_difference = adc_result - ADC0_in(7);
    lcd_set_ddram_addr(0x00);
    lcd_write_string("ADC = ");
    lcd_write_doublebyte(adc_result);
    lcd_set_ddram_addr(0x40);
    lcd_write_string("ADC Diff = ");
    lcd_write_doublebyte(adc_difference);
    
    
}

void UART_write_string(const char *string) {
  while (*string != '\0') {
    UART_out_char(*string++);
  }

} /* UART_write_string */

void config(void) {
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);

  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}
