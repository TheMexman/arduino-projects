#include <Wire.h>

const byte addr =   0x38;
const byte rows[4] = {0xF8,0xF4,0xF2,0xF1};
const char keypad[4][4] ={ {'1','2','3','A'},
                           {'4','5','6','B'},
                           {'7','8','9','C'},
                           {'*','9','#','D'}
};
unsigned long lastTime = millis();
unsigned int diff = 500;

void setup()
{
  Wire.begin();                   // Join I2C bus
  Serial.begin(9600);
  delay(diff);
}

void loop()
{
  int i;
  for(i=0;i<4;i++){
      deviceWrite(rows[i]);         // Turn on LEDs connected to P1 and P2
      byte  key = deviceRead(addr);
      key  = key >> 4;
   if (key != 0){
        switch (key){
          case 8: key=0; break;
          case 4: key=1;break;
          case 2: key=2;break;
          case 1: key=3;break;
          default: break;
        }
       if( (millis() - lastTime) >= 500){
            Serial.println(keypad[i][key]);
            lastTime = millis();
          }
     }
  }
}

void deviceWrite(byte txData)
{
  Wire.beginTransmission(addr);
  Wire.write(txData);
  Wire.endTransmission();
}

byte deviceRead(byte addr){
  byte readData;
  Wire.requestFrom(addr, (byte)1);
  if(Wire.available()) {
    readData = Wire.read();
  }
  return readData;
}
