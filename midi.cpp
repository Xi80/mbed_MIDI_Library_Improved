#include "midi.h"

MIDI::MIDI(Serial uart,uint16_t baud = 31250) :_serial(uart){
    _serial.baud(baud);

    callbackFunctionNoteOff                 = NULL;
    callbackFunctionNoteOn                  = NULL;
    callbackFunctionPolyphonicKeyPressure   = NULL;
    callbackFunctionControlChange           = NULL;
    callbackFunctionProgramChange           = NULL;
    callbackFunctionChannelPressure         = NULL;
    callbackFunctionPitchWheelChange        = NULL;

    callbackFunctionAllSoundsOff            = NULL;
    callbackFunctionResetAllController      = NULL;
    callbackFunctionLocalControl            = NULL;
    callbackFunctionAllNotesOff             = NULL;

    callbackFunctionSystemExclusive         = NULL;

    resetStatus();
    _serial.attach(this,&MIDI::receiveMessage,Serial::RxIrq);
}

void MIDI::resetStatus(void){
    messageBuffer.clear();
    systemExclusiveBufferPosition = 0;
    return;
}

void MIDI::setCallbackReceiveNoteOff(void (*func)(uint8_t,uint8_t,uint8_t)){
    callbackFunctionNoteOff = func;
    return;
}

void MIDI::setCallbackReceiveNoteOn(void (*func)(uint8_t,uint8_t,uint8_t)){
    callbackFunctionNoteOn = func;
    return;
}

void MIDI::setCallbackReceivePolyphonicKeyPressure(void (*func)(uint8_t,uint8_t,uint8_t)){
    callbackFunctionPolyphonicKeyPressure = func;
    return;
}

void MIDI::setCallbacKReceiveControlChange(void (*func)(uint8_t,uint8_t,uint8_t)){
    callbackFunctionControlChange = func;
    return;
}

void MIDI::setCallbackReceiveProgramChange(void (*func)(uint8_t,uint8_t)){
    callbackFunctionProgramChange = func;
    return;
}

void MIDI::setCallbackReceiveChannelPressure(void (*func)(uint8_t,uint8_t)){
    callbackFunctionChannelPressure = func;
    return;
}

void MIDI::setCallbackReceivePitchWheelChange(void (*func)(uint8_t,uint16_t)){
    callbackFunctionPitchWheelChange = func;
    return;
}

void MIDI::setCallbackReceiveAllSoundsOff(void (*func)(uint8_t)){
    callbackFunctionAllSoundsOff = func;
    return;
}

void MIDI::setCallbackReceiveResetAllController(void (*func)(uint8_t)){
    callbackFunctionResetAllController = func;
    return;
}

void MIDI::setCallbackReceiveLocalControl(void (*func)(uint8_t,bool)){
    callbackFunctionLocalControl = func;
    return;
}

void MIDI::setCallbackReceiveAllNotesOff(void (*func)(uint8_t)){
    callbackFunctionAllNotesOff = func;
    return;
}

void MIDI::setCallbackReceiveSystemExclusive(void (*func)(uint8_t*,uint8_t)){
    callbackFunctionSystemExclusive = func;
    return;
}

void MIDI::setCallbackReceiveReset(void (*func)(void)){
    callbackFunctionReset = func;
    return;
}

void MIDI::midiParse(void){
    if(!messageBuffer.size())return;
    data[0] = messageBuffer.pull();

    if(isSystemExclusiveMode){
        if(data[0] == endSystemExclusive){
            isSystemExclusiveMode = false;
            decodeSystemExclusive();
            return;
        } else {
            if(systemExclusiveBufferPosition > systemExclusiveBufferSize)systemExclusiveBufferPosition = 0;
            if(data[0] == activeSensing)return;
            systemExclusiveBuffer[systemExclusiveBufferPosition++] = data[0];
            return;
        }
    } else {
        if(data[0] & 0x80){
            if(!(data[0] >= 0xF8)){
                runningStatusMessage    = data[0] & 0xF0;
                runningStatusChannel    = data[0] & 0x0F;
                threeBytesFlag          = false;
                return;
            }
        } else {
            if(threeBytesFlag){
                threeBytesFlag  = false;
                data[2]         = data[0];
                decodeMessage();
                return;
            } else {
                if(runningStatusMessage < 0xC0){
                    threeBytesFlag = true;
                    data[1] = data[0];
                    return;
                } else {
                    if(runningStatusMessage < 0xE0){
                        data[1] = data[0];
                        decodeMessage();
                        return;
                    } else {
                        if(runningStatusMessage < 0xF0){
                            threeBytesFlag  = true;
                            data[1]         = data[0];
                            return;
                        }
                    }
                }
            }
        }
    }
}

void MIDI::sendNoteOff(uint8_t channel,uint8_t note,uint8_t velocity){
    _serial.putc(noteOff + channel);
    _serial.putc(note);
    _serial.putc(velocity);
    return;
}

void MIDI::sendNoteOn(uint8_t channel,uint8_t note,uint8_t velocity){
    _serial.putc(noteOn + channel);
    _serial.putc(note);
    _serial.putc(velocity);
    return;
}

