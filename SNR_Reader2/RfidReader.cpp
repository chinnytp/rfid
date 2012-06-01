#include "RfidReader.h"
#include "Arduino.h"
#include "PString.h"

const byte RfidReader::REQUEST_IDLE[2] = {0x52,0x00};

RfidReader::RfidReader(int txPin, int rxPin) : rfid(txPin, rxPin)
{
  _txPin = txPin;
  _rxPin = rxPin;
}

String RfidReader::getCode(){
 send_command(ALL_STATIONS);
 return read_response(); 
}

void RfidReader::begin(int baudRate){
  rfid.begin(baudRate);
}

void RfidReader::send_command(byte stationId, byte cmd, const byte data[])
{
  byte checksum;
  byte dataLen;
  
  rfid.write(STX);  // Start of message
  rfid.write(stationId);  // Target station id
  dataLen = sizeof(data) + 1;
  rfid.write(dataLen);  // Length of data
  rfid.write(cmd);  // Command
  
  checksum = (stationId ^ dataLen ^ cmd);
 
  int i=0;
  while (i<dataLen-1)
  {
    rfid.write(data[i]);  // Data
    checksum = checksum ^ data[i];
    i++;
  }
  rfid.write(checksum);  // Calculated checksum
  rfid.write(ETX);  // End of message

  delay(TX_DELAY);
}

String RfidReader::read_response()
{
    readStates state = START; // Setup the initial state of the FSM
    byte incomingByte;
    byte dataLen=0;
    byte checkSum = 0;
    byte statusCode = 0;
  
    char buffer[50];
    PString response(buffer, sizeof(buffer));
  
    while (rfid.available() > 0) 
    {
       if(state==DONE)
       break;
      // allow buffer to fill
      delay(3);

      incomingByte = rfid.read();
    
      // Calculate checksum for bytes between
      // START and received CHECKSUM.
      if(state > START && state < CHECKSUM){
        checkSum = checkSum ^ incomingByte;
      }

    
      switch(state){
        case START:
         // Don't start parsing the message until
         // STX is found
         if(incomingByte==STX)
           state=STATION_ID;
           break;
        case STATION_ID:
          // Station Id
          state=DATA_LENGTH;
          break;
        case DATA_LENGTH:
          dataLen=incomingByte;
          state=STATUS;
          break;
        case STATUS:
          dataLen--;
          statusCode = incomingByte;
          if(statusCode!=0x00){
            state=ERROR;
          }else{
            state=DATA; 
          }
          break;
        case DATA:
            // Add leading 0 to output, if necessary.           
            if(incomingByte<0x10)
              response.print("0");
            response.print(incomingByte, HEX);
            dataLen--;
            
          if(dataLen==0){
            state=CHECKSUM; 
          }
          break;      
        case CHECKSUM:
          if(checkSum!=incomingByte){
            state = ERROR;
          }else{
            state = END;
          }
          break;
        case END:
          if(incomingByte!=ETX)
            state = ERROR;
          else
            state = DONE;
        case DONE:
          break;
        case ERROR: // On error clear response
          response.begin();
          state = DONE;
          break;
        default:
          state = ERROR; 
          break;
      }   
  }
  return String(response);
}
