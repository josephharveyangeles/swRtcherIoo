/** swRtcheIoo
* IR controlled extension box
* josephharveyangeles
* 
* 01-17-2015 : first draft
* 01-24-2015 : created header files
* 01-25-2015 : cleaning up and refactoring
*/

#include "cbox.h"
#include "cboxircodes.h"

#ifdef COBO_ACONST
  // flags, statusFlags[4] = power flag
  boolean statusFlags[4] = {false, false, false, false};
  boolean powerFlag = false;
  boolean modeFlag = false;
  boolean muteFlag = false;
  
  // IR reading buffer
  uint16_t pulses[100][2]; 
  // reading index pointer
  uint8_t currentpulse = 0;
#endif

void setup(void) {
  
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
  initPins();
  initLeds();
}
/*************************** Init Methods ***********************/
void initPins(){
  pinMode(13, OUTPUT);
  pinMode(powLed,OUTPUT);
  for(int i=0; i<4; i++){
    pinMode(statusLeds[i], OUTPUT);
    pinMode(triggerRelays[i], OUTPUT);
  }
}

void initLeds(){
  boolean tempFlag = false;
  digitalWrite(powLed, HIGH);
  
  for(int j=0; j<2; j++){
    for(int i=0; i<4; i++){
      digitalWrite(statusLeds[i], HIGH);
      tempFlag = !tempFlag;
      digitalWrite(13, tempFlag);
      delay(200);
    }
    for(int i=0; i<4; i++){
      digitalWrite(statusLeds[i], LOW);
      tempFlag = !tempFlag;
      digitalWrite(13, tempFlag);
      delay(200);
    }
  }
}

/****************************************************************/

void trigger(int index){
  statusFlags[index] = !statusFlags[index];
  digitalWrite(statusLeds[index], statusFlags[index]);
  digitalWrite(triggerRelays[index], statusFlags[index]);
}

void loop(void) {
  int numberpulses;
  numberpulses = listenForIR();

  if(numberpulses < 10){
    return;
  }
  if (IRcompare(numberpulses, powerRemoteSignal, sizeof(powerRemoteSignal)/4)) {
    powerFlag = !powerFlag;
    for(int i=0; i<4; i++){
      statusFlags[i] = powerFlag;
      digitalWrite(statusLeds[i], statusFlags[i]);
      digitalWrite(triggerRelays[i], statusFlags[i]);
    }
  }
  else if (IRcompare(numberpulses, modeSignal, sizeof(modeSignal)/4)) {
    modeFlag = !modeFlag;
    for(int i=0; i<=1; i++){
      statusFlags[i] = modeFlag;
      digitalWrite(statusLeds[i], statusFlags[i]);
      digitalWrite(triggerRelays[i], statusFlags[i]);
    }
  }
  else if (IRcompare(numberpulses, muteSignal, sizeof(muteSignal)/4)) {
    muteFlag = !muteFlag;
    for(int i=2; i<=3; i++){
      statusFlags[i] = muteFlag;
      digitalWrite(statusLeds[i], statusFlags[i]);
      digitalWrite(triggerRelays[i], statusFlags[i]);
    }
  }
  else if (IRcompare(numberpulses, numZeroSignal, sizeof(numZeroSignal)/4)) {
    trigger(0);
  }
  else if (IRcompare(numberpulses, numOneSignal, sizeof(numOneSignal)/4)) {
    trigger(1);
  }
  else if (IRcompare(numberpulses, numTwoSignal, sizeof(numTwoSignal)/4)) {
    trigger(2);
  }
  else if (IRcompare(numberpulses, numThreeSignal, sizeof(numThreeSignal)/4)) {
    trigger(3);
  }
  else{
    return;
  }
}

int listenForIR(void) {
  currentpulse = 0;
  while (1) {
    uint16_t highpulse, lowpulse; // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length

    while (IRread) {
      // pin is still HIGH
      // count off another few microseconds
      highpulse++;
      delayMicroseconds(RESOLUTION);
      // If the pulse is too long, we 'timed out' - either nothing
      // was received or the code is finished, so print what
      // we've grabbed so far, and then reset
      if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
        return currentpulse;
      }
    }

    // we didn't time out so lets stash the reading
    pulses[currentpulse][0] = highpulse;
    // same as above
    while (! (IRpin_PIN & _BV(IRpin))) {
      // pin is still LOW
      lowpulse++;
      delayMicroseconds(RESOLUTION);
      if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
        return currentpulse;
      }
    }

    pulses[currentpulse][1] = lowpulse;
    // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}

boolean IRcompare(int numpulses, int Signal[], int refsize) {
  int count = min(numpulses,refsize);
  
  for (int i=0; i< count-1; i++) {
    int oncode = pulses[i][1] * RESOLUTION / 10;
    int offcode = pulses[i+1][0] * RESOLUTION / 10;
    
    
    // check to make sure the error is less than FUZZINESS percent
    if ( abs(oncode - Signal[i*2 + 0]) <= (Signal[i*2 + 0] * FUZZINESS / 100)) {
    } else {
      return false;
    }
    
    
    if ( abs(offcode - Signal[i*2 + 1]) <= (Signal[i*2 + 1] * FUZZINESS / 100)) {
    } else {
      return false;
    }
  }
  
  return true;
}




