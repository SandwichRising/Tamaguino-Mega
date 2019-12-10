/* =======================================
 *
 * Game Mode
 *
 * Code to render the game mode and detect
 * button input during the game. 
 *
 ======================================= */

//game 
bool game=false;
bool paused=false;
bool gameOver=false;
int score = 0;
//moved to eeprom area
//int hiScore = 0;
int level=0;
bool newHiScore = false;
bool jumping=false;
bool jumpUp=true;
int jumpPos=0;
bool obstacle1show = false;
bool obstacle2show = false;
int obstacle1XPos = 0;
int obstacle2XPos = 0;

        
/* ------ GAME VISUALS -----*/
void gameRender(){
  level=round(score/10);

  if(jumping && !gameOver && !paused){
    if(jumpUp){
      jumpPos=jumpPos+1+level;
      if(jumpPos>=12){
        jumpUp=false;
      }
    }else{
      //jumpPos--;
      jumpPos=jumpPos-1-level;
      if(jumpPos<=0){
        jumpUp=true;
        jumping=false;
        queueSound(100,50,0);
        score+=1;
      }
    }
  }


  if(!gameOver && !paused){
    if(walkAnimReverse){
      walkPos-=1;
      if(walkPos==-1){walkPos=0; walkAnimReverse=false;}
    }else{
      walkPos+=1;
      if(walkPos==3){walkPos=2; walkAnimReverse=true;}
    }

    walkXPos+=2;
    grassXPos+=4;
    treesXPos=treesXPos+1+level;
    obstacle1XPos=obstacle1XPos+2+level;
    obstacle2XPos=obstacle2XPos+2+level;

    //hit obstacle detection
    if(!jumping && 
        (
          (obstacle1show && display.width()-obstacle1XPos>=20 && display.width()-obstacle1XPos<=46)
          || 
          (obstacle2show && display.width()-obstacle2XPos>=20 && display.width()-obstacle2XPos<=46)
        )
      ){
      gameOver=true;
      jumping=true;
      jumpPos=-2;
      queueSound(500,40,50);
      queueSound(350,40,50);
      queueSound(200,60,0);

      if(score>hiScore){
        hiScore=score;
        newHiScore=true;
      }
      if(happiness+5+ceil(score/2)<100){
        happiness+=5+ceil(score/2);
      }else{
        happiness=100;
      }
      health-=1;
      if(weight-score*0.0025>5){
        weight-=score*0.0025;
      }
    }
  }
  
  if(walkXPos==display.width()){
    walkXPos=0;
  }
  if(grassXPos==display.width()){grassXPos=0;}
  if(treesXPos==display.width()){treesXPos=-128;}
  
  //draw front grass
  for(int i=0;i<display.width()/16+1;i++){
    display.drawBitmap(-grassXPos+i*32, 60, grass_front , 32, 8, WHITE);
  }
  //draw trees
  display.drawBitmap(-treesXPos, 23, trees , 112, 20, WHITE);

  //draw pet
  if(jumping){
    display.drawBitmap(10, 26-jumpPos, dinoAnimation[6] , 48, 24, WHITE);
  }else{
    display.drawBitmap(10, 26, dinoAnimation[walkPos] , 48, 24, WHITE);
  }
  
  //draw grass
  for(int i=0;i<display.width()/4+1;i++){
    display.drawBitmap(-walkXPos+i*8, 50, grass , 8, 6, WHITE);
  }

  // obstacles 1
  if(obstacle1XPos-16>=display.width()){
    obstacle1XPos=0;
    obstacle1show=false;
  }
  if(!obstacle1show && random(1,10)==1 && obstacle2XPos > 40){
    obstacle1show = true;
    obstacle1XPos=0;
  }
  if(obstacle1show){
    display.drawBitmap(display.width()-obstacle1XPos, 44, obstacle1 , 16, 6, WHITE);
  }

  // obstacles 2
  if(obstacle2XPos-16>=display.width()){
    obstacle2XPos=0;
    obstacle2show=false;
  }
  if(!obstacle2show && random(1,10)==1 && obstacle1XPos > 40){
    obstacle2show = true;
    obstacle2XPos=0;
  }
  
  if(obstacle2show){
    display.drawBitmap(display.width()-obstacle2XPos, 44, obstacle2 , 16, 6, WHITE);
  }


  if(!gameOver){
    display.setCursor(0,56);
    display.setTextColor(WHITE);
    display.print(F("lvl: "));
    display.print(level);
    display.setCursor(64,56);
    display.setTextColor(WHITE);
    display.print(F("pts: "));
    display.print(score);
  }

  if(paused && round(cloud1XPos)%2==0){
    display.fillRect(24,11,80,15,BLACK);
    display.fillRect(25,12,78,13,WHITE);
    display.setCursor(47,15);
    display.setTextColor(BLACK);
    display.println(F("PAUSED"));
  }
}

        
/* ------ GAME BUTTONS -----*/
void gameButtonActions(long unsigned currentMillisFunc){ 
  
  /* ------- BUTTON 1 - GAME JUMP ------- */
  if(game && button1State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
    // JUMP IN GAME
    if(!jumping && !paused){
      queueSound(200,50,0);
      jumping=true;
    }
    previousButtonMillis = currentMillisFunc;
  }
  /* ------- BUTTON 2 - GAME PAUSE ------- */
  if(game && !gameOver && button2State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
    paused=!paused;
    queueSound(600,80,80);
    previousButtonMillis = currentMillisFunc;
  }
  /* ------- BUTTON 3 - BACK ------- */
  if((game || gameOver) && button3State==ACTIVATED && currentMillisFunc > previousButtonMillis + buttonDelay){
      queueSound(1000,80,0);
    
      walkPos=0;
      walkXPos=0;
      walkAnimReverse=false;
      walkRight=true;
      walkDirOffset=0;
      treesXPos=-20;
      grassXPos=0;
      obstacle1show=false;
      obstacle2show=false;
      jumping=false;
      jumpPos=0;
      jumpUp=true;
      game=false;
      score=0;
      newHiScore=false;
      gameOver=false;
      level=0;
      paused=false;
      
      previousButtonMillis = currentMillisFunc;   
    
  }
}
