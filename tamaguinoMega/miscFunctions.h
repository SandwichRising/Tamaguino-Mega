/* ===================
 *  
 *  Smaller functions placed here for 
 *  use globally, to reduce clutter in main .ino
 *  
 =================== */

 // evolve(); - evolves pet to an adult skin and changes the art out
 // countPoops(); - counts the poops
 // notificationLEDcheck(); - checks notificationLED variable to control LED and on-screen notifications
 // splashScreen(); - diplays the game startup splash art

// ============= evolve() ==============
//Evolution Handler
int evolve(){
  for (int x = 1000; x < 1400; x +=100){
    queueSound(x,80);
  }
  int randEvolution = random(1,4);
  for (int i=0;i<ANIMATIONSIZE;i++){
    switch (randEvolution) {
      case 1:
        dinoAnimation[i] = dinoLongAnimSet[i];
        break;
      case 2:
        dinoAnimation[i] = dinoSpikeAnimSet[i];
        break;
      case 3:
        dinoAnimation[i] = dinoTrikeAnimSet[i];
        break;
      //junk data to fix sprite issues (some frames of actual 3 skins dont work without inclusion)
      case 4:
        dinoAnimation[i] = dinoJunkAnimSet[i];
        break;
      case 5:
        dinoAnimation[i] = dinoJunk2AnimSet[i];
        break;
      case 6:
        dinoAnimation[i] = dinoJunk3AnimSet[i];
        break;       
      case 7:
        dinoAnimation[i] = dinoJunk4AnimSet[i];
        break;
      case 8:
        dinoAnimation[i] = dinoJunk5AnimSet[i];
        break;
    }
  }
  return randEvolution;
}


// ============ countPoops() ==================
int countPoops(){
  int poopsCnt = 0;
  for(int i=0; i<3; i++){
    if(poops[i]>0){
      ++poopsCnt;
    }
  }
  return poopsCnt;
}


// ========== notificationLEDchek() ================
void notificationLEDcheck(long unsigned currentMillisFunc) {
  //display notification
  
  if(notificationLED > 0){
    int notificationDelay = 1000;
    int notificationTimeon = 1000;
    if (notificationLED > 1){
      notificationTimeon = 100;
      notificationDelay = 300;
    }
    if (ledState == 0 && currentMillisFunc > notificationLEDmillis + notificationDelay) {
      display.fillRect(117,28,11,11,WHITE);
      display.setTextColor(BLACK);
      if(!dead && ledEnabled){
        digitalWrite(ledPin,HIGH);
      }
      notificationLEDmillis = millis();
      ledState = 1;

    } else if (ledState == 1 && currentMillisFunc > notificationLEDmillis + notificationTimeon) {
      display.setTextColor(WHITE);
      digitalWrite(ledPin,LOW);
      notificationLEDmillis = millis();
      ledState = 0;
    } else {
      display.setTextColor(WHITE);
    }
    display.drawRect(117,28,11,11,WHITE);
    display.setCursor(120,30);
    display.println(F("!"));

  } else {
    digitalWrite(ledPin,LOW);
  }
  notificationLED = 0;
}


// ============ splashScreen() ===============
void splashScreen(long unsigned currentMillisFunc){
  if (playSplash > 0){
    if (playSplash == 1 || playSplash == 2) {
      canProceed = 0;
      display.setTextColor(WHITE);
      display.setCursor(0,10);
      //display.println(F("jakobdesign presents")); 
      display.print(F(" jakobdesign presents")); 
      display.drawBitmap(15, 24, splash1 , 48, 26, WHITE);
      display.drawBitmap(48, 24, splash2 , 80, 40, WHITE);
      if (playSplash == 1){
        //splash tone
        queueSound(500,200);
        queueSound(1000,200);
        queueSound(700,200);
        queueSound(1100,200);
        playSplash = 2;
      }
    }
    if (currentMillisFunc > 2200){
      playSplash=3;
    }
    if (playSplash == 3){
      display.drawBitmap(0, 2, splash3, 128, 64, WHITE);
      display.setCursor(112,48);
      display.print(F("by")); 
      display.setCursor(90,55);
      display.print(F("Conrad")); 
      if (currentMillisFunc > 4400){
        playSplash = 0;
        canProceed = 1;
      }
    }
  }
}
