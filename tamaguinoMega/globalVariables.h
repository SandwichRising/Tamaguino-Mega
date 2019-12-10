/* ====================
 * Most global variables are here to prevent clutter on main .ino
 *
 * Exceptions: batteryMonitor.cpp, game.cpp, menu.cpp, sounds.cpp, and tamaguinArt files 
 * The listed files contain most or all of their needed global variables.
 *
 * Sorry for the lack of consistency, but it seemed logical 
 *
 =============== */
 
//physical button inputs 
int button1State = 0;
int button2State = 0;
int button3State = 0;

   
/* ------- PET STATS ------- */

// EEPROM variables
int nameMap[11] = {0,0,0,0,0,0,0,0,0,0,0};
float hunger=100;
float happiness=100;
float health=100;
float discipline=100;
float weight=1;
float age=0;
float poopometer=0;
int poops [3] = { 0,0,0,};
int evolution = 0;
//other
int hiScore = 0;
// End of EEPROM variables


// Pet state stuff
String petName = "           ";
bool dead=false;
bool sleeping=false;

int nextPoop;


//playsplash 0 don't play, playspalash 1 play and queue music (main program loop monitors variable)
int playSplash = 1;
//main program loop monitors variable, explained in petNaming.cpp
int petNameMenuCtrl = 1;
   
   
/* ------- TIMER SETTINGS ------ */
//Enable rendering/overall game loop
//Multiple uses, setting to 0 stops game from being included in loop, 1 is normal looping, anything > 1 is a delay in ms before gameloop runs again
int canProceed = 1;

//Button Delay System
long unsigned previousButtonMillis = 0;

//Frame Rate - limits the top speed of the game on different setups, doesn't fix slow running but makes SPI screens run at I2C speed
const int gameFPS = 13;
long unsigned previousFrameMillis = 0;

//time before a button can be pressed again after pressing (in ms)
const int buttonDelay = 180;

//Notification system
int notificationLED = 0;
long unsigned notificationLEDmillis=0;
bool ledState = 0;
//move to eeprom?
bool ledEnabled = true;

//low battery warning trigger
bool lowBattery = false;

//Screen sleep settings
bool sleepState = false;

//used to increment age, since millis() resets due to poweroff/on reload and age doesn't
// +1 = 864 seconds, or 0.01 of a 24hr day
float programRuntime = 0;

//Current animation sprites go in here
const unsigned char* dinoAnimation[ANIMATIONSIZE];
