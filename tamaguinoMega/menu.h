/* ======================
 * 
 * Main Menu Rendering
 * 
 * Menu items can be added to the MainMenu array, and will appear in the cycle-thru list as soon as being added, 
 * as either a main menu item or a subitem: {main menu item, submenu item 1, submenu item 2, submenu item 3,NULL}, 
 * mainMenu is set to be fairly plug-and-play, with maybe some array size adjustments. But restructuring of the 
 * button actions, display setting, etc is not needed, just add in what you want to be able to do.
 * 
 * Main menu items with no subitems will trigger themselves (their setting) as actions with button 2 press
 * mainMenu items with submenu items listed will switch to the submenu when triggered instead of performing an action.
 * mainMenu array numbers may need adjusted to expand or collase array size to accomodate adding items, if going beyond or below the currently set constraints.
 *
 * Anything added to the manMenu array is considered a 'setting' value to be cycled thru, triggering a setting with button 2 will turn that
 * setting into an 'action' (action = setting;)
 * 
 * Below the mainMenu array, adding an 'action' 'if' detection statement for the setting's name will make the program perform whatever action needs assigned to a mainMenu setting.
 *
 * Below the action detection, adding a 'setting' detection 'if' statement will render anything additional the menu selection needs to display (like a health bar),
 * or a description to display for a mainMenu setting with no subitems. Default will just display whatever word is added to the mainMenu array, in either the upper area (main menu items)
 * or the submenu area (submenu items).
 *
 * Below that, but still inside the "if (setting != ""){}", additional popup windows for submenu selections can be added to render on top of everything.
 *
 ===================== */


#include <Arduino.h>

// use 9 to include dev and doctor options, if making 7 comment out dev and doctor options in menu array also
#define MENUSIZE 7
#define STRING_SIZE 12

const char mainMenu[MENUSIZE][9][STRING_SIZE] = {
  {"stats","hunger","happiness","health","discipline ","weight","age","battery",NULL},
  {"food","apple","steak","water",NULL},
  {"game",NULL},
  {"sleep",NULL},
  {"clean",NULL},
  // {"doctor",NULL}, // No longer needed as health goes up and down based on if poops are on screen or not
  {"discipline",NULL},
  {"settings","sound","LED","screen dim","manual save","rename pet","erase save",NULL},
  // comment out next line if not using dev options and reduce MENUSIZE by 1
  //{"dev options","evolve","hp down","poop",NULL},
};


long unsigned sleepTimer[5] = {0, 300000, 900000, 1800000, 3600000};
const char sleepTimerTimes[5][8] = {"off", "5 mins", "15 mins", "30 mins", "60 mins"};

// Menu Variables
bool menuOpened = false;
bool batteryWindowOpened = false;
int menu=0;
int subMenu=1;
bool menuDepth=false;
String setting = "";
String action = "";
int sleepIndex = 3;


// Menu Functions
void drawBar(float value){
  display.fillRect(72,19,48*value/100,3,WHITE);
}

void resetPoops(){
  for(int i=0; i<3; i++){
    poops[i]=0;
  }
}

/* ------- BUTTON PRESS ACTIONS ------- */
void menuButtonActions(long unsigned currentMillisFunc){   
      
  /* ------- BUTTON 1 - MENU ------- */   
  if(!game && button1State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
    // MENU
    queueSound(300,80,0);
    // if menu is closed, open it
    if(!menuOpened){
      menuOpened=true;
      menu=0;
      if(mainMenu[menu][1][0]==NULL){
        subMenu=0;
      } else {
        subMenu=1;
      }
      setting = mainMenu[menu][subMenu];
    // if menu is opened...
    }else{
      // ... and submenus are arlready selected, cycle them
      if(menuDepth){
        if(mainMenu[menu][subMenu+1][0]==NULL){
          subMenu=1;
        }else{
          ++subMenu;
        }
        setting = mainMenu[menu][subMenu];
      // ... cycle main menus
      }else{
        if(menu==MENUSIZE-1){
          menu=0;
        }else{
          ++menu;
        }
        // if there are no submenu settings, use the main menu name for the setting
        if(mainMenu[menu][1][0]!=NULL){
          subMenu=1;
        } else {
          subMenu=0;
        }
        setting = mainMenu[menu][subMenu];
      }
    }        
    previousButtonMillis = currentMillisFunc;
  }
  /* ------- BUTTON 2 - SELECT ------- */
  if(!game && button2State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
      queueSound(600,80,0);
      
      if(menuOpened){
        if(mainMenu[menu][1][0]==NULL){
          action = setting;
        }
        if(menuDepth){
          action = setting;
        }
        if(mainMenu[menu][1][0]!=NULL){
          menuDepth=true;
        }
        
      }else{
        action = "";
/* Autoopen Stats stuff       
        menuOpened=true;
        menuDepth=true;
        subMenu=1;
        menu=6;
        action=701;
        setting=701;
*/
      }
    
    previousButtonMillis = currentMillisFunc;
  }
  /* ------- BUTTON 3 - BACK ------- */
  if(!game && button3State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
    queueSound(1000,80,0);

    if(!menuDepth){
      menuOpened=false;
//      menu=0;
      setting = "";
    }else{
      menuDepth=false;
    }
    action = "";
    previousButtonMillis = currentMillisFunc;   
  }
}

