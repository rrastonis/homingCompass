#include <Wire.h>    
 
#define disk1 0x50    //Address of 24LC256 eeprom chip
 
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();  
 Serial.write("Testing");
 delay(5000);
 
  unsigned int address = 0;
 
  // for (int i = 0; i < 100; i++) {
  //   writeEEPROM(disk1, i, i);
  // }
  // Serial.println("Back from writes.");
  for (int j = 0; j < 700; j++) {
    for (int i = 0; i < 38; i++) {
      Serial.write(readEEPROM(disk1, (38 * j) + i));
      // Serial.print(" ");
      // if ((i % 10) == 9) Serial.println();
    }
    // Serial.println(readEEPROM(disk1, address), DEC);
    // Serial.println("");
  }
}
 
void loop(){}
 
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(2);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}
