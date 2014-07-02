void setup()  
{

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);

  lastTime = millis();

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  //  useInterrupt(true);

  delay(100);
  // Ask for firmware version
  Serial1.println(PMTK_Q_RELEASE);

  /* Initialise the MAGNETOMETER */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  compass.init();
  compass.enableDefault();

  // Compass Calibration ... run compass callibration code to get correct numbers
  compass.m_min.x = -152; 
  compass.m_min.y = -360; 
  compass.m_min.z = -290;
  compass.m_max.x = +669; 
  compass.m_max.y = +376; 
  compass.m_max.z = +407;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(40);

  // Make input & enable pull-up resistors on switch pins for pushbutton
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);   

}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
  // writing direct to UDR0 is much much faster than Serial.print 
  // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } 
  else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();

void loop()
{
  int i;
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    //    readMag();
    compassCheck();

    buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) {
      buttonCheck();
    }

    lastButtonState = buttonState;

    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //   Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer

    /*
    Serial.print("\nTime: ");
     Serial.print(GPS.hour, DEC); 
     Serial.print(':');
     Serial.print(GPS.minute, DEC); 
     Serial.print(':');
     Serial.print(GPS.seconds, DEC); 
     Serial.print('.');
     Serial.println(GPS.milliseconds);
     Serial.print("Date: ");
     Serial.print(GPS.day, DEC); 
     Serial.print('/');
     Serial.print(GPS.month, DEC); 
     Serial.print("/20");
     Serial.println(GPS.year, DEC);
     Serial.print("Fix: "); 
     Serial.print((int)GPS.fix);
     Serial.print(" quality: "); 
     Serial.println((int)GPS.fixquality);
     */
    if (GPS.fix) {
      ourHeadingCalc();
    }
  }
  // compassMode();
  headingMode();

  if ((millis() - lastTime) > 5000) {
  
    /* Logging Data to EEPROM */
    dtostrf(latC,9,4,latbuf);
    dtostrf(lonC,9,4,lonbuf);

    strlength = sprintf(buffer,"%s,%s,%02d/%02d/%02d,%02d:%02d:%02d\n",latbuf,lonbuf,GPS.month,GPS.day,GPS.year,GPS.hour,GPS.minute,GPS.seconds);
    Serial.print(buffer);

    for (i = 0; i < 38; i++) {
      eepromWrite((byte)((eepromLocation + i) >> 8),(byte)((eepromLocation + i) & 0xFF),buffer[i]);
      delay(5);
    }
    eepromLocation += 38;
    lastTime = millis();
  }
}

void eepromWrite(byte highAddress, byte lowAddress, byte data) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(highAddress);
  Wire.write(lowAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void ourHeadingCalc() {
  latC = decimalDegrees(GPS.latitude, GPS.lat);
  lonC = decimalDegrees(GPS.longitude, GPS.lon);

  /*
  Serial.print("Location: ");
   Serial.print(latC,4);
   Serial.print(", "); 
   Serial.println(lonC,4);
   Serial.print("Home Location: ");
   Serial.print(latH,4);
   Serial.print(", "); 
   Serial.println(lonH,4);
   
   Serial.print("ButtonState: ");
   Serial.println(buttonState);
   
   Serial.print("Satellites: "); 
   Serial.println((int)GPS.satellites);
   Serial.println("");
   */

  /* THE MATH */      
  // Taking the arccos of the dot product gives an angle between
  // 0 and pi, but we want a compass heading:

  // Convert to spherical coordinates
  t = (90.0 - latC) * PI/180.0;
  th = (90.0 - latH) * PI/180.0;
  p = lonC * PI/180.0;
  ph = lonH * PI/180.0;

  // Your position
  vx = cos(p) * sin(t);
  vy = sin(p) * sin(t);
  vz = cos(t);

  // Apply Rz(-alpha) Ry(-th).  This rotates the home point to the north pole.
  vppx = vx * cos(ph) * cos(th) + vy * sin(ph) * cos(th) - vz * sin(th);
  vppy = vy * cos(ph) - vx * sin(ph);
  vppz = vx * cos(ph) * sin(th) + vy * sin(ph) * sin(th) + vz * cos(th);

  // Now find the tangent vector pointing north.
  vhppx = vppx * (-vppz) / sqrt(1.0 - vppz*vppz);
  vhppy = vppy * (-vppz) / sqrt(1.0 - vppz*vppz);
  vhppz = sqrt(1.0 - vppz*vppz);

  // Transform the home vector to the original frame.
  vhx = vhppx * cos(ph) * cos(th) + vhppz * cos(ph) * sin(th) - vhppy * sin(ph);
  vhy = vhppx * sin(ph) * cos(th) + vhppz * sin(ph) * sin(th) + vhppy * cos(ph);
  vhz = vhppz * cos(th) - vhppx * sin(th);

  // Get angle between your home vector and the vector pointing from you to the north pole.
  vnx = vx * (-vz) / sqrt(1.0 - vz*vz);
  vny = vy * (-vz) / sqrt(1.0 - vz*vz);
  vnz = sqrt(1.0 - vz*vz);

  if (p <= ph) {
    alphaH = 180.0 / PI * acos(vnx*vhx + vny*vhy + vnz*vhz);
  }
  else {
    alphaH = 360.0 - 180.0 / PI * acos(vnx*vhx + vny*vhy + vnz*vhz);
  }

  ourHeading = alphaH - alphaC;
  // Serial.print("Heading: ");
  // Serial.println(ourHeading);
}

void buttonCheck() {
  menuTime = millis();
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonHoldTime = millis();
  }

  //Change Home Location to Current Location, once
  if (buttonState == LOW && lastButtonState == LOW) {
    if ((millis() - buttonHoldTime) > buttonHoldDelay) {
      latH = latC;
      lonH = lonC;
      colorWipe(strip.Color(245, 200, 20), 50); // Orange
      colorWipe(strip.Color(0, 0, 0), 50); // blank
      buttonHoldTime = millis();
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Convert NMEA coordinate to decimal degrees
float decimalDegrees(float nmeaCoord, char dir) {
  uint16_t wholeDegrees = 0.01*nmeaCoord;
  int modifier = 1;

  if (dir == 'W' || dir == 'S') {
    modifier = -1;
  }

  return (wholeDegrees + (nmeaCoord - 100.0*wholeDegrees)/60.0) * modifier;
}
