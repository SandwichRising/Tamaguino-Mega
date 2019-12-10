/* ======================
 * 
 * Sound System
 * sound generation and queuing
 *
 * Usage: queueSound(frequency, duration); or queueSound(frequency, duration, delay);
 * Example: queueSound(1000, 250, 1000)
 * Example will play a 1000 freq tone for 250ms, and wont play another tone for 1000ms after the *start* of the tone
 * Adds a sound to the first available slot in the (currently) 10-value queue to be played. 
 * Optionally, delay adds a ms delay before the next tone is able to be played.
 *
 * playSound(); checks if the next tone can be played (based of milli() timing) and plays it after checking the sound 
 * setting to see if sounds are enabled. Plays the first tone in queue then bumps all sounds up the queue a slot.
 *
 * 
 ===================== */

#include <Arduino.h>

//Sound System
bool soundEnabled=true;
long unsigned previousSoundMillis = 0;
int currentSoundDelay = 0;
// queue for playing music or single sounds in format of: [freqs],[durations],[delays]
int soundArray[3][10] = {{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};


void queueSound(int soundFrequency, int soundDuration, int soundDelay){
  int i = 0;
  while (i < 10){
    //Check for empty slot after queued sounds, load sound into it if one exists
    if (!soundArray[0][i]) {
      soundArray[0][i] = soundFrequency;
      soundArray[1][i] = soundDuration;
      soundArray[2][i] = soundDelay;
      i = 10;
    }
    i++;
  }
}

void queueSound(int soundFrequency, int soundDuration){
  int i = 0;
  while (i < 10){
    //Check for empty slot after queued sounds, load sound into it if one exists
    if (!soundArray[0][i]) {
      soundArray[0][i] = soundFrequency;
      soundArray[1][i] = soundDuration;
      soundArray[2][i] = soundDuration;
      i = 10;
    }
    i++;
  }
}


void playSound(long unsigned currentMillisFunc){
  //Play Any Queued Sounds
  //Already millie limited, doesn't need contained in Frame Rate
  if (soundArray[0][0]){
    //Check if it's time to play a sound
    if (currentMillisFunc > previousSoundMillis + currentSoundDelay){  
      if(soundEnabled){
        tone(soundPin, soundArray[0][0], soundArray[1][0]);
      }
      //Set the time the next sound can play
      if(soundArray[2][0] >= soundArray[1][0]){ currentSoundDelay = soundArray[2][0]; }
      else currentSoundDelay = soundArray[1][0];
      //Bump all the sounds up a slot
      for (int i = 0; i < 3; i++){
        for (int j = 0; j < 9; j++) {
        soundArray[i][j] = soundArray[i][j+1];
        }
      }
      //Free up last sound slot
      for (int i = 0; i < 3; i++){
        soundArray[i][9] = 0;
      }
      previousSoundMillis = currentMillisFunc;
    } 
  }
}     
