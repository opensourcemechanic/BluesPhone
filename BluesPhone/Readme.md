BluesPhone by Brian Nitz
Februrary 14, 2016

An Etherphone is also known as a Theremin. The musician plays music by
waving their hands in front of a pair of antenna. One antenna controls
the pitch and the other controls the loudness. Its spooky electronic sound
was commonly used in the sound tracks of science fiction movies. The
traditional etherphone doesn't play discrete notes and so the musician
would either slide from one note to the next or modulate the loudness and
carefully position their other hand to the position of the next note.

The optical etherphone uses light detectors instead of antenna to detect
the position of the musician's hand. In the BluesPhone variation, there 
is the option to move to the next note in discrete steps corresponding
to the Blues scale, pentatonic scale, arabic scale, equally-tempered octave
scale or the continuous non-quantized range of a traidtional theremin.

Hardware requirements:
This progrem requires a TSL2561 optical sensor. It's a linear sensor which
outputs light level in lumens. 

Library requirements:
Before compiling this program, you must use the Arduino library manager to install
Adafruit's TSL2561 unified sennsor driver library:
https://github.com/adafruit/Adafruit_TSL2561

When initialized, this program expects that the musician will block the sensor
to calibrate the low range and then unblock the sensor to calibrate the high 
range. Block the sensor again to select the next scale.



