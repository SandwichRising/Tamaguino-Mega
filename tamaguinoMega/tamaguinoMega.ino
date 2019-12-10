/* ===================
 *
 * TAMAGUINO (MEGA VERSION)
 * v0.99 December 10th, 2019
 * 
 * Compiled using Arduino IDE 1.8.10 and MightyCore by MCUdude v2.0.3 board library (for ATmega1284 use)
 * 
 * Based entirely off jakob's tamaguino code @ https://alojzjakob.github.io/Tamaguino/
 *
 * This is still a WIP program, but far enough along to take a break
 * and have a complete-feeling project. I used .h extensions for support files, but that's probably not proper
 * 
 * Known Issue: there is (literally) junk art data added in to fix 
 * some sort of pointer-related issue with art frames. Removing the
 * junk art (with parts located in tamaguinoArt.cpp and .h and the evolve()
 * area in miscFunctions.cpp) causes one art frame to be distorted in the adult
 * evolutions, adding different amounts of junk art sets moves the frame distorted
 * around, adding the current amount moves it out of the real art 
 * arrays (apparently) and displays all art normally
 *
 * Uses Additional Libraries:
 * Adafruit_GFX (only version 1.1.5 due to I2C speed issues)
 * Adafruit_SSD1306 (only version 1.1.2 due to I2c speed issues)
 * Adafruit_SH1106 (unofficial, @ https://github.com/wonho-maker/Adafruit_SH1106)
  ==================== */


/* TO-DO

  Add more boards (ATmega4809)
  Dim mountains and increase starfield size at night
  Popup notifications function that can interrupt or not interrupt the game (I don't know if this is neccessary anymore)
  Add notification and animation to evolution
  Add hatching (egg) animation
  Add tutorial/instructions? Or add to tin lid?
  Audit variables for locations/global neccesity (ongoing)
  Add option for no battery? Variables and functions are used in a lot of places, menu.cpp, main .ino, etc
  [X] Add #define board settings for pullup resistors or not
  [X] move to milli() based system instead of using delay()
  [X] add battery charge indicator
  [X] Add EEPROM saving
  [X] name pet
  [X] Stat modifiers based off age (younger poops faster and loses happy slower, old is opposite, health is based off poop or no poop, no doctor)
  [X] Rearrange menu -> stats, food, game, sleep, clean, discipline, settings (no more doctor)
  [X] Move splash to main loop
  [X] Add to splash
  [X] Add led option
  [X] Display sleep timer
  [X] easy screen options using #ifdef
  [X] Add multiple evolutions
  [X] Convert main loop to a canProceed variable
  [X] Move functions
  [X] Make sound variable driven (expandable) *manually expanded instead*
  [X] Add sound function with 2 variables (funtion overload)
  [X] Fix SH1106 display (especially text windows)

  Hardware:
  [X] Fix reset-on-unplug issue - transistors? (3v charge board plus boost converter to draw enough current to stay on)

*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
// had to roll back GFX library to run I2C at full speed (1.1.5)
#include <Adafruit_GFX.h>
// had to roll back SSD1306 library to run I2C at full speed (1.1.2)
#include <Adafruit_SSD1306.h>


/* ======= BOARD SETTINGS ======= */
// screen-specific #includes and all pin settings moved to boardSettings.h

// uncomment ONE board being used for the upload, this changes the pin mapping
#define BOARD_TYPE_ATMEGA1284

//uncomment one type of pin setting, INTERNAL uses internal pullup resistors, EXTERNAL uses external pulldown resistors (you need to add these to your board)
//#define BUTTON_RESISTORS_EXTERNAL
#define BUTTON_RESISTORS_INTERNAL

// uncomment ONE SCREEN_DRIVER
//#define SCREEN_DRIVER_1306
#define SCREEN_DRIVER_1106

// uncomment ONE SCREEN_TYPE
#define SCREEN_TYPE_I2C
//#define SCREEN_TYPE_SPI

// uncomment ONE battery type
//#define BATTERY_TYPE_18650
#define BATTERY_TYPE_LIPO_PACK


#include "boardSettings.h"
#include "tamaguinoArt.h"
#include "sounds.h"
#include "globalVariables.h"
#include "miscFunctions.h"
#include "save.h"
#include "game.h"
#include "batteryMonitor.h"
#include "menu.h"
#include "petNaming.h"




/* ------ SETUP ------*/