void MIDI::sendPolyphonicKeyPressure(uint8_t channel,uint8_t note,uint8_t pressure){
    _serial.putc(polyphonicKeyPressure + channel);
    _serial.putc(note);
    _serial.putc(pressure);
    return;
}

void MIDI::sendControlChange(uint8_t channel,uint8_t number,uint8_t value){
    _serial.putc(controlChange + channel);
    _serial.putc(number);
    _serial.putc(value);
    return;
}

void MIDI::sendProgramChange(uint8_t channel,uint8_t number){
    _serial.putc(programChange + channel);
    _serial.putc(number);
    return;
}

void MIDI::sendChannelPressure(uint8_t channel,uint8_t pressure){
    _serial.putc(channelPressure + channel);
    _serial.putc(pressure);
    return;
}

void MIDI::sendPitchWheelChange(uint8_t channel,uint16_t value){
    uint8_t MSB = (value & 0x7F80) >> 8;
    uint8_t LSB = value & 0x7F00;
    _serial.putc(pitchWheelChange + channel);
    _serial.putc(LSB);
    _serial.putc(MSB);
    return;
}

void MIDI::sendAllSoundsOff(uint8_t channel){
    _serial.putc(controlChange + channel);
    _serial.putc(allSoundsOff);
    _serial.putc(0x00);
    return;
}

void MIDI::sendResetAllController(uint8_t channel){
    _serial.putc(controlChange + channel);
    _serial.putc(resetAllController);
    _serial.putc(0x00);
    return;
}

void MIDI::sendLocalControl(uint8_t channel,bool mode){
    _serial.putc(controlChange + channel);
    _serial.putc(localControl);
    _serial.putc(0x00);
    return;
}

void MIDI::sendAllNotesOff(uint8_t channel){
    _serial.putc(controlChange + channel);
    _serial.putc(allNotesOff);
    _serial.putc(0x00);
    return;
}

void MIDI::sendSystemExclusive(uint8_t *data,uint8_t length){
    _serial.putc(beginSystemExclusive);
    for(int i = 0;i < length;i++){
        _serial.putc(data[i]);
    }
    _serial.putc(endSystemExclusive);
    return;
}

void MIDI::receiveMessage(void){
    uint8_t data = _serial.getc();
    if(messageBuffer.size() > receiveMessageBufferSize)messageBuffer.clear();
    messageBuffer.push_back(data);
    return;
}

void MIDI::decodeMessage(void){
    switch(runningStatusMessage){
        case noteOff:
            if(callbackFunctionNoteOff != NULL)callbackFunctionNoteOff(runningStatusChannel,data[1],data[2]);
            break;
        case noteOn:
            if(callbackFunctionNoteOn != NULL)callbackFunctionNoteOn(runningStatusChannel,data[1],data[2]);
            break;
        case polyphonicKeyPressure:
            if(callbackFunctionPolyphonicKeyPressure != NULL)callbackFunctionPolyphonicKeyPressure(runningStatusChannel,data[1],data[2]);
            break;
        case controlChange:
            switch(data[1]){
                case allSoundsOff:
                    if(callbackFunctionAllSoundsOff != NULL)callbackFunctionAllSoundsOff(runningStatusChannel);
                    break;
                case resetAllController:
                    if(callbackFunctionResetAllController != NULL)callbackFunctionResetAllController(runningStatusChannel);
                    break;
                case localControl:
                    if(callbackFunctionLocalControl != NULL)callbackFunctionLocalControl(runningStatusChannel,(data[2])? true : false);
                    break;
                case allNotesOff:
                    if(callbackFunctionAllNotesOff != NULL)callbackFunctionAllNotesOff(runningStatusChannel);
                    break;
                default:
                    if(callbackFunctionControlChange != NULL)callbackFunctionControlChange(runningStatusChannel,data[1],data[2]);
                    break;
            }
            break;
        case programChange:
            if(callbackFunctionProgramChange != NULL)callbackFunctionProgramChange(runningStatusChannel,data[1]);
            break;
        case channelPressure:
            if(callbackFunctionChannelPressure != NULL)callbackFunctionChannelPressure(runningStatusChannel,data[1]);
            break;
        case pitchWheelChange:
            uint8_t bend = (data[3] << 7) & data[2];
            if(callbackFunctionPitchWheelChange != NULL)callbackFunctionPitchWheelChange(runningStatusChannel,bend);
            break;
    }
}

void MIDI::decodeSystemExclusive(void){
    if(checkResetMessage()){
        if(callbackFunctionReset != NULL)callbackFunctionReset();
    } else {
        if(callbackFunctionSystemExclusive != NULL)callbackFunctionSystemExclusive(systemExclusiveBuffer,systemExclusiveBufferPosition);
    }
}

bool MIDI::checkResetMessage(void){
    for(int i = 0;i < systemExclusiveBufferPosition;i++){
        if(!(systemExclusiveBuffer[i] == gmSystemOn[i]) && !(systemExclusiveBuffer[i] == xgSystemOn[i]) && !(systemExclusiveBuffer[i] == gsReset[i])){
            return false;
        }
    }
    return true;
}