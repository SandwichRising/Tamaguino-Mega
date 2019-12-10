
#include <Arduino.h>
#include "tamaguinoArt.cpp"

/* ------ SETTINGS ------ */

//walking
int walkPos=0;
int walkXPos=0;
bool walkAnimReverse=false;
bool walkRight=false;
int walkDirOffset=2;
    
//ground
int grassXPos=0;
float treesXPos=-20;

//sky
float couldsXPos=0;
const int sunRadius=3;
bool sunOrMoon = false;
const int moonShadow=2;
float sunXPos=-2*sunRadius;

//clouds
const int cloud1Width=32;
float cloud1XPos=display.width()+cloud1Width;

//stars
int stars [6][2];




/* ---- ANIMATION SETS ----- */

/* should be able to add more animations without changing existing code
   just add to the end of the AnimSet arrays, add a define before ANIMATIONSIZE
   and update ANIMATIONSIZE with the new define */
  
#define WALKSIZE 6
#define JUMPSIZE 1
#define ANIMATIONSIZE (WALKSIZE + JUMPSIZE)

const unsigned char* dinoSmallAnimSet[ANIMATIONSIZE] = {
      dinoSmallAnimSet0,dinoSmallAnimSet1,dinoSmallAnimSet2,
      dinoSmallAnimSet3,dinoSmallAnimSet4,dinoSmallAnimSet5,
      dinoSmallAnimSet6
    };
    
const unsigned char* dinoLongAnimSet[ANIMATIONSIZE] = {
      dinoLongAnimSet0,dinoLongAnimSet1,dinoLongAnimSet2,
      dinoLongAnimSet3,dinoLongAnimSet4,dinoLongAnimSet5,
      dinoLongAnimSet6
    };

const unsigned char* dinoSpikeAnimSet[ANIMATIONSIZE] = {
      dinoSpikeAnimSet0,dinoSpikeAnimSet1,dinoSpikeAnimSet2,
      dinoSpikeAnimSet3,dinoSpikeAnimSet4,dinoSpikeAnimSet5,
      dinoSpikeAnimSet6
    };

const unsigned char* dinoTrikeAnimSet[ANIMATIONSIZE] = {
      dinoTrikeAnimSet0,dinoTrikeAnimSet1,dinoTrikeAnimSet2,
      dinoTrikeAnimSet3,dinoTrikeAnimSet4,dinoTrikeAnimSet5,
      dinoTrikeAnimSet6
    };

//junk data to fix sprite issues (some frames of actual 3 skins dont work without inclusion)
const unsigned char* dinoJunkAnimSet[ANIMATIONSIZE] = {
      dinoJunkAnimSet0,dinoJunkAnimSet1,dinoJunkAnimSet2,
      dinoJunkAnimSet3,dinoJunkAnimSet4,dinoJunkAnimSet5,
      dinoJunkAnimSet6
    };
const unsigned char* dinoJunk2AnimSet[ANIMATIONSIZE] = {
      dinoJunk2AnimSet0,dinoJunk2AnimSet1,dinoJunk2AnimSet2,
      dinoJunk2AnimSet3,dinoJunk2AnimSet4,dinoJunk2AnimSet5,
      dinoJunk2AnimSet6
    };
const unsigned char* dinoJunk3AnimSet[ANIMATIONSIZE] = {
      dinoJunk3AnimSet0,dinoJunk3AnimSet1,dinoJunk3AnimSet2,
      dinoJunk3AnimSet3,dinoJunk3AnimSet4,dinoJunk3AnimSet5,
      dinoJunk3AnimSet6
    };
const unsigned char* dinoJunk4AnimSet[ANIMATIONSIZE] = {
      dinoJunk4AnimSet0,dinoJunk4AnimSet1,dinoJunk4AnimSet2,
      dinoJunk4AnimSet3,dinoJunk4AnimSet4,dinoJunk4AnimSet5,
      dinoJunk4AnimSet6
    };
    const unsigned char* dinoJunk5AnimSet[ANIMATIONSIZE] = {
      dinoJunk5AnimSet0,dinoJunk5AnimSet1,dinoJunk5AnimSet2,
      dinoJunk5AnimSet3,dinoJunk5AnimSet4,dinoJunk5AnimSet5,
      dinoJunk5AnimSet6
    };
    
  
const unsigned char* eating[4] = {
      eating1,eating2,eating3,eating2
    };
