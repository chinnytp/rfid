#ifndef RfidReader_h
#define RfidReader_h

#include "Arduino.h"
#include <SoftwareSerial.h>

#define STX 0xAA
#define ETX 0xBB

class RfidReader
{
  public:
    RfidReader(int txPin, int rxPin);
    void begin(int baudRate);
    String getCode();
    
  private:
          // Station Ids
const static byte ALL_STATIONS = 0x00;

// Commands
const static byte MF_GET_SNR = 0x25; // MiFare Get Serial Number

const static byte REQUEST_IDLE[];

const static int TX_DELAY = 50;


    int _txPin, _rxPin;
    SoftwareSerial rfid;
    void send_command(byte stationId, byte cmd=MF_GET_SNR, const byte data[]=REQUEST_IDLE);
    String read_response();
    // States for RFID reader response parsing FSM
    enum readStates {START, STATION_ID, DATA_LENGTH, STATUS, DATA, CHECKSUM, END, DONE, ERROR};
};

#endif