void menuRender(){
  /* ------- MENUS AND ACTIONS ------- */
  
  //close menu if game is running
  if (menuOpened && game){
    menuOpened=false;
    setting = "";
  }
  
  //render menu
  if(menuOpened){
    display.fillRect(0,0,display.width(),30,BLACK);
    display.drawRect(0,0,display.width(),29,WHITE);
    display.fillRect(1,1,display.width()-2,27,BLACK);
    display.drawRect(0,0,display.width(),12,WHITE);
    display.setCursor(8,2);
    display.setTextSize(1);
display.setTextColor(WHITE);
    if(menuDepth){
//      display.fillRect(0,0,display.width(),12,WHITE);
      display.fillRect(1,18,1,5,WHITE);
      display.fillRect(2,19,1,3,WHITE);
      display.fillRect(3,20,1,1,WHITE);
//      display.setTextColor(BLACK,WHITE);
    }else{
      display.fillRect(1,3,1,5,WHITE);
      display.fillRect(2,4,1,3,WHITE);
      display.fillRect(3,5,1,1,WHITE);
//      display.setTextColor(WHITE);
    }
    String oneItem = mainMenu[menu][0];
    if (oneItem=="stats"){
      oneItem = petName;
    }
    display.println(oneItem);
    if(subMenu){
      display.setTextColor(WHITE);
      display.setCursor(8,16);
      String subItem = mainMenu[menu][subMenu];
      display.println(subItem);
    }
  }

  //do actions
  if(action!=""){
    
    if (action == "battery"){
      batteryWindowOpened = true;
    }

    if((action=="apple" || action=="steak" || action=="water") && !sleeping && random(1,(11-round(discipline/10)))==1 ){
      //95-100 discipline = 100% chance to feed
      //85-95 discipline = 50% chance
      //75-85 discipline = 33.33% chance
      //65-75 discipline = 25% chance
      //55-65 discipline = 20% chance
      //45-55 discipline = 16.67% chance
      //35-45 discipline = 14.28% chance
      //25-35 discipline = 12.5% chance
      //15-25 discipline = 12.5% chance
      //5-15 discipline = 10% chance
      //0-5 discipline = 9% chance

      //animate eating
      display.fillRect(0,0,display.width(),display.height(),BLACK);
      for(int j=0;j<3;j++){
        for(int i=0; i<4; i++){
          display.clearDisplay();
            if (action ==  "apple") {
              //apple
              display.drawBitmap(50,40,apple,24,24,WHITE);
              if(j>0) display.fillCircle(76,54,12,BLACK);
              if(j==2) display.fillCircle(47,55,12,BLACK);
              }
            if (action ==  "steak") {
              //steak
              display.drawBitmap(50,40,steak,24,24,WHITE);
              if(j>0) display.fillCircle(76,59,13,BLACK);
              if(j==2) display.fillCircle(60,63,13,BLACK);
              }
            if (action ==  "water") {
              //water ripples
              display.drawCircle(80,55,1+1*i,WHITE);
              display.drawCircle(80,55,5+2*i,WHITE);
              display.drawCircle(80,55,10+4*i,WHITE);
            }
          display.drawBitmap(80,24,eating[i],48,40,WHITE);
          delay(150);
          display.display();
        }
      }
      
      
        //apple
        if (action ==  "apple") {
          if(hunger+10>100){
            hunger=100;
            weight+=0.1;
          }else{
            hunger+=10;
          }
          if(health+1<=100){
            health+=1;
          }
          poopometer+=0.02;
          }
        //steak
        if (action ==  "steak") {
          if(hunger+20>100){
            hunger=100;
            weight+=0.2;
          }else{
            hunger+=20;
            weight+=0.1;
          }
          if(health-1>0){
            health-=1;
          }
          poopometer+=0.05;
          }
        //water
        if (action ==  "water") {
          if(hunger+5<=100){
            hunger+=5;
          }
          poopometer+=0.01;
          }
      
    }else{
      if(action=="apple" || action=="steak" || action=="water"){
        for(int i = 0; i < 3; i++){
          queueSound(100,100,150);
        }
      }
    }

    if (action == "game") {
      //game
      if(!sleeping && health>20){
        game=true;
        walkPos=0;
        walkXPos=0;
        walkAnimReverse=false;
        walkRight=false;
        walkDirOffset=2;
        treesXPos=-20;
        grassXPos=0;
      } else {
        for(int i = 0; i < 3; i++){
          queueSound(100,100,150);
        }
      }
    }
    if (action == "sleep") {
      //sleep
      sleeping=!sleeping;
      if(!sleeping) {sunOrMoon=false;}else{
        for(int i=0;i<6;i++){
          stars[i][0]=random(0,display.width());
          stars[i][1]=random(0,10);
        }
      }
    }
    if (action == "clean") {
      //bath
        resetPoops();
    }
    if (action == "doctor") {
      //doctor
      if(health<60){
        health=100;
        for(int i=0;i<5;i++){
          display.clearDisplay();
          if(i%2!=0){
            display.fillRect(32,23,64,16,WHITE);
            display.fillRect(56,0,16,64,WHITE);
          }
          display.display();
          delay(300);
        }
      } else {
        for(int i = 0; i < 3; i++){
          queueSound(100,100,150);
        }
      }
    }
    if (action ==  "discipline") {
      //discipline
      if(action=="discipline" && !sleeping){
        if(discipline+12<=100){
          discipline+=12;
        }else{
          discipline=100;
        }
        if(happiness-3>0){
          happiness-=3;
        }
        delay(150);
        for(int i=0;i<5;i++){
          queueSound(200*i,100,0);
          display.setCursor(100+3*i,32);
          display.print(F("!"));
          display.display();
          delay(150);
        }
      }
    }
    
    if (action ==  "sound") {
      soundEnabled=!soundEnabled;  
    }     
    if (action ==  "LED") {
      ledEnabled=!ledEnabled;  
    }    
    if (action ==  "screen dim") {
      sleepIndex += 1;
      if (sleepIndex > 4) {
        sleepIndex = 0;  
      }
    }    
    if (action ==  "rename pet") {
      petNameMenuCtrl = 3;  
    } 
    if(action=="manual save"){
      saveGame("save");
    }
    if(action=="erase save"){
      saveGame("erase");
    }

      
    if (action ==  "evolve") {
           evolution=evolve();  
    }
    if (action ==  "devolve") {
           evolution=0;  
    }
    if (action ==  "hp down") {
           health -= 20;  
    }
    if (action ==  "poop") {
           poopometer = nextPoop;  
    }
  
    action = "";
  }

  //display settings
  if(setting != "" && !game){
    display.setCursor(8,16);
    if(setting=="game"){
      display.println(F("increase happiness"));
    }
    if(setting=="sleep"){
      if (!sleeping){
        display.println(F("get some rest"));
      } else {
        display.println(F("wake up"));
      }
    }
    if(setting=="clean"){
      display.println(F("keep it healthy"));
    }
    if(setting=="doctor"){
      display.println(F("when health is bad"));
    }
    if(setting=="discipline"){
      display.println(F("get smarter"));
    }
    if(setting=="hunger" || setting=="happiness" || setting=="health" || setting=="discipline " || setting=="battery"){
      display.drawRect(70,17,52,7,WHITE);
    }
    if(setting=="hunger"){
      drawBar(hunger);
    }
    if(setting=="happiness"){
      drawBar(happiness);
    }
    if(setting=="health"){
      drawBar(health);
    }
    if(setting=="discipline "){
      drawBar(discipline);
    }
    if(setting=="battery"){
      drawBar(batteryLevel);
    }
    if(setting=="weight" || setting=="age" || setting=="sound" || setting=="LED"){
      display.setCursor(55,16);
    }
    if(setting=="weight"){
      //display.setCursor(80,16);
      display.print(weight,1);
      display.println(F(" t"));
    }
    if(setting=="age"){
      display.print(age,2);
      display.println(F(" days"));
    }
    if(setting=="sound"){
      if(soundEnabled){
        display.println(F("on"));
      }else{
        display.println(F("off"));
      }
    }
    if(setting=="LED"){
      if(ledEnabled){
        display.println(F("on"));
      }else{
        display.println(F("off"));
      }
    }
    if(setting=="screen dim"){
      display.setCursor(75,16);
      display.println(sleepTimerTimes[sleepIndex]);
    }
    
    
    if(setting=="evolve"){
      display.println(F("evolve pet"));
    }    
    
    // Submenu Triggered Popup windows go here to display on top of everything
    
    // display battery popup window if option is turned on
    if (batteryWindowOpened) {
      batteryWindow();
    }
  }
}
