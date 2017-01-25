/*
  cbox.h - Library for swRtcheIoo device, IR controlled extension box
  Created by Joseph Harvey C. Angeles, January 24, 2015
  Not released to any parties, privately maintained.
*/


#ifndef CBOX_H
  /***************** IR Input pin configuration *****************/
  #ifndef COBO_LLEVEL_IR_INPUT
    // low level pin access macros
    #define IRpin_PIN PIND // PORTD access,
    #define IRpin 3        // IR pin
    #define IRread (IRpin_PIN & (1 << IRpin)) // you know these,
                                              // ANDing stuff. 
  #define COBO_LLEVEL_IR_INPUT
  #endif
  /**************************************************************/
  
  /************* IR reading calibration constants ***************/
  #ifndef COBO_IRCALCON
    #define MAXPULSE 65000
    
    // what our timing resolution should be, larger is better
    // as its more 'precise' - but too large and you wont get
    // accurate timing
    #define RESOLUTION 20
    #define FUZZINESS 20 // some leeway, % error of margin when
                         // matching IR codes read
  #define COBO_IRCALCON
  #endif
  /**************************************************************/
  
  /***************** Output Pin Declarations ********************/
  #ifndef COBO_OUTPINS
    // Status LEDs pinouts
    #define powLed A1 // Status led, power led
    #define ledZero 12
    #define ledOne 11
    #define ledTwo 10
    #define ledThree 9
    
    // Relay pinouts
    #define relZero 8
    #define relOne 7
    #define relTwo 6
    #define relThree 5
  #define COBO_OUTPINS
  #endif
  /**************************************************************/
  
  /*********************** Array globals ************************/
  #ifndef COBO_ACONST
    int statusLeds[4] = {ledZero, ledOne, ledTwo, ledThree};
    int triggerRelays[4] = {relZero, relOne, relTwo, relThree};
  #define COBO_ACONST
  #endif
  /**************************************************************/
  
#define CBOX_H
#endif



