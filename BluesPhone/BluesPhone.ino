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
// lumens. The code could probably be rewritten to work with standard
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
int NOTE_MS = 200;
int CALIBRATION_MS = 2000;
int AVG_ANALOG_SAMPLES = 64;
int mode = 0;
boolean tsl_present = false;
uint16_t minlight;
uint16_t maxlight;
uint16_t x = 0;
int analogPin0 = A0;
int soundPin0 = 13;
int note = 0;

int blues_scale [] = {
  NOTE_C1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_G1, NOTE_A1, NOTE_B1,
  NOTE_C2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_G2, NOTE_A2, NOTE_B2,
  NOTE_C3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_G3, NOTE_A3, NOTE_B3,
  NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_B5,
  NOTE_C6
};

int pent_scale [] = {
  NOTE_C1, NOTE_C1, NOTE_D1, NOTE_E1, NOTE_G1,
  NOTE_A1, NOTE_C2, NOTE_D2, NOTE_E2, NOTE_G2,
  NOTE_A2, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_G3,
  NOTE_A3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4,
  NOTE_A4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5,
  NOTE_A5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_G6
};

int octave_scale [] = {
  NOTE_C1, NOTE_D1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_A1, NOTE_B1,
  NOTE_C2, NOTE_D2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_A2, NOTE_B2,
  NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5
};

// C-D♭-E-F-G-A♭-B
int arabic_scale [] = {
  NOTE_C1, NOTE_CS1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_GS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_GS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_GS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_GS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_GS5, NOTE_B5
};

int MAX_NOTE_INDEX = sizeof(octave_scale) / sizeof(NOTE_C1);

void setup(void) {
  Serial.begin(9600);
  if (tsl.begin())
  {
    // You can change the gain on the fly, to adapt to brighter/dimmer light situations
    //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
    tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
    Serial.print("max note index ");
    Serial.println(MAX_NOTE_INDEX);
    // Changing the integration time gives you a longer time over which to sense light
    // longer timelines are slower, but are good in very low light situtations!
    //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
    tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
    //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

    // Now we're ready to get readings!
    tone(soundPin0, octave_scale[0], NOTE_MS);
    delay(CALIBRATION_MS);
    minlight = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    if (minlight < 65535)   // Assme max unsigned read means tsl is not available
    {
      tsl_present = true;
      tone(soundPin0, octave_scale[MAX_NOTE_INDEX], NOTE_MS);
      delay(CALIBRATION_MS);
      maxlight = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    }
    else
    {
      tsl_present = false;
      minlight = analogRead(analogPin0);
      tone(soundPin0, octave_scale[MAX_NOTE_INDEX], NOTE_MS);
      delay(CALIBRATION_MS);
      maxlight = analogRead(analogPin0);
    }
    Serial.print("Calibrate minimum light = ");
    Serial.print(minlight);
    Serial.print("\n");
    Serial.print("Calibrate maximum light = ");
    Serial.print(maxlight);
    Serial.print("\n");
    if (abs(maxlight - minlight) < 10) {
      // calibration failed, make a best guess.
      minlight = 20;
      maxlight = 256;
    } else 
    {
    if (maxlight < minlight)
    {        // swap values
      maxlight = maxlight xor minlight;
      minlight = maxlight xor minlight;
      maxlight = maxlight xor minlight;
    }
  }
}
else {
  Serial.print("Couldn't connect to tsl light sensor");
    while (1);
  }
}

void loop(void) {
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read

  if (tsl_present)
  {
    x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  }
  else
  {
    // Fall back on analog input
    x = 0;
    for (int i = 1; i < AVG_ANALOG_SAMPLES; i++) {
      x = x + analogRead(analogPin0);
      delay(1);
    }
    x = x / AVG_ANALOG_SAMPLES;
  }
  // Serial.println(x);

  // Cover the light sensor to make it very dark in order to
  // select the next mode
  if (x < minlight)
  {
    mode = mode + 1;
    if (mode == 4) {
      tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // shortest integration time (bright light)
    }
    else {
      tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
    }
    if ( mode > 4) mode = 0;
    Serial.print("mode ");
    Serial.println(mode);
    delay(3000);
  }
  note = map(x, minlight, maxlight, 0, MAX_NOTE_INDEX);
  note = constrain (note, 0, MAX_NOTE_INDEX);
  if (mode == 0)
  {
    tone(soundPin0, pent_scale[note], NOTE_MS);
  }
  else if (mode == 1)
  {
    tone(soundPin0, blues_scale[note], NOTE_MS);
  }
  else if (mode == 2)
  {
    tone(soundPin0, octave_scale[note], NOTE_MS);
  }
  else if (mode == 3)
  {
    tone(soundPin0, arabic_scale[note], NOTE_MS);
  }
  else if (mode == 4)
  {
    tone(soundPin0, map(x, minlight, maxlight, 20, 1024), NOTE_MS);
  }
}
