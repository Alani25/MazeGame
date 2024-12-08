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

void config_pb1_interrupts(void);

void config_pb2_interrupts(void);

bool playGame(const char *maze);

void buzzer(void);

void mainMenu(void);

/*
TODO

- Move switch case statement from choose level function into main loop
- remove choose level

DONE
- turn levelNum into global variable
- create main menu function (prototype DONE)



*/

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
// maximum level number (can't be greater than 3)
#define MAX_LEVEL_NUM 3

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// default player position on the map
volatile int playerPos = 24;
// current level selected (also affects game mode)
volatile uint8_t levelNum = 0;
// counter variable to keep track of frames since start of the project
volatile uint16_t counter = 0;

// pb1 & pb2 interrupts
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;

// Define a structure to hold different data types
int main(void) {
  // defaul, setup
  clock_init_40mhz();
  launchpad_gpio_init();
  I2C_init();
  lp_leds_init();
  lcd1602_init(); //
  dipsw_init();   // ?
  lpsw_init();    // ?
  config_pb1_interrupts();
  config_pb2_interrupts();
  led_init();
  led_enable();
  seg7_init(); // ?
  spi1_init();
  seg7_off();
  UART_init(115200); // baud rate of 115200
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA); // Initialize ADC
  config(); // pin configurations

  // all maze levels
  char mazeMap[][66] = {
    // level 0.  Catacombs
    "# #    #"   //  0- 7
    " #  ## #"   //  8-15
    " # #   #"   // 16-23
    "   # ###"   // 24-31
    " ###    "   // 32-39 END of maze here
    " # # # #"   // 40-47
    " # ### #"   // 48-55
    "    #  # ", // 56-63

    // level 1.  Mitochondria
    " # # # #"   //  0- 7
    "   #   #"   //  8-15
    " # # # #"   // 16-23
    " # # # #"   // 24-31
    " # # # #"   // 32-39 
    " # # # #"   // 40-47
    " # # #  "   // 48-55 END of maze here
    " #   # # ", // 56-63

    // level 2.  Flat Earth
    "       #"   //  0- 7
    "       #"   //  8-15
    "       #"   // 16-23
    "       #"   // 24-31
    "       #"   // 32-39 END of maze here
    "       #"   // 40-47
    "        "   // 48-55
    "######## " // 56-63
  }; 
    

  bool done = false;

  while(!done){
    // MAIN MENU MODE
    // Display main meny until a level is selected
    mainMenu();

    // GAME MODE 
    bool win = playGame(mazeMap[levelNum]);
    // notify user that they have completed the maze
    lcd_clear();
    if (win){ 
        UART_write_string("\n\r\n\r MAZE 1 COMPLETE \n\r\n\r");
        lcd_write_string("Maze Complete :D");
        buzzer();
    } else {
        UART_write_string("\n\r\n\r DEFEAT \n\r\n\r");
        lcd_write_string("YOU GAVE UP :(");
    }
    // Note: since win variable is used once, we don't need to create variable
    // can just write if(playGame(mazeMap)) but that might be "bad code practice"

    lcd_set_ddram_addr(0x40);
    lcd_write_string("Press 2: MENU");
    while(!g_pb2_pressed); // wait for push button 1 to restart

  }


  while (1);

} /* main */

// Keep all functions below this point

