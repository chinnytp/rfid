#include <PString.h>
#include <SoftwareSerial.h>

/*
RFID  -> ARDUINO
===================
TX    -> TX (pin 4)
RX    -> RX (pin 3)
+5V   -> +5V
GND   -> GND

*/

SoftwareSerial rfid(4, 3);

#define STX 0xAA
#define ETX 0xBB

#define STRIKER 13

// Commands
const byte MF_GET_SNR = 0x25; // MiFare Get Serial Number

byte REQUEST_IDLE[2] = {0x52,0x00};

const int TX_DELAY = 50;

String currentCode = "";

void send_command(byte stationId, int cmd, byte data[]=REQUEST_IDLE)
{
  int i;  //an index for the data send loop
  int checksum;
  int dataLen;
  
  rfid.write(STX);
  rfid.write(stationId);
  
  dataLen = sizeof(data) + 1;
  rfid.write(dataLen);
  rfid.write(cmd);
  
  checksum = (stationId ^ dataLen ^ cmd);

  i=0;  
  while (i<dataLen-1)
  {
    rfid.write(data[i]);
    checksum = checksum ^ data[i];
    i++;
  }
  rfid.write(checksum);
  rfid.write((byte) ETX);

  delay(TX_DELAY);
}

PString read_response()
{
  byte incomingByte;
  int byteCount=0;
  int dataLen=0;
  int checkSum = 0;
  
  boolean error = false;
  boolean etxExpected = false;
  
  char buffer[50];
  PString mystring(buffer, sizeof(buffer));
    
  while (rfid.available() > 0) 
  {
        // read the incoming byte:
    incomingByte = rfid.read();
    
    switch(byteCount){
     case 0:
        if(incomingByte!=STX)
          error=true;
          break;
      case 1:
      // Station Id
        break;
      case 2:
        dataLen=incomingByte;
        break;
      case 3:
        dataLen--;
        if(incomingByte!=0x00){
          error=true;
        }
          break;
      default:
       if(dataLen>0){
           if(!error){
             if(incomingByte<0x10)
               mystring.print("0");
             mystring.print(incomingByte, HEX);
           }
           dataLen--;
       }else{
         checkSum = incomingByte; 
       }  
      break;
    }   

    byteCount++;    
  }
  return mystring;
}

void setup()
{
  //start serial port at 9600 bps: This is for the debug info we send to your PC/Mac
  Serial.begin(9600);
  //start a NewSoftSerial serial port at 9600bps : This is to talk to the RFID.
  rfid.begin(9600);
}

void loop()
{
  send_command(0x00, MF_GET_SNR);
  PString res = read_response();
  String readString;  
    
    
  if(res.length()>0){
    if(currentCode.equals("") || !currentCode.equals(res)){
      Serial.print("CARD CODE ");
      Serial.print(res);
      Serial.println();
      currentCode = res;
    }
    
    while (Serial.available()>0) {
      delay(3);  //delay to allow buffer to fill 
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
    
      if(readString == "UNLOCK\r\n"){
        readString = "";
        Serial.println("STRIKER OPEN");
        digitalWrite(STRIKER, HIGH);
        delay(5000);
        digitalWrite(STRIKER, LOW);
        Serial.println("STRIKER CLOSED");
      }
  }else{
    if(!currentCode.equals("")){
       Serial.print("CARD REMOVED");
       Serial.println();
       currentCode="";
    } 
  }
  delay(200);
}

