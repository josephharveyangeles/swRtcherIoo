#define IRpin_PIN PIND
#define IRpin 2
// for MEGA use these!
//#define IRpin_PIN PINE
//#define IRpin 4
// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20
#define FUZZINESS 20

#define led 13
// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing
#include "cboxircodes.h"

boolean flag = false;
void setup(void) {
  pinMode(led,OUTPUT);
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
  initLed();
}

void loop(void) {
  int numberpulses;
  numberpulses = listenForIR();
  Serial.print("Heard ");
  Serial.print(numberpulses);
  Serial.println("-pulse long IR signal");

  if(numberpulses < 10){
    return;
  }

  if (IRcompare(numberpulses, powerRemoteSignal, sizeof(powerRemoteSignal)/4)) {
    Serial.println("Power");
    flag = !flag;
    digitalWrite(led, flag);
  }

  /*else if (IRcompare(numberpulses, modeSignal, sizeof(modeSignal)/4)) {
    Serial.println("Mode");
  }
  else if (IRcompare(numberpulses, muteSignal, sizeof(muteSignal)/4)) {
    Serial.println("Mute");
  }
  else if (IRcompare(numberpulses, playPauseSignal, sizeof(playPauseSignal)/4)) {
    Serial.println("Play/Pause");
  }
  else if (IRcompare(numberpulses, rewindSignal, sizeof(rewindSignal)/4)) {
    Serial.println("Rewind");
  }
  else if (IRcompare(numberpulses, forwardSignal, sizeof(forwardSignal)/4)) {
    Serial.println("Forward");
  }
  else if (IRcompare(numberpulses, eqSignal, sizeof(eqSignal)/4)) {
    Serial.println("EQ");
  }

  else if (IRcompare(numberpulses, volMinusSignal, sizeof(volMinusSignal)/4)) {
    Serial.println("Volume-");
  }

  else if (IRcompare(numberpulses, volPlusSignal, sizeof(volPlusSignal)/4)) {
    Serial.println("Volume+");
  }

  if (IRcompare(numberpulses, rptSignal)) {
    Serial.println("RPT");
  }
  if (IRcompare(numberpulses, usdSignal)) {
    Serial.println("U/SD");
  }
  */
  if (IRcompare(numberpulses, numZeroSignal, sizeof(numZeroSignal)/4)) {
    Serial.println("0");
  }

  if (IRcompare(numberpulses, numOneSignal,sizeof(numOneSignal)/4)) {
    Serial.println("1");
  }
  /*
  if (IRcompare(numberpulses, numTwoSignal)) {
    Serial.println("2");
  }
  if (IRcompare(numberpulses, numThreeSignal)) {
    Serial.println("3");
  }
  if (IRcompare(numberpulses, numFourSignal)) {
    Serial.println("4");
  }
  if (IRcompare(numberpulses, numFiveSignal)) {
    Serial.println("5");
  }
  if (IRcompare(numberpulses, numSixSignal)) {
    Serial.println("6");
  }
  if (IRcompare(numberpulses, numSevenSignal)) {
    Serial.println("7");
  }
  if (IRcompare(numberpulses, numEightSignal)) {
    Serial.println("8");
  }
  if (IRcompare(numberpulses, numNineSignal)) {
    Serial.println("9");
  }
*/
}

int listenForIR(void) {
  currentpulse = 0;
  while (1) {
    uint16_t highpulse, lowpulse; // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length

    while (IRpin_PIN & (1 << IRpin)) {
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
  //Serial.print("count set to: ");
  //Serial.println(count);
  for (int i=0; i< count-1; i++) {
    int oncode = pulses[i][1] * RESOLUTION / 10;
    int offcode = pulses[i+1][0] * RESOLUTION / 10;
    
#ifdef DEBUG    
    Serial.print(oncode); // the ON signal we heard
    Serial.print(" - ");
    Serial.print(Signal[i*2 + 0]); // the ON signal we want 
#endif   
    
    // check to make sure the error is less than FUZZINESS percent
    if ( abs(oncode - Signal[i*2 + 0]) <= (Signal[i*2 + 0] * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.print(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
    
#ifdef DEBUG
    Serial.print("  \t"); // tab
    Serial.print(offcode); // the OFF signal we heard
    Serial.print(" - ");
    Serial.print(Signal[i*2 + 1]); // the OFF signal we want 
#endif    
    
    if ( abs(offcode - Signal[i*2 + 1]) <= (Signal[i*2 + 1] * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.print(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
#ifdef DEBUG
    Serial.println();
#endif
  }
  // Everything matched!
  return true;
}

void initLed(){
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
  digitalWrite(led, HIGH); 
  delay(200);
  digitalWrite(led, LOW); 
  delay(200);
}


