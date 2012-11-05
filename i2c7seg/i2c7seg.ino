#include <Wire.h>
#include <DS1307new.h>
// *********************************************
// VARIABLES
// *********************************************
uint16_t startAddr = 0x0000;            // Start address to store in the NV-RAM
uint16_t lastAddr;                      // new address for storing in NV-RAM
uint16_t TimeIsSet = 0xaa55;            // Helper that time must not set again
const byte addr =   0x38;
int index = 1;
int hz = 1;
unsigned int number; 
unsigned const int MaxDigit = 9999;
byte i2cData = 0x00;
unsigned long changeTime = millis();
char date = false;

void setup()
{
  Wire.begin();                   
  Serial.begin(9600);
  RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t));// Store startAddr in NV-RAM address 0x08 
  RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
  if (TimeIsSet != 0xaa55)
  {
    RTC.stopClock();
        
    RTC.fillByYMD(2012,9,13);
    RTC.fillByHMS(18,11,0);
    
    RTC.setTime();
    TimeIsSet = 0xaa55;
    RTC.setRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
    RTC.startClock();
  }
  else
  {
    RTC.getTime();
  }

/*
   Control Register for SQW pin which can be used as an interrupt.
*/
  RTC.ctrl = 0x00;                      // 0x00=disable SQW pin, 0x10=1Hz,
                                        // 0x11=4096Hz, 0x12=8192Hz, 0x13=32768Hz
  RTC.setCTRL();
  uint8_t MESZ;
  MESZ = RTC.isMEZSummerTime();
}

void deviceWrite(byte txData, byte devAddr)
{
  Wire.beginTransmission(devAddr);
  Wire.write(txData);
  Wire.endTransmission();
}

byte deviceRead(byte devAddr){
  byte readData;
  Wire.requestFrom(devAddr, (byte)1);
  if(Wire.available()) {
    readData = Wire.read();
  }
  return readData;
}

int digitSplit(int inDigit, int ix){
   //Make sure number is not greater than 4Digit7seg Max 
   i2cData = 0x00;
   if(inDigit > MaxDigit)
      number = inDigit = 0;
    int digit1 = inDigit / 1000;
    int digit2 = (inDigit - (digit1 * 1000)) / 100;
    int digit3 = (inDigit - (digit1 * 1000) - (digit2 * 100)) / 10;
    int digit4 = (inDigit - (digit1 * 1000) - (digit2 * 100) - (digit3 * 10));
    int var;
    
    if(ix == 1)
      var = digit1;
    else if (ix == 2)
      var = digit2;
    else if (ix == 3)
      var = digit3;
    else if (ix == 4)
      var = digit4;
    bitWrite(i2cData, 3, (1 && (var & B00001000)));
    bitWrite(i2cData, 2, (1 && (var & B00000100)));
    bitWrite(i2cData, 1, (1 && (var & B00000010)));
    bitWrite(i2cData, 0, (1 && (var & B00000001)));
}

void loop(){
  byte mask = 0x00;
  RTC.getTime();
  if(!date)
    number = RTC.hour *100 + RTC.minute;
  else if(date)
    number = RTC.month *100 + RTC.day;
  if( millis() - changeTime > 4000){
    date = !date;
    changeTime = millis();
  }
  digitSplit(number,index);
  switch(index){
    case 1: mask = B10000000;
            break;
    case 2: mask = B01000000;
            break;
    case 3: mask = B00100000;
            break;
    case 4: mask = B00010000;
            break;
    default: break;
  }
  i2cData = i2cData ^ mask;
  delay(hz);
  if(index >= 4){
    index = 1;
    //number ++;
  }
  else{
    index++; }
 deviceWrite(i2cData,addr);
}
