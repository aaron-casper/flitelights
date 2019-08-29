"#include <Keyboard.h>

//airplane lights
//ripped from strandtest and heavily cut down.
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
 
#define PIN 6
#define PIN2 5
#define PIN3 7
const int timedelay = 25;
const int striplength = 18; 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(striplength, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2= Adafruit_NeoPixel(striplength, PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3= Adafruit_NeoPixel(striplength, PIN3, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip2.begin();
  strip3.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip2.show();
  strip3.show();
  
}
 
void rainbow(uint8_t wait) {
uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip3.numPixels(); i++) {
      strip3.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip3.show();
    delay(wait);
  }
}
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip3.numPixels(); i++) {
      strip3.setPixelColor(i, Wheel(((i * 256 / strip3.numPixels()) + j) & 255));
    }
    strip3.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip3.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip3.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip3.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// MAIN LOOP
void loop() {
int i = 0;

while (i < striplength){
      strip.setPixelColor(i,255,255,196);
      strip2.setPixelColor(i,255,255,196);
      strip3.setPixelColor(i,196,196,255);
      strip.show(); 
      strip2.show(); 
      strip3.show();
      //rainbow(40);
      delay(timedelay);
      i++;
 }
}
