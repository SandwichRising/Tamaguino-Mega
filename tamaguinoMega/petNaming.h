/* ==================
 *
 * Pet Naming Functions
 *
 * Checks to see if the pet is named, if not traps the user in menu interface to name pet.
 * Also invoked by using the "rename pet" action (sets petNameMenuCtrl = 3). 
 * Also displays the "Loading..." screen (if setup{} detects a pet name exists after attempting EEPROM load).
 *
 * Note: This is one of the only program functions that traps the user instead of continuing to
 * cycle through the full program loop. This is to prevent the creation of global variables
 * that are only needed for pet naming, like the letter arrays and millis checks. Because of this,
 * playSound() had to be added in as well as display.commands. Screen sleeping intentionally left out.
 *
 * Usage: program monitors the petNameMenuCtrl global variable for non-zero state, and this 
 * menu system is triggered if !=0.
 * petNameMenuCtrl values:
 * 0: do not diplay naming menu
 * 1: "What is your pet's name?" name entry screen brought up
 * 2: Confirm pet name screen
 * 3: "Rename Pet?" confirmation screen (yes goes to 1, no goes to 0)
 * 4: "Loading..." bar screen (then goes to 0)
 *
 ==================== */


/* ------ MENU CONSTRUCTOR ------ */
void namePet() {
  
  long unsigned previousNamePetMillis = 0;
  int timeFactor = 0; //for loading screen

  #define ARRAY_LENGTH 27
  const char firstLetter[ARRAY_LENGTH] = {' ','A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  const char otherLetters[ARRAY_LENGTH] = {' ','a','b','c','d','e','f','g','h','i','j',
    'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
  int letterIndex = 0;
  int petNameIndex = 0;
  
  while (petNameMenuCtrl != 0){
    
    long unsigned currentMillisFunc = millis();

    playSound(currentMillisFunc);
    
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
    button3State = digitalRead(button3Pin);
    
    display.clearDisplay();

    //diplay and fill textbox and display options
    display.drawRect(0,10,76,30,WHITE);
    if (petNameMenuCtrl == 1){
      
      display.setCursor(3,15);
      display.println("What is your");
      display.setCursor(6,27);
      display.println("pet's name?");
      
      display.setCursor(77,0);
      display.println("Change->");
      display.setCursor(83,26);
      display.println("Next ->");
      display.setCursor(83,52);
      display.println("Done ->");
  
      if (currentMillisFunc > previousNamePetMillis + 800) {
        display.drawLine(3+6*petNameIndex,56,9+6*petNameIndex,56, WHITE);
        if (currentMillisFunc > previousNamePetMillis + 1600) {
          previousNamePetMillis = currentMillisFunc;
        }
      }
      
    } else if (petNameMenuCtrl == 2){
      
      display.setCursor(10,22);
      display.println("Name pet?");
      display.setCursor(83,0);
      display.println("Keep ->");
      display.setCursor(83,52);
      display.println("Back ->");
      
    } else if (petNameMenuCtrl == 3){
          
      display.setCursor(4,22);
      display.println("Rename pet?");
      display.setCursor(83,0);
      display.println("Yes ->");
      display.setCursor(83,52);
      display.println(" No ->");
      
    }
    
    /* ------- BUTTON PRESS ACTIONS ------- */
    
    /* ------- BUTTON 1 - CHANGE/CONFIRM/YES ------- */   
    if(button1State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
      // MENU
      queueSound(300,80,0);
      if(petNameMenuCtrl==1){
        if (letterIndex<ARRAY_LENGTH-1){
          letterIndex += 1;
        } else{
          letterIndex = 0;
        }
        nameMap[petNameIndex] = letterIndex;
      } else if (petNameMenuCtrl==2) {
        petNameMenuCtrl = 0;
      } else if (petNameMenuCtrl==3){
        petNameMenuCtrl = 1;
        for (int i=0;i<11;i++){
          nameMap[i] = 0; 
        }
        petName = "           ";
      }
      previousButtonMillis = currentMillisFunc;
    }
    /* ------- BUTTON 2 - NEXT/NOTHING/NOTHING ------- */
    if(button2State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
      queueSound(600,80,0);
      if (petNameMenuCtrl==1){
        nameMap[petNameIndex] = letterIndex;
        if (petNameIndex<10){
          petNameIndex += 1;
        } else {
          petNameIndex = 0;
        }
        letterIndex = nameMap[petNameIndex];
      }
      previousButtonMillis = currentMillisFunc;
    }
    /* ------- BUTTON 3 - DONE/BACK/NO ------- */
    if(!game && button3State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
      queueSound(1000,80,0);
  
      if(petNameMenuCtrl==1){
        petNameMenuCtrl += 1;
      } else if (petNameMenuCtrl==2){
        petNameMenuCtrl -= 1;
      }else if (petNameMenuCtrl==3){
        petNameMenuCtrl = 0;
      }
      previousButtonMillis = currentMillisFunc;   
    }
    /* ------- BUTTON END -------- */

  
    //letter changing
    if (petNameMenuCtrl == 1){
      if (petNameIndex == 0){
        petName.setCharAt(petNameIndex,firstLetter[letterIndex]);
      } else {
        petName.setCharAt(petNameIndex,otherLetters[letterIndex]);
      }
    }
    
    //display pet's name in box
    display.drawRect(0,45,76,14,WHITE);
    /* Method is not really necessary, keeping for posterity
    for (int i = 0; i < 10; i++){
      display.setCursor(4+6*i, 48);
      display.print(petName.charAt(i));
    }*/
    display.setCursor(4,48);
    display.print(petName);
    
    //load a saved name
    if (petNameMenuCtrl == 4 || petNameMenuCtrl == 5){
      display.clearDisplay();
      if (petNameMenuCtrl ==4){
        for (int i = 0; i < 11; i++){
          if (i == 0){
            petName.setCharAt(i,firstLetter[nameMap[i]]);
          } else {
            petName.setCharAt(i,otherLetters[nameMap[i]]);
          }
        }
        petNameMenuCtrl = 5;
      }
      if (currentMillisFunc > 50*timeFactor + 4400){
        if (timeFactor < 10){
          timeFactor += 1;
        }
        display.setCursor(35,16);
        display.print("Loading...");
        display.drawRect(7,30,110,10,WHITE);
        display.fillRect(7,30,11*timeFactor,10,WHITE);
        if (currentMillisFunc > 5500){
          for (int x = 1000; x < 1400; x +=100){
            queueSound(x,80);
          }
          petNameMenuCtrl = 0;
        }
      }
    }
    
    
    display.display();
  }
}
