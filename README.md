# mbed MIDI Library -Improved-

## Description

A library for mbed framework that makes it easy to handle MIDI.

Both sending and receiving are **SUPPORTED**.

This library uses a Serial Receive Interrupt.



## Sample Code

```cpp
#include <mbed.h>
#include "midi/midi.h"

MIDI midi(USBTX,USBRX,38400);
DigitalOut led(LED1);

void noteOn(uint8_t channel,uint8_t note,uint8_t velocity){
  led = !led;
}

int main() {
  midi.setCallbackReceiveNoteOn(noteOn);

  while(1) {
    midi.midiParse();
  }
}
```

This is the sample code that causes the LED to blink when a MIDI Note On Event is received.



## set Callback Functions

You can set the function to be called when the event  which shows below occurs.

- Note On
  - void (*func)(uint8_t channel,uint8_t note,uint8_t velocity);
- Note Off
  - void (*func)(uint8_t channel,uint8_t note,uint8_t velocity);
- Polyphonic Key Pressure
  - void (*func)(uint8_t channel,uint8_t note,uint8_t pressure);
- Control Change
  - void (*func)(uint8_t channel,uint8_t number,uint8_t value);
- Program Change
  - void (*func)(uint8_t channel,uint8_t number);
- Channel Pressure
  - void (*func)(uint8_t channel,uint8_t pressure);
- Pitch Wheel Change
  - void (*func)(uint8_t channel,uint16_t bend);
- All Sound Off
  - void (*func)(uint8_t channel);
- Reset All Controller
  - void (*func)(uint8_t channel);
- Local Control
  - void (*func)(uint8_t channel,bool mode);
- All Notes Off
  - void (*func)(uint8_t channel);
- System Exclusive Messages
  - void (*func)(uint8_t *data,uint8_t length)
- Reset(Compatible with GM System On,XG System On,GS Reset)
  - void (*func)(void)

 

## Send MIDI Event

You can also send a MIDI event with the same arguments as the callback function.

The functions are listed in the following code.

```cpp
//Send Functions

void sendNoteOff(uint8_t,uint8_t,uint8_t);
void sendNoteOn(uint8_t,uint8_t,uint8_t);
void sendPolyphonicKeyPressure(uint8_t,uint8_t,uint8_t);
void sendControlChange(uint8_t,uint8_t,uint8_t);
void sendProgramChange(uint8_t,uint8_t);
void sendChannelPressure(uint8_t,uint8_t);
void sendPitchWheelChange(uint8_t,uint16_t);

void sendAllSoundsOff(uint8_t);
void sendResetAllController(uint8_t);
void sendLocalControl(uint8_t,bool);
void sendAllNotesOff(uint8_t);

void sendSystemExclusive(uint8_t*,uint8_t);

```



## Note

This library is a ***WORK IN PROGRESS*** and I will not be liable for any damage caused by the use of this library.

***ALL RIGHTS TO THIS LIBRARY BELONG TO Xi80*** and redistribution or modification is ***PROHIBITED***.