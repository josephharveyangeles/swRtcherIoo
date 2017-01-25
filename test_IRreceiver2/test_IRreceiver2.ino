#include "cbox.h"
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

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing
void setup(void) {
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}

void loop(void) {
  int numberpulses;
  numberpulses = listenForIR();
  Serial.print("Heard ");
  Serial.print(numberpulses);
  Serial.println("-pulse long IR signal");
  if(numberpulses>=34){
    printpulses();
    Serial.println();
  }
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

void printpulses(void){
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}
