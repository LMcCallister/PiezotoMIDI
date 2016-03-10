#include "PiezoPad.h"

PiezoPad::PiezoPad(uint16_t pin, uint16_t led, uint32_t debouncetime, uint32_t notelength) {
  _PIN = pin;
  _LED = led;
  _DEBOUNCETIME = debouncetime;
  _NOTELENGTH = notelength;
}

void PiezoPad::printStuff() {
  //DEBUG ONLY
  Serial << "Debounce: " << _DEBOUNCETIME << " Notelength" << _NOTELENGTH << endl;
}

void PiezoPad::updateBuffer() {
  //does a read on the analog pin assigned to the object, puts it in a buffer of samples
  static uint8_t bufferpos = 0;
  samples[bufferpos] = analogRead(_PIN);
  if (bufferpos < PIEZOPAD_NSAMPLES)
  {
    bufferpos++;
  }
  if (bufferpos >= PIEZOPAD_NSAMPLES) {
    bufferpos = 0;
  }
}

uint16_t PiezoPad::findMaxValue() {
  //returns highest value in buffer
  uint16_t value = 0;
  for (uint8_t i = 0; i < PIEZOPAD_NSAMPLES - 1; i++) {
    if (samples[i] > samples[i + 1]) {
      value = samples[i];
    }
  }
  return value;
}

uint16_t PiezoPad::findMeanValue() {
  //Return avg value of the sample buffer
  uint16_t value = 0;
  for (uint8_t i = 0; i < PIEZOPAD_NSAMPLES; i++) {
    value += samples[i];
  }
  value = value / PIEZOPAD_NSAMPLES;
  return value;
}

void PiezoPad::sendMidi(uint8_t Note, uint16_t Neighbor1, uint16_t Neighbor2) {
  //Title is a bit misleading. This function will only send a MIDI if a proper hit is detected.
  /* Neighbor 1 & 2 are the nearest pads to the Pad we are currently using.
     This pad can ONLY play if it's own value exceeds 33% of the value of either of those pads.
     This will cancel out dual-playing notes from a single hit.
  */
  uint16_t MaxReading = findMaxValue();
  //uint16_t MeanReading = findMeanValue();
  if ((MaxReading > (Neighbor1 / 3)) && (MaxReading > (Neighbor2 / 3)) && (MaxReading > _SENSORMIN)) {
    SensorReading = MaxReading = constrain(MaxReading, 0, _SENSORMAX);
    Velocity = map(SensorReading, 0, _SENSORMAX, 0, 127);
  } else {
    Velocity = 0;
  }

  if ((Velocity > _MINVOLUME)&& (Velocity > (pVelocity / 3))) {
    if (millis() > (startMillis + _DEBOUNCETIME) ) {
      if (noteplayed == true) {        //Turn off the already playing note before proceeding
        //DEBUG ONLY
        //Serial << "Reset " << Note << endl;
        usbMIDI.sendNoteOff(Note, 0, 1);
      }
      
      //FOR DEBUG ONLY
      //Serial << "Sending " << Note << " At " << Velocity << endl;
      usbMIDI.sendNoteOn(Note, Velocity, 1);//play midi note: (note, velocity, channel)

      startMillis = millis();
      noteplayed = true;
      digitalWrite(_LED, HIGH);
      pVelocity = Velocity;
    }
  }

  if (noteplayed == true) {
    if (millis() >= (startMillis + _NOTELENGTH)) {
      //DEBUG ONLY
      //Serial << "Timeout " << Note << endl;
      usbMIDI.sendNoteOff(Note, 0, 1);
      noteplayed = false;
      digitalWrite(_LED, LOW);
      pVelocity = 0;
    }
  }
}


