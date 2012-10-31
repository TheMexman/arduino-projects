#include <Wire.h>

const byte addr =   0x38;
  byte leds = B11111111;
  byte mask = B01111111;
void setup()
{
  Wire.begin();                   

}

void loop()
{
  deviceWrite(leds^mask,addr);
  mask = mask >> 1;
  if(mask == 0) mask = 128;
  delay(1000); 

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
