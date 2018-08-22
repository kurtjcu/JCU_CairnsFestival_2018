#include <FastLED.h>



FASTLED_USING_NAMESPACE

// Based upon FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// original works by - Mark Kriegsman, December 2014
// added 2nd output.
// random direction for 2nd output
// Kurt Schoenhoff - August 2018

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN1    5
#define DATA_PIN2    6
//#define CLK_PIN   4
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER GRB
#define NUM_LEDS1    192
#define NUM_LEDS2    90
CRGB leds1[NUM_LEDS1];
CRGB leds2[NUM_LEDS2];

#define BRIGHTNESS          254
#define FRAMES_PER_SECOND  60

//function prototypes (needed for platformio etc)
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void set2ndArray();
void set2ndArrayInverted();
void chooseRandomDirectionFor2ndArray();


void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN1>(leds1, NUM_LEDS1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN2>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //intitalize stips to 0 (black)
  for(int i = 0; i < NUM_LEDS1; i++){
    leds1[i]= CRGB::Black;
  }
  for(int i = 0; i < NUM_LEDS2; i++){
    leds2[i]= CRGB::Black;
  }
  
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool invert = false;
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 5 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);

  //sets an iverted flag for use when filling 2nd output
  if(random8(2)){
    invert = false;
  } else {
    invert = true;
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds1, NUM_LEDS1, gHue, 4);
  chooseRandomDirectionFor2ndArray();
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  chooseRandomDirectionFor2ndArray();
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds1[ random16(NUM_LEDS1) ] += CRGB::White;
  }
  chooseRandomDirectionFor2ndArray();
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds1, NUM_LEDS1, 10);
  int pos = random16(NUM_LEDS1);
  leds1[pos] += CHSV( gHue + random8(64), 200, 255);
  chooseRandomDirectionFor2ndArray();
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS1, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS1-1 );
  leds1[pos] += CHSV( gHue, 255, 192);
  set2ndArray();
  chooseRandomDirectionFor2ndArray();
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 120;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS1; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  chooseRandomDirectionFor2ndArray();
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds1, NUM_LEDS1, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds1[beatsin16( i+7, 0, NUM_LEDS1-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  chooseRandomDirectionFor2ndArray();
}

void chooseRandomDirectionFor2ndArray(){
  if(invert){
    set2ndArrayInverted();    
  } else {
    set2ndArray();
  }
}

void set2ndArray(){
  for(int i = 0; i < NUM_LEDS2; i++){
    leds2[i] = leds1[i];
  }
}

void set2ndArrayInverted(){
  int y = NUM_LEDS2 - 1;
  int x = 0;
  
  while(y){
    leds2[x] = leds1[y];
    x++;
    y--;
  }
}