/////////////////////
//    MAIN MENU    //
/////////////////////
void mainMenu(void) {
  // display level info on LCD screen
  lcd_clear();
  lcd_write_string("level: "); // <-- address 0x07 will be used to display level num after this
  lcd_set_ddram_addr(0x40);
  lcd_write_string("See serial menu");

  bool done = false;
  while (!done) {
    // Display level number
    lcd_set_ddram_addr(0x07);
    lcd_write_byte(levelNum);

    UART_write_string("\n\r");

    // Display
    UART_write_string("\n\r>>>>>  MAZE LEVEL  <<<<<\n\r");
    UART_write_string("Use JOYSTICK to switch between levels,\n\r");
    UART_write_string("Push button 1 to select level.\n\r\n\r");

    // define level names here
    char levelNames[MAX_LEVEL_NUM][20] = // make sure to adjust max length based on names
    { "0. Catacombs\n\r", "1. Mitochondria\n\r", "2. Flat Earth\n\r" };

    // display all levels in serial console
    for (uint8_t i = 0; i < MAX_LEVEL_NUM; i++) {
      UART_write_string(levelNum == i ? ">> Level " : "   Level ");
      UART_write_string(levelNames[i]);
    }
    // keep this here, 3 enters send to serial
    UART_write_string("\n\r\n\r");

    // delay slightly to avoid counting input as double
    msec_delay(500);

    // get user inputed character based on KEYs & JOYSTICK
    char inputChar = ' ';
    // wait until we get input we should act on
    while (inputChar != 'w' && inputChar != 's' && !g_pb1_pressed) {
      // TODO turn this into a function for getting input since we use it
      // multiple times Check for keyboard
      if ((UART0->STAT & UART_STAT_RXFE_MASK) != UART_STAT_RXFE_SET)
        inputChar = ((char)(UART0->RXDATA));
      else if ((counter % 20) == 0) // if not keyboard then check joystick
        inputChar = move_joystick();
    }
    // Act based on the input
    switch (inputChar) {
    case 'w':
      if (levelNum > 0)
        levelNum--;
      break;

    case 's':
      if (levelNum < MAX_LEVEL_NUM - 1)
        levelNum++;
      break;

    default: // only other case is if the user pressed pb1
      done = true;
    }
  } /* while loop for main menu interaction */
} /* main menu function ends here */

/////////////////////
//    GAMEPLAY     //
/////////////////////
bool playGame(const char *maze) {
  // LCD instructions
  lcd_clear();
  lcd_write_string("Press 1: RESTART");
  lcd_set_ddram_addr(0x40);
  lcd_write_string("Press 2: QUIT");

  bool done = false;
  // GAME MODE
  while (!done) {
    // global counter to keep track of frames
    counter++;

    // matrix_test(maze1); // TODO fix wiring with master slave 

    // display maze in serial console
    // UART_write_string("\n\r\n\r Maze 1 \n\r");
    // TODO: Combine these two functions into one (I'll do this later)
    displayMaze(maze); // display maze in serial console
    matrix_test(maze); // display maze on 8x8 LEDs

    // get user inputed character based on KEYs & JOYSTICK
    char inputChar = ' ';

    // Check for keyboard, if none clicked then check for joystick
    if ((UART0->STAT & UART_STAT_RXFE_MASK) != UART_STAT_RXFE_SET)
      inputChar = ((char)(UART0->RXDATA));
    else if ((counter % 20) == 0)
      inputChar = move_joystick();
    // do{ ... } while (inputChar == ' ');

    // TODO: Turn into an interrupt
    if (g_pb1_pressed)
      playerPos = 24;
    g_pb1_pressed = false;
    // interact with maze based on input
    switch (inputChar) {
    case 'w':
      // move up
      if (playerPos > 7 && maze[playerPos - 8] != '#')
        playerPos -= 8;
      break;

    case 's':
      // move down
      if (playerPos < 56 && maze[playerPos + 8] != '#')
        playerPos += 8;
      break;

    case 'd':
      // move right
      if ((playerPos + 1) % 8 != 0 &&
          maze[playerPos + 1] !=
              '#') // first check unnecessary, but goes with logic
        playerPos++;
      break;

    case 'a':
      // move left
      if (playerPos % 8 != 0 && maze[playerPos - 1] != '#')
        playerPos--;
      break;

    case 'r':
      // reset position
      playerPos = 24;
      break;
    }

    if ((playerPos + 1) % 8 ==
        0) { // if player exits the maze or pb 2 pressed end loop
      done = true;
      return true;
    } else if (g_pb2_pressed) {
      g_pb2_pressed = false;
      done = true;
      return false;
    }

    UART_write_string("\n\r"); // make sure to return to send data through serial

  } /* while loop, for playing the maze level */
}

////////////////////
// JOYSTICK INPUT //
////////////////////

