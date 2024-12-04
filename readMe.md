This here will be the manual for the project.



# PROJECT SUMMARY
Create a digital maze game accessible to people with disabilities, with multiple levels.

To explain in more detail— when a user looks at our project, they will be greeted with a message (LCD Screen) that tells them to turn the potentiometer to select a level. The 7-segment hex LEDs will also display the level number. We plan on having possibly around 4 to 9 levels, and we’ll need to have an organized way to store the maze level information inside the code. When the user gets to the level number that they would like to play, they click push button 2 to select (second line of LCD will prompt them to do this). The LCD instructions will also be displayed in the serial console in more details, in case the user is lost.
<br><br>After a level is selected, a maze will be displayed on the 8x8 LEDs. The maze will only include walls and a player. The end of the maze is reached when the player gets to the other side of the 8x8 LEDs screen. The user can then use the joystick to move the player up, down, right, and left. To avoid error, the joystick will be synchronized with a clock and calibrated accordingly.
<br><br>To make this digital game accessible to more people, including those with disabilities, we also plan to have a speaker play three different types of sounds for these three different scenarios:

1. Player tries to move into a wall
2. Player moves into a space that they were on before
3. Player reaches the end of the maze


In addition, we will have a DC motor spin when the player moves into a wall to mimic a “vibration” of hitting a wall.
<br>This concept is also inspired by the vibration mechanic on console controllers, which also uses two motors.
<br>When the player reaches the end of the maze, they will return to the main menu. The player can also click push __**button 1** to return to the _main menu_,__ or push __**button 2** to _reset_ the maze__ level while playing the maze.
<br>In addition, while the serial console will provide instructions in more details, it will also be used to display the current maze level, and work as a second movement controller with <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd> keyboard inputs for moving the player up, left, down, and right accordingly.
