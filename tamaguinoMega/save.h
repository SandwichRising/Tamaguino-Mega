/* ================
 *
 * Saving/loading to/from EEPROM
 *
 * usage: saveGame(parameter)
 * parameters:
 * "save" - saves the game data values to EEPROM and sets the EEPROM saveState byte (address 1) to 99
 * "load" - checks the EEPROM saveState byte (address 1) for 99 value, if found loads the EEPROM data
 * "erase" - sets the  EEPROM saveState byte (address 1) to 0, causing "load" to not load from EEPROM
 *
 ================== */


/*
Data types and default values for data used in saving: 
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
int hiScore = 0; 
*/

void saveGame(String saveType) {
  
  #define SAVESTATE_ADDRESS 1
  #define NAMEMAP_ADDRESS 2   // 11 OF THESE
  #define HUNGER_ADDRESS 26
  #define HAPPINESS_ADDRESS 30
  #define HEALTH_ADDRESS 34
  #define DISCIPLINE_ADDRESS 38
  #define WEIGHT_ADDRESS 42
  #define AGE_ADDRESS 46
  #define POOPOMETER_ADDRESS 50
  #define POOPS_ADDRESS 54    // 3 OF THESE
  #define EVOLUTION_ADDRESS 60
  #define HISCORE_ADDRESS 62
  
  //EEPROM.commands don't seem to like math in them, these are used to 
  //do math in-loop before invoking EEPROM.commands
  int eepromHelper = 0;
  int eepromAddressHelper = 0;
  
  if (saveType == "erase"){
    //check if saveState bit == 0 (erase save), if not write it to 0
    byte saveState = 0;
    EEPROM.update(SAVESTATE_ADDRESS, saveState);
  }
  
  // load game if saveState bit == 99
  if (saveType == "load"){
    byte saveState = 0;
    EEPROM.get(SAVESTATE_ADDRESS,saveState);
    if (saveState == 99){
      for (int i=0;i<11;i++){
        eepromAddressHelper = NAMEMAP_ADDRESS+(i*2);
        EEPROM.get(eepromAddressHelper,eepromHelper);
        nameMap[i] = eepromHelper;
      }
      EEPROM.get(NAMEMAP_ADDRESS,nameMap);
      EEPROM.get(HUNGER_ADDRESS, hunger);
      EEPROM.get(HAPPINESS_ADDRESS, happiness);
      EEPROM.get(HEALTH_ADDRESS, health);
      EEPROM.get(DISCIPLINE_ADDRESS, discipline);
      EEPROM.get(WEIGHT_ADDRESS, weight);
      EEPROM.get(AGE_ADDRESS, age);
      EEPROM.get(POOPOMETER_ADDRESS, poopometer);
      for (int i=0;i<3;i++){
        eepromAddressHelper = POOPS_ADDRESS+(i*2);
        EEPROM.get(eepromAddressHelper,eepromHelper);
        poops[i] = eepromHelper;
      }
      EEPROM.get(EVOLUTION_ADDRESS,evolution);
      EEPROM.get(HISCORE_ADDRESS, hiScore);
    }
  }
  
  // save game (write save byte to 99)
  if (saveType == "save"){
    byte saveState = 99;
    EEPROM.put(SAVESTATE_ADDRESS,saveState);
    for (int i=0;i<11;i++){
      eepromHelper = nameMap[i];
      eepromAddressHelper = NAMEMAP_ADDRESS+(i*2);
      EEPROM.put(eepromAddressHelper,eepromHelper);
    }
    EEPROM.put(HUNGER_ADDRESS, hunger);
    EEPROM.put(HAPPINESS_ADDRESS, happiness);
    EEPROM.put(HEALTH_ADDRESS, health);
    EEPROM.put(DISCIPLINE_ADDRESS, discipline);
    EEPROM.put(WEIGHT_ADDRESS, weight);
    EEPROM.put(AGE_ADDRESS, age);
    EEPROM.put(POOPOMETER_ADDRESS, poopometer);
    for (int i=0;i<3;i++){
      eepromHelper = poops[i];
      eepromAddressHelper = POOPS_ADDRESS+(i*2);
      EEPROM.put(eepromAddressHelper,eepromHelper);
    }
    EEPROM.put(EVOLUTION_ADDRESS,evolution);
    EEPROM.put(HISCORE_ADDRESS, hiScore);
  }
}