void setup() {

  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  
#ifdef BUTTON_RESISTORS_INTERNAL
  digitalWrite(button1Pin, HIGH);
  digitalWrite(button2Pin, HIGH);
  digitalWrite(button3Pin, HIGH);
#endif

  pinMode(VOLTAGE_PIN, INPUT);
  
  pinMode(soundPin, OUTPUT);

  pinMode(ledPin, OUTPUT);

  randomSeed(analogRead(0));

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  //try to load savegame
  saveGame("load");

  // check for saved/loaded name, and display "Loading..." screen if found
  int nameCheck = 0;
  for (int i=0; i<11; i++){
    nameCheck += nameMap[i];
  }
  if (nameCheck > 0){
    petNameMenuCtrl = 4;
  }

  //load appropriate dino skin
  if (evolution==0){
    for (int i = 0; i < ANIMATIONSIZE; i++) {
      dinoAnimation[i] = dinoSmallAnimSet[i];
    }
  } else {
    for (int i=0;i<ANIMATIONSIZE;i++){
      switch (evolution) {
        case 1:
          dinoAnimation[i] = dinoLongAnimSet[i];
          break;
        case 2:
          dinoAnimation[i] = dinoSpikeAnimSet[i];
          break;
        case 3:
          dinoAnimation[i] = dinoTrikeAnimSet[i];
          break;
      }
    }
  }
  
  //randomize when the next poop will happen
  nextPoop = random(8,12);
  if (evolution > 0){
    nextPoop+=5;
  }
  
}
//end setup{}


