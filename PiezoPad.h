#include <Streaming.h>


/*
 * Create an Object to easily tie Piezo sensors to a MIDI output. 
 * 
 * Piezos aren't too complex to read, and MIDI is fortunately pretty easy to get the hang of.
 * Inside the .cpp file, you can alternate commenting out the MIDI messages and Serial messages, and change 
 * the usb-type of the Teensy board to debug.
 * 
 * Useful MIDI references:
 * http://www.music-software-development.com/midi-tutorial.html
 * http://computermusicresource.com/GM.Percussion.KeyMap.html
 * 
 * 
 * 
 * 
 */

#ifndef _PIEZOPAD_h
#define _PIEZOPAD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#endif


#ifndef PIEZOPAD_NSAMPLES
#define PIEZOPAD_NSAMPLES 20
#endif

class PiezoPad {
  public:
    //Constructor
    PiezoPad(uint16_t pin, uint16_t led, uint32_t debouncetime, uint32_t notelength);

    void printStuff();
    void updateBuffer();
    uint16_t findMaxValue();
    uint16_t findMeanValue();
    void sendMidi(uint8_t Note, uint16_t Neighbor1, uint16_t Neighbor2);

    uint32_t SensorReading;
    uint32_t Velocity;
    uint32_t pVelocity; 

  private:
    uint16_t _PIN;
    uint16_t _LED;
    uint32_t _DEBOUNCETIME; //First timeout period, prevents double-triggering
    uint32_t _NOTELENGTH;


    
    uint32_t startMillis;
    static const uint16_t _SENSORMAX = 300;
    static const uint16_t _SENSORMIN = 3;
    static const uint8_t _MINVOLUME = 20; //minimum value to trigger a note to play
    static const uint16_t _MAXVOLUME = 127; //might not even be used
    uint16_t samples[PIEZOPAD_NSAMPLES]; //array for the samples of the analog pin
    bool noteplayed = false;
};
