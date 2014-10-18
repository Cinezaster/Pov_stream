#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, 5, NEO_GRB + NEO_KHZ800);

volatile unsigned long disk_speed, disk_timer;
volatile boolean start;

unsigned long loop_speed, loop_timer;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(3,INPUT);
  attachInterrupt(3, interrupt, FALLING);
  
}

void loop() {
  
  
  loop_speed=micros()-loop_timer;
  //while(!start) {}
  start=false;  
  loop_timer=micros();
  
  
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  
 rainbow(1);
 rainbow(2);
 rainbow(3);
 rainbow(5);
 rainbow(5);
 rainbow(10);
 rainbow(20);
 
 // rainbowCycle(20);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      int k=(i%4)*10+int(i/4);
      strip.setPixelColor(k, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      int k=(i%4)*10+int(i/4);
      strip.setPixelColor(k, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      int k=(i%4)*10+int(i/4);
      strip.setPixelColor(k, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void interrupt() {

  Serial.println("IRQ");
  
  disk_speed=micros()-disk_timer;
  disk_timer=micros();
  start=true;
}