/*------ MAIN LOOP ----- */
void loop() {

  //Update Program Timer (delay replacement)
  long unsigned currentMillis = millis();

  vSmoothing(currentMillis);
  
  display.clearDisplay();

  splashScreen(currentMillis);

  playSound(currentMillis);

  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  if (sleepState || batteryWindowOpened) {
    if (button1State == ACTIVATED || button2State == ACTIVATED || button3State == ACTIVATED){
      if (sleepState){
        sleepState = false;
      } else if (batteryWindowOpened && currentMillis > previousButtonMillis + buttonDelay){
        batteryWindowOpened = false;
      }
      previousButtonMillis = currentMillis;
    }
  }
  
  if (playSplash == 0 && petNameMenuCtrl != 0) {
    namePet();
  }
  
  // ====== MAIN PET LOOP ======
  //Frame Rate limit control starts, as well as canProceed check
  if (currentMillis > (previousFrameMillis + (1000/gameFPS)) && currentMillis > (previousFrameMillis + canProceed) && canProceed > 0){

    // get rid of milliseconds delay from canProceed if one exists
    canProceed = 1;

    //char* str = "";
    
    if(!dead){
      float ageFactor = (age + 1) * 0.0000025;
      if (evolution > 0){
        ageFactor += 0.00005;
        if (age > 30) {
          ageFactor = 0.0001275;
        }
      }
      /* -------- MODIFY PET STATS -------- */
      // different levels applied by age, evolving causes a noticeable number jump
      //sleeping
      if(sleeping){
        hunger-=0.00005;
        poopometer+=0.00005;
        if(happiness-0.0001>=0){
          happiness-=0.0001;
        }
        if (countPoops()>0) {
          health-=0.00005+countPoops()*0.0001;
        } else {
          if (health+0.0003<=100){
            health+=0.0003;
          }
        }
        if(discipline-0.0001>=0){
          discipline-=0.0001;
        }
      //not sleeping
      }else{
        //hunger at this rate varies from %100-%0 in 7h-10.5h, age 0-30 respectively
        hunger-=0.0003-(ageFactor/1.275);
        
        //need to figure out the math on this, gets more complicated with the poop making randomness
        //somewhere around 51mins to 90mins at age 0 per poop, quite a bit less when older 
        poopometer+=0.00025-(ageFactor/2);

        //happiness range fully deplete from ~14hr to 10.68hr, age 0-30 respectively
        if(happiness-0.00015-(ageFactor*0.4)>=0){
          happiness-=0.00015-(ageFactor*0.4);
        }
        // health depletes when there's poop on screen and replenishes when there isn't 
        // rate is 1 poop delpetes full %100bar @10.5h, 2@7.12h, 3@5.34h <- this is off now, added 0.00005 to ticker
        // clean replenishes full bar in around 9.5hrs
        // removing "doctor" option, health will be fully tidyness related
        if (countPoops()>0) {
          health-=0.00015+countPoops()*0.0001;
        } else {
          if (health+0.000225<=100){
            health+=0.000225;
          }
        }
        // 0 age will run out in 7 hours, 30 age will take 16.5ish hours
        if(discipline-0.0003-(ageFactor*1.35)>=0){
          discipline-=0.0003-(ageFactor*1.35);
        }
        
        //discipline-=0.02;
      }
//      age+=0.0000025;
//      new age calc
      if (currentMillis > ((programRuntime + 1.00) * 864000))
      {
          programRuntime += 1;
          age = age + 0.01;
          //Auto Saves 4 times in the dino's day
          //there has to be a better way to handle this saving check. the =='s don't seem to like comparing a float,
          //so it's been converted to int
          float ageCheck = age-floor(age);
          int ageCheckInt = ageCheck*100;
          if(ageCheckInt==0 || ageCheckInt==25 || ageCheckInt==50 || ageCheckInt==75) {
            saveGame("save");
          }
          // run out any additional age difference between canProceed and currentMillis from being locked in a canProceed 0 
          // menu, or anything like that. Prevents gaining age more than once (without stat decreases triggering) by sitting in a menu
          while (currentMillis > ((programRuntime + 1.00) * 864000)) {
            programRuntime += 1;
          }
      }
      if (evolution == 0){
        if (age>2){
          evolution=evolve();
        }
      }
  
      //diarrhea :) for testing
      //poopometer+=0.005;
  
      //health-=1;
      //health-=countPoops()*0.0001;
      //health-=countPoops()*0.05;
  
      //Make Poop
      if(poopometer>=nextPoop){
        poopometer=countPoops();
        poops[round(poopometer)]=random(20,display.width()+32);
        queueSound(200,50,0);
        poopometer=0;
        nextPoop = random(8,12);
        if (evolution > 0){
          nextPoop+=5;
        }
      }
  
      if((hunger>19.99975 && hunger<20.00025) || (happiness>19.9998 && happiness<20.0002) || (health>19.9999 && health<20.0001)){
        queueSound(200,50,0);
      }
    
      
      if(countPoops()>0){
        notificationLED+=1;  
      }
      if(hunger<=20 || happiness<=20 || health<=20){
        notificationLED+=2;  
      }
//      if(hunger>20 && countPoops()==0 && happiness>20 && health>20){
//        notification=false;  
//        digitalWrite(ledPin,LOW);
//      }
  
      if(hunger<=0 || health<=0 || happiness<=0){
        dead=true;
        queueSound(500,500,550);
        queueSound(400,500,550);
        queueSound(300,600,0);
      }
      
      display.setCursor(0,0);
    
      //Take actions based on captured button inputs
      menuButtonActions(currentMillis);
      gameButtonActions(currentMillis);
    
    
      /* ------- SCENERY AND WALKING ------- */
      // Objects that are always drawn - ground and sky (unless menu is open)

      //SKY
      //separated movement and draw, to turn off drawing when menu is open
      //sun and moon move
      sunXPos+=0.1;
      if(sunXPos>display.width()+2*sunRadius){
        sunXPos=-2*sunRadius;
        sunOrMoon=!sunOrMoon;
      }      
      if(sleeping){
        sunOrMoon=true;
      }
      //cloud move
      cloud1XPos-=0.3;
      if(cloud1XPos<-cloud1Width){
        cloud1XPos=display.width()+cloud1Width;
      }

      //draw all sky elements if menu is closed
      if (!menuOpened){
        //daytime draw
        if(!sunOrMoon){
          display.fillCircle(sunXPos,2*sunRadius,sunRadius,WHITE);
        //nighttimevdraw
        }else{
          display.fillCircle(sunXPos,2*sunRadius,sunRadius,WHITE);
          display.fillCircle(sunXPos-moonShadow,2*sunRadius,sunRadius,BLACK);
          //if(walkPos == 5){
          if(round(cloud1XPos) % 5 == 0){
            for(int i=0;i<6;i++){
              stars[i][0]=random(0,display.width());
              stars[i][1]=random(0,10);
            }
          }else{
            for(int i=0;i<6;i++){
              display.drawPixel(stars[i][0],stars[i][1],WHITE);
            }
          }
        }

        //cloud draw
        display.drawBitmap(cloud1XPos, 5, cloud2 , cloud1Width, 5, WHITE);
    
        //mountains draw
        display.drawBitmap(0, 7, mountains , 128, 16, WHITE);
      }
    
      //Walk and Move Ground Perspective
  
      //Draw objects for the game mode
      if(game){  
        gameRender();
      }else{
        /* ------ NO GAME -----*/
        //Draw objects if the game isn't running

        //draw grass (ground)
        for(int i=0;i<2*display.width()/4;i++){
          display.drawBitmap(-walkXPos+i*8, 50, grass , 8, 6, WHITE);
        }
        // draw poops
        for(int i=0; i<3; i++){
          if(poops[i]>0){
            display.drawBitmap(-walkXPos+poops[i], 44, poop , 16, 6, WHITE);
          }
        }
        //draw front grass
        for(int i=0;i<2*display.width()/16;i++){
          display.drawBitmap(-grassXPos+i*32, 56, grass_front , 32, 8, WHITE);
        }
        //draw trees
        display.drawBitmap(-treesXPos, 23, trees , 112, 20, WHITE);
        //moved down to get rid of pet transparency
        //draw pet
        if(!sleeping){
          display.drawBitmap(walkXPos, 26, dinoAnimation[walkPos+walkDirOffset] , 48, 24, WHITE);
        }else{
          display.drawBitmap(walkXPos, 29, dinoAnimation[walkPos+walkDirOffset] , 48, 24, WHITE);          
          if(walkRight){
            if(round(cloud1XPos) % 3 ==0){
              display.setCursor(walkXPos+48,36);
              display.print(F("Z"));
            }else{
              display.setCursor(walkXPos+46,38);
              display.print(F("z"));
            }
          }else{
            if(round(cloud1XPos) % 3 ==0){
              display.setCursor(walkXPos-4,36);
              display.print(F("Z"));
            }else{
              display.setCursor(walkXPos-2,38);
              display.print(F("z"));
            }
          }
        }
        if(walkRight){
          if(!sleeping){
            walkXPos+=1;
            grassXPos+=2;
            treesXPos+=0.5;
          }
          if(walkXPos>80){walkRight=false; walkDirOffset=3;}
        }else{
          if(!sleeping){
            walkXPos-=1;
            grassXPos-=2;
            treesXPos-=0.5;
          }
          if(walkXPos<0){walkRight=true; walkDirOffset=0;}  
        }
          
        if(!sleeping){
          if(walkAnimReverse){
            --walkPos;
            if(walkPos==-1){walkPos=0; walkAnimReverse=false;}
          }else{
            ++walkPos;
            if(walkPos==3){walkPos=2; walkAnimReverse=true;}
          }
        }
      }
    
      //Render menu elements
      menuRender();

      // GAME OVER
      if(gameOver){
        display.fillRect(15,17,98,43,BLACK);
        display.drawRect(16,18,96,41,WHITE);
        display.fillRect(16,18,96,13,WHITE);
        display.setCursor(36,21);
        display.setTextColor(BLACK);
        display.println(F("GAME OVER"));
        display.setTextColor(WHITE);
        display.setCursor(21,35);
        if(newHiScore){
          display.println(F("NEW HI-SCORE!"));
          display.setCursor(21,46);
        }else{
          display.println(F("SCORE:"));
          display.setCursor(21,46);
        }
        display.println(score);
      }
       
    }else{
      //dead...
//      display.clearDisplay();

      saveGame("erase");
      display.setCursor(0,0);
      display.setTextColor(WHITE);
      display.print(petName);
      display.println(F("\nhas died...\n\nPress button 1\nto restart"));
      display.display();
  
      if(button1State==ACTIVATED){
        if(soundEnabled){
          tone(soundPin,300,80);
          delay(200);
        }
        noTone(soundPin);
        asm volatile ("  jmp 0");
      }
    }
    
    //for controlling frame rate
    previousFrameMillis = currentMillis;
  }
  //show LED notifications
  notificationLEDcheck(currentMillis);

  // use clearDisplay for powersave, input capture code above prevents actions from being taken 
  // when pressing a button to wake up
  if (currentMillis > previousButtonMillis + sleepTimer[sleepIndex] && sleepTimer[sleepIndex] > 0){
    sleepState = 1;
    display.clearDisplay();
  }
  
  //debugVoltageDisplay(2);

  //battery low notify - shows even when sleeping
  if (lowBattery) {
    display.fillRect(29,55,69,9,BLACK);
    display.setCursor(31,56);
    display.print("LOW BATTERY");
  }
  
  //show generated image from loop
  display.display();
}
