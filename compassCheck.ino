void compassCheck() {
  // if millis() or timer wraps around, we'll just reset it
  if (compassTimer > millis()) compassTimer = millis();

  // approximately every 10 seconds or so, update time
  if (millis() - compassTimer > 50) {
    compassTimer = millis(); // reset the timer
    compass.read();
    compassReading = compass.heading((LSM303::vector<int>){
      0,-1,0    }
    );
    correctedHeading = (compassReading + 630) % 360;
  }
  //  Serial.print("Compass Reading: "); 
  //  Serial.println(compassReading);
/*
  Serial.print("Corrected Heading: "); 
  Serial.println(correctedHeading);
*/
}

/*
void compassMode() {
  dirLED_r = 0;
  dirLED_g = 120;
  dirLED_b = 200;
  compassDirection(compassReading);   
}
*/

void headingMode() {

  if ((int)GPS.fix == 1){   // change to "good" color
    dirLED_r = 0;
    dirLED_g = 255;
    dirLED_b = 130;
  } 
  else {  // "bad" color
    dirLED_r = 255;
    dirLED_g = 0;
    dirLED_b = 0;
  }
  int showHeading = (compassReading - (int)ourHeading + 360) % 360;
  compassDirection(showHeading); 
}  


void compassDirection(int compassHeading) 
{
  //Serial.print("Compass Direction: ");
  //Serial.println(compassHeading);

  unsigned int ledDir = 2;
  int tempDir = 0;
  //Use this part of the code to determine which way you need to go.
  //Remember: this is not the direction you are heading, it is the direction to the destination (north = forward).
  if ((compassHeading > 348.75)||(compassHeading < 11.25)) {
    if (mode == 1 ) {
      tempDir = topLED;
    } 
    else {
      tempDir = topLED;
    }
  }

  if ((compassHeading >= 11.25)&&(compassHeading < 33.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 1;
    } else {
      tempDir = topLED + 1;
    }
  }  

  if ((compassHeading >= 33.75)&&(compassHeading < 56.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 2;
    } 
    else {
      tempDir = topLED + 2;
    }
  }

  if ((compassHeading >= 56.25)&&(compassHeading < 78.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 3;
    } 
    else {
      tempDir = topLED + 3;
    }
  }

  if ((compassHeading >= 78.75)&&(compassHeading < 101.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 4;
    } 
    else {
      tempDir = topLED + 4;
    }
  }

  if ((compassHeading >= 101.25)&&(compassHeading < 123.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 5;
    } 
    else {
      tempDir = topLED + 5;
    }
  }

  if ((compassHeading >= 123.75)&&(compassHeading < 146.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 6;
    } 
    else {
      tempDir = topLED + 6;
    }
  }

  if ((compassHeading >= 146.25)&&(compassHeading < 168.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 7;
    } 
    else {
      tempDir = topLED + 7;
    }
  }

  if ((compassHeading >= 168.75)&&(compassHeading < 191.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 8;
    } 
    else {
      tempDir = topLED + 8;
    }
  }

  if ((compassHeading >= 191.25)&&(compassHeading < 213.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 9;
    } 
    else {
      tempDir = topLED + 9;
    }
  }

  if ((compassHeading >= 213.75)&&(compassHeading < 236.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 10;
    } 
    else {
      tempDir = topLED + 10;
    }
  }

  if ((compassHeading >= 236.25)&&(compassHeading < 258.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 11;
    } 
    else {
      tempDir = topLED + 11;
    }
  }

  if ((compassHeading >= 258.75)&&(compassHeading < 281.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 12;
    } 
    else {
      tempDir = topLED + 12;
    }
  }

  if ((compassHeading >= 281.25)&&(compassHeading < 303.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 13;
    } 
    else {
      tempDir = topLED + 13;
    }
  }

  if ((compassHeading >= 303.75)&&(compassHeading < 326.25)) {
    if (mode == 1 ) {
      tempDir = topLED - 14;
    } 
    else {
      tempDir = topLED + 14;
    }
  }

  if ((compassHeading >= 326.25)&&(compassHeading < 348.75)) {
    if (mode == 1 ) {
      tempDir = topLED - 15;
    } 
    else {
      tempDir = topLED + 15;
    }
  }

  if (tempDir > 15) {
    ledDir = tempDir - 16;
  } 
  else {
    ledDir = tempDir;
  }

  if (tempDir < 0) {
    ledDir = tempDir + 16;
  } 
  else {
    ledDir = tempDir;
  }

  if (mode == 1) {
    ledDir = ledDir + compassOffset;
    if (ledDir > 15) {
      ledDir = ledDir - 16;
    }
  } 
  else {
    ledDir = ledDir + compassOffset;
    if (ledDir > 15) {
      ledDir = ledDir - 16;
    }
  }

  if (lastDir != ledDir) {
    strip.setPixelColor(lastDir, strip.Color(0, 0, 0));
    strip.setPixelColor(ledDir, strip.Color(dirLED_r, dirLED_g, dirLED_b));
    strip.show();
    lastDir = ledDir;
  }
}
