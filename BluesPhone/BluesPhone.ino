#include <Wire.h>
#include "TSL2561.h"
#include "pitches.h"

// Example for demonstrating the TSL2561 library - public domain!
// Modified by Brian Nitz on February 14, 2016 to work as an optical 
// etherphone (aka optical theremin) which works on several scales:
// 1) Blues
// 2) Pentatonic
// 2) Octave
// 4) Arabic
// 5) Continuous (like a traditional theremin) 
//
// This program was designed to use the highly accurate light sensor
// in the TSL2561 sensor. The TSL2561 library outputs brightness in 
// lumins. The code could probably be rewritten to work with standard
// Light Dependant Resitors (LDRs) which would be less expensive, though
// less linear.
//
// connect SCL to analog 5
// connect SDA to analog 4
// connect VDD to 3.3V DC
// connect GROUND to common ground
// ADDR can be connected to ground, or vdd or left floating to change the i2c address

// The address will be different depending on whether you let
// the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
// use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
TSL2561 tsl(TSL2561_ADDR_FLOAT);
int mode = 0;
uint16_t minlight;
uint16_t maxlight;
 
int blues_scale []= {
NOTE_C2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_G2,NOTE_A2, NOTE_B2,
NOTE_C3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_G3,NOTE_A3, NOTE_B3, 
NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_G4,NOTE_A4, NOTE_B4, 
NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_G5,NOTE_A5, NOTE_B5, 
NOTE_C6};

int pent_scale []= {
NOTE_A1, NOTE_C2, NOTE_D2, NOTE_E2, NOTE_G2,
NOTE_A2, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_G3,
NOTE_A3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4,
NOTE_A4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5,
NOTE_A5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_G6};

int octave_scale []= {
NOTE_C2, NOTE_D2, NOTE_E2, NOTE_F2, NOTE_G2,NOTE_A2, NOTE_B2,
NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3,NOTE_A3, NOTE_B3, 
NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,NOTE_A4, NOTE_B4, 
NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5,NOTE_A5, NOTE_B5};

// C-D♭-E-F-G-A♭-B
int arabic_scale [] = {
NOTE_C2, NOTE_CS2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_GS2, NOTE_B2,
NOTE_C3, NOTE_CS3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_GS3, NOTE_B3,
NOTE_C4, NOTE_CS4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_GS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_GS5, NOTE_B5};

void setup(void) {
  
  if (tsl.begin()) {
    tone(13, octave_scale[0], 500);
    delay(1000);
    minlight = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    tone(13, octave_scale[26], 500);
    delay(1000);
    maxlight = tsl.getLuminosity(TSL2561_FULLSPECTRUM);


  } else {
    while (1);
  }
    
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
  // Now we're ready to get readings!
}

void loop(void) {
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
    
  uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  // Cover the light sensor to make it very dark in order to 
  // select the next mode
  if (x < minlight) {
    mode = mode + 1;
    delay(2000);
    if ( mode > 4) mode = 0;
  }
  int note = map(x,minlight, maxlight, 0, 26);
  note = constrain (note, 0, 26);
  if (mode == 0) {
    tone(13, pent_scale[note],200);
  } else if (mode == 1) {
    tone(13, blues_scale[note],200);
  } else if (mode == 2) {
    tone(13, octave_scale[note], 200);
  }
    else if (mode == 3) {
    tone(13, arabic_scale[note], 200);
  } else if (mode == 4) {
    tone(13, map(x,minlight,maxlight,60,512), 200);
  }
  
}
