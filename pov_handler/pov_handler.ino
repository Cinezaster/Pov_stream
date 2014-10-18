#include <Adafruit_NeoPixel.h>
 
#define PIN             5
#define N_LEDS          40
#define intTriggerPin   3
#define circleDivisions 40
#define spokes          4
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

elapsedMicros oneTurnElapsedTimer;
IntervalTimer circleDivisionIntervalTimer;

int oneTurnElapsedTime;
int currentActiveDivision;

void oneTurnTime() {
  oneTurnElapsedTime = oneTurnElapsedTimer;
  oneTurnElapsedTimer = 0;
  circleDivisionIntervalTimer.end();
  circleDivisionIntervalTimer.begin(nextDivision,oneTurnElapsedTime/circleDivisions);
  currentActiveDivision = 0;
}

void nextDivision() {
  currentActiveDivision++;
}
 
void setup() {
  Serial.begin(115200);

  pinMode(intTriggerPin, INPUT); // sets the digital pin as output
  attachInterrupt(intTriggerPin, oneTurnTime, FALLING); // interrrupt 1 is data ready

  strip.begin();
}


 
void loop() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
  }
  strip.show();
  strip.setPixelColor(discPosition(currentActiveDivision)  , strip.Color(255, 0, 0)); // Draw new pixel
  strip.show();
  if (currentActiveDivision > 40) {
    currentActiveDivision = 0;
  }
  
  Serial.println(oneTurnElapsedTime);
}

int discPosition(int pos){
  int div = pos/spokes;
  int rest = ((pos+spokes)%spokes)*(N_LEDS/spokes);
  return rest+div;
}