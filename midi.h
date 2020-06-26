#pragma once

#include <mbed.h>
#include "CircularBuffer.h"

const static int receiveMessageBufferSize = 1024;

const static int systemExclusiveBufferSize = 128;

const  static uint8_t gmSystemOn[] = {0x7E,0x7F,0x09,0x01};

const  static uint8_t xgSystemOn[] = {0x43,0x10,0x4C,0x00,0x00,0x7E,0x00};

const  static uint8_t gsReset[]    = {0x41,0x10,0x42,0x12,0x40,0x00,0x7F,0x00,0x41};

class MIDI{
  public:
    MIDI(Serial uart,uint16_t baud = 31250);
    //receive Functions

    void resetStatus(void);

    void setCallbackReceiveNoteOff(void (*func)(uint8_t,uint8_t,uint8_t));
    void setCallbackReceiveNoteOn(void (*func)(uint8_t,uint8_t,uint8_t));
    void setCallbackReceivePolyphonicKeyPressure(void (*func)(uint8_t,uint8_t,uint8_t));
    void setCallbacKReceiveControlChange(void (*func)(uint8_t,uint8_t,uint8_t));
    void setCallbackReceiveProgramChange(void (*func)(uint8_t,uint8_t));
    void setCallbackReceiveChannelPressure(void (*func)(uint8_t,uint8_t));
    void setCallbackReceivePitchWheelChange(void (*func)(uint8_t,uint16_t));

    void setCallbackReceiveAllSoundsOff(void (*func)(uint8_t));
    void setCallbackReceiveResetAllController(void (*func)(uint8_t));
    void setCallbackReceiveLocalControl(void (*func)(uint8_t,bool));
    void setCallbackReceiveAllNotesOff(void (*func)(uint8_t));

    void setCallbackReceiveSystemExclusive(void (*func)(uint8_t*,uint8_t));
    void setCallbackReceiveReset(void (*func)(void));

    void midiParse(void);

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

  private:
    Serial _serial;

    lunaLib::CircularBuffer<uint8_t,receiveMessageBufferSize> messageBuffer;

    uint8_t systemExclusiveBuffer[systemExclusiveBufferSize];
    uint8_t systemExclusiveBufferPosition = 0;

    void (*callbackFunctionNoteOff)(uint8_t,uint8_t,uint8_t);
    void (*callbackFunctionNoteOn)(uint8_t,uint8_t,uint8_t);
    void (*callbackFunctionPolyphonicKeyPressure)(uint8_t,uint8_t,uint8_t);
    void (*callbackFunctionControlChange)(uint8_t,uint8_t,uint8_t);
    void (*callbackFunctionProgramChange)(uint8_t,uint8_t);
    void (*callbackFunctionChannelPressure)(uint8_t,uint8_t);
    void (*callbackFunctionPitchWheelChange)(uint8_t,uint16_t);

    void (*callbackFunctionAllSoundsOff)(uint8_t);
    void (*callbackFunctionResetAllController)(uint8_t);
    void (*callbackFunctionLocalControl)(uint8_t,bool);
    void (*callbackFunctionAllNotesOff)(uint8_t);

    void (*callbackFunctionSystemExclusive)(uint8_t*,uint8_t);
    void (*callbackFunctionReset)(void);

    uint8_t runningStatusMessage = 0x00;
    uint8_t runningStatusChannel = 0x00;

    bool isSystemExclusiveMode = false;
    bool threeBytesFlag = false;

    uint8_t data[3];

    void receiveMessage(void);
    void decodeMessage(void);
    void decodeSystemExclusive(void);
    bool checkResetMessage(void);

    const static uint8_t noteOff                = 0x80;
    const static uint8_t noteOn                 = 0x90;
    const static uint8_t polyphonicKeyPressure  = 0xA0;
    const static uint8_t controlChange          = 0xB0;
    const static uint8_t programChange          = 0xC0;
    const static uint8_t channelPressure        = 0xD0;
    const static uint8_t pitchWheelChange       = 0xE0;

    const static uint8_t allSoundsOff           = 0x78;
    const static uint8_t resetAllController     = 0x79;
    const static uint8_t localControl           = 0x7A;
    const static uint8_t allNotesOff            = 0x7B;

    const static uint8_t beginSystemExclusive   = 0xF0;
    const static uint8_t endSystemExclusive     = 0xF7;
    const static uint8_t activeSensing          = 0xFE;
};