// Convert joystick readings into WASD chars for movement
char move_joystick(void) {

  uint16_t adc_y = ADC0_in(7);
  uint16_t adc_x = ADC0_in(0);

  if (adc_x < 5 || adc_x > 500 || adc_y < 500 ||
      adc_y > 3e3) { // clean up later TODO
    // Check adc range to return WASD direction
    if (adc_y < 500)
      return 'w';
    if (adc_y > 3e3)
      return 's';
    if (adc_x < 5)
      return 'a';
    if (adc_x > 1000)
      return 'd';
  }
}

////////////////////
//  DISPLAY MAZE  //
////////////////////

// function to display maze IN THE SERIAL CONSOLE
void displayMaze(const char *string) {
  int index = 0;

  // loop through entire char array
  while (*string != '\0') {

    // add in a new line at the end of each row
    if (index % 8 == 0)
      UART_write_string("\n\r");

    // Place in an X for player position
    if (playerPos == index)
      UART_write_string("X");
    else // draw maze as it is
      UART_out_char(*string);

    // increment index & string char
    index++;
    string++;
  }
  UART_write_string("\n\r\n\r\n\r");
}
// Sending data to with 75HC595
void send_spi_data(uint16_t spi_data) {
  spi1_write_data((uint8_t)spi_data);
  uint8_t received_data = spi1_read_data(); // TODO: Is this line necessary?
                                            // DONE: Yes it is, plz keep
}
// Update Y coordinates on 8x8 LEDs display
void update_leds(void) {
  GPIOB->DOUT31_0 |= LP_SPI_CS0_MASK;
  msec_delay(1);
  GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;
}
// display the maze on the 8x8 LEDs (788BS)
void matrix_test(const char *string) {

  // visual maze display for reference
  // |# # # # |     0- 7
  // | # ### #|     8-15
  // | # #   #|    16-23
  // |   # ###|    24-31
  // | ###    |    32-39 END of maze here
  // | # # # #|    40-47
  // | # ### #|    48-55
  // |    #  #|    56-63

  int index = 0;
  uint8_t y = 128; // #10000000

  while (*string != '\0') {

    //  uint8_t y = 2^((uint8_t)floor(index/8));
    if (index % 8 == 0) { // next row
      // short delay BEFORE turning LEDs off, multiplexing
      msec_delay(1);

      // x7, y++
      // turn off all LEDs (x)
      leds_off();

      // Send row info & update LEDs (y)
      send_spi_data(y); // 1<<(int)floor(index/8));
      update_leds();

      // shift to the right for next row
      y /= 2;
    }

    // NOTE: playerPos == index when we're at the player position

    // Turn on X LED if we're at a wall in row `floor(index/8)`
    if (*string == '#')
      led_on(7 - (index % 8));
    else if (playerPos == index && (counter % 8) < 4) {
      led_on(7 - (index % 8));
    }
    // increment index & string char
    index++;
    string++;
  }
}

////////////////////
//     BUZZER     //
////////////////////
void buzzer(void)
{
  lp_leds_on(LP_RGB_BLU_PORT); //Buzzer input at port 22, same port used for the lp_rgb_blu
}


////////////////////
//  SERIAL WRITE  //
////////////////////

// Writing out simple string to serial console
void UART_write_string(const char *string) {
  while (*string != '\0') {
    UART_out_char(*string++);
  }

} /* UART_write_string */

////////////////////
//   PIN CONFIG   //
////////////////////

// pin configurations, necessary for ADC readings and other functionalities
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

////////////////////
//   INTERRUPTS   //
////////////////////

void config_pb1_interrupts(void) {
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}
void config_pb2_interrupts(void) {
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
}
void GROUP1_IRQHandler(void) {
  uint32_t group_iidx_status;
  uint32_t gpio_mis;

  do {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;

    switch (group_iidx_status) {
    case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
      gpio_mis = GPIOB->CPU_INT.MIS;
      if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) ==
          GPIO_CPU_INT_MIS_DIO18_SET) {
        g_pb1_pressed = !g_pb1_pressed;

        GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
      }

      break;
    case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
      gpio_mis = GPIOA->CPU_INT.MIS;
      if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) ==
          GPIO_CPU_INT_MIS_DIO15_SET) {
        g_pb2_pressed = true;

        GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
      }

      break;

    default:
      break;
    }
  } while (group_iidx_status != 0);
}
