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

char move_joystick(void);

void matrix_test(const char *string);

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
  led_init();
  led_enable();    
  seg7_init();
  spi1_init();
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

    char maze1[] =  "# # # # " //  0- 7
                    " # ### #" //  8-15
                    " # #   #" // 16-23
                    "   # ###" // 24-31
                    " ###    " // 32-39 END of maze here
                    " # # # #" // 40-47
                    " # ### #" // 48-55
                    "    #  #";// 56-63
    

    bool done = false;
    while(!done){
        matrix_test(maze1);
    }
    while(!done){
        UART_write_string("\n\n Maze 1 \n\n");
        displayMaze(maze1);
        msec_delay(100);

        char inputChar = ' ';
        // char inputChar = UART_in_char(); // inputted character
        do
        {
            inputChar = move_joystick();
        } while(inputChar==' ');
        
        
        switch (inputChar) {
            case 'w':
            // move up
            if(playerPos>7&&maze1[playerPos-8]!='#')
                playerPos-=8;
            break;

            case 's':
            // move down
            if(playerPos<56&&maze1[playerPos+8]!='#')
                playerPos+=8;
            break;

            case 'd':
            // move right
            if((playerPos+1)%8!=0&&maze1[playerPos+1]!='#')
                playerPos++;
            break;

            case 'a':
            // move left
            if(playerPos%8!=0&&maze1[playerPos-1]!='#')
                playerPos--;
            break;

            case 'r':
            // reset position
            playerPos = 24;
            break;
        
        }
        if((playerPos+1)%8==0){
            done = true;
        }
    }
    displayMaze(maze1);
    UART_write_string("\n\n MAZE 1 COMPLETE \n\n");
//   while(true)
//   {
//     move_joystick();
//   }

  
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


void send_spi_data(uint16_t spi_data)
{
    
    spi1_write_data((uint8_t)spi_data);

    uint8_t received_data = spi1_read_data();


}

void update_leds(void)
{
  GPIOB->DOUT31_0 |= LP_SPI_CS0_MASK;
  msec_delay(50);
  GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;
}

void matrix_test(const char *string)
{


        // char maze1[] =  "       #" //  0- 7
        //             " # ### #" //  8-15
        //             " # #   #" // 16-23
        //             "   # ###" // 24-31
        //             " ###    " // 32-39 END of maze here
        //             " # # # #" // 40-47
        //             " # ### #" // 48-55
        //             "    #  #";// 56-63

     int index = 0;
    
  while (*string != '\0') {
    
     uint8_t y = 2^((uint8_t)floor(index/8));
    if(index%8==0){
        // next row
        // x7, y++
        // turn off all LEDs and lights
        // floor(y/8)
        leds_off();
        send_spi_data(y);//1<<(int)floor(index/8));
        update_leds();
        msec_delay(500);
    }

    // if(playerPos==index)
    //     UART_out_char('X'); // player location
    // else{


        // we have a wall at 7-(index%8), floor(y/8)
        if(*string == '#')
            led_on(7-(index%8)); 


    // }
    
    // increment index & string char
    index++;
    string++;
    
  }


}

char move_joystick(void)
{
    uint16_t adc_y = ADC0_in(7);
    uint16_t adc_x = ADC0_in(0);
    // msec_delay(25);




    // if(adc_result>2400)
    //     adc_result = adc_result - 2400; // #define the 2400 TODO

    // uint16_t adc_difference = abs(adc_result - 2400); // #define the 2400 as initial position TODO

    lcd_set_ddram_addr(0x00);
    lcd_write_string("X = ");
    lcd_write_doublebyte(adc_x);
    lcd_set_ddram_addr(0x40);
    lcd_write_string("Y = ");
    lcd_write_doublebyte(adc_y);

    
    if(adc_x<5||adc_x>500||adc_y<500||adc_y>3e3){ // clean up later TODO
        // msec_delay(500);
        if(adc_y<500)
            return 'w';
        if(adc_y>3e3)
            return 's';
        if(adc_x<5)
            return 'a';
        if(adc_x>1000)
            return 'd';
    }


    // if(adc_result>6){
    //     msec_delay(500);
    //     uint16_t range = 15;
    //     // left
    //     if(abs(adc_result-2050)<range){
    //         return 'a';
    //     }else if (abs(adc_result-1685)<range) { // down
    //         return 's';
    //     }else if (abs(adc_result-310)<range){ // right
    //         return 'd';
    //     }else if (abs(adc_result-17)<range) {
    //         return 'w';
    //     }

    // }


    // lcd_write_string("ADC Diff = ");
    // lcd_write_doublebyte(adc_difference);
    
    
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

  uint32_t gpio_pincm = IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;
  IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = gpio_pincm;
  GPIOB->DOE31_0 |= LP_SPI_CS0_MASK;
  GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;
}
