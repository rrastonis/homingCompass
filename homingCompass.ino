/* Built by Ramune Rastonis using construction and some code from Adafruit's Flora NeoGeo Watch: https://learn.adafruit.com/flora-geo-watch
as well as help from CCA thesis advisor, Scott Minneman, and friend, Adam Coogan
*/

#include <Adafruit_GPS.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>
#include <Wire.h>
#include <LSM303.h>

Adafruit_GPS GPS(&Serial1);
LSM303 compass;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

#define FULLON 255 //light strength

#define VERBOSE //ifdef to make code smaller

#define ADDRESS 0x50 //EEPROM address

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

#define PIN 6 //NeoPixel Ring light

// Enter top NeoPixel LED (0-15)
#define TOP_LED 0

// Point TOP_LED north, count clockwise how many LEDs away from TOP_LED the lit LED is
#define LED_OFFSET 12

// Assign a unique ID to this sensor at the same time
Adafruit_LSM303_Mag mag = Adafruit_LSM303_Mag(12345);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, 6, NEO_GRB + NEO_KHZ800);

// VARIABLES
float i = 0;

/* HOME COORDINATES */
// California College of the Arts - San Francisco, CA
float latH = 37.767135;
float lonH = -122.399999;

// CURRENT COORDINATES
float latC;
float lonC;

// angle between device and north
float alphaH = 0;
float alphaC = 0;

float ourHeading;
sensors_event_t magEvent;
int correctedHeading;
int compassReading;

// Convert to spherical coordinates
float t, th, p, ph;

// Your position
float vx, vy, vz;

// Apply Rz(-alpha) Ry(-th).  This rotates the home point to the north pole.
float vppx, vppy, vppz;

// Now find the tangent vector pointing north.
float vhppx, vhppy, vhppz;

// Transform the home vector to the original frame.
float vhx, vhy, vhz;

// Get angle between your home vector and the vector pointing from you to the north pole.
float vnx, vny, vnz;

// Trip distance
float tripDistance;

// NeoPixel Ring
int topLED = TOP_LED;
int compassOffset = LED_OFFSET;
int startLED = 0;
int startLEDlast = 16;
int start = 0;
int mode = 0;
int lastDir = 16;
int dirLED_r = 0, dirLED_g = 0, dirLED_b = 0;

// Compass parts
uint32_t gpsTimer = millis();
uint32_t startupTimer = millis();
uint32_t compassTimer = millis();

// Pushbutton setup
int buttonPin = 10;             // the number of the pushbutton pin
int buttonState;               // the current reading from the input pin
int lastButtonState = HIGH;    // the previous reading from the input pin
long buttonHoldTime = 0;         // the last time the output pin was toggled
long buttonHoldDelay = 2500;      // how long to hold the button down

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;     // the last time the output pin was toggled
long debounceDelay = 50;       // the debounce time; increase if the output flickers
long menuDelay = 2500;
long menuTime;

//EEPROM
char buffer[50], latbuf[12], lonbuf[12];
int eepromLocation = 0, strlength;
float mylat = 37.85, mylon = -48.25;
long lastTime = 0;

void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy
