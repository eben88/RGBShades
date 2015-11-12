//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
byte sineOffset = 0; // counter for current position of sine waves
void threeSine() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*9+x*16)),2);
      byte sinDistanceG = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*10+x*16)),2);
      byte sinDistanceB = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*11+x*16)),2);

      leds[XY(x,y)] = CRGB(255-sinDistanceR, 255-sinDistanceG, 255-sinDistanceB); 
    }
  }
  
  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}

// RGB Plasma
byte offset  = 0; // counter for radial color wave motion
int plasVector = 0; // counter for orbiting plasma center
void plasma() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector/256);
  int yOffset = sin8(plasVector/256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x-7.5)*10+xOffset-127)+sq(((float)y-2)*10+yOffset-127))+offset);
      leds[XY(x,y)] = CHSV(color, 255, 255);
    }    
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
byte riderPos = 0;
void rider() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x*(256/kMatrixWidth) - triwave8(riderPos)*2 + 127)*3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x,y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic

}



// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y <kMatrixHeight; y++) {
      leds[XY(x,y)] = CHSV(cycleHue,255,random8(5)*63);
    }
  }

}


// Fills saturated colors into the array from alternating directions
byte currentColor = 0;
byte currentRow = 0;
byte currentDirection = 0;
void colorFill() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
  }

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x,y)] = currentPalette[currentColor];
    }
  } 

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x,y)] = currentPalette[currentColor];
    }
  }

  currentRow++;
  
  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3,6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }


}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
  }
  
   for (byte x = 0; x < kMatrixWidth; x++) {
     for (byte y = 0; y < kMatrixHeight; y++) {
       if (x < 7) {
         leds[XY(x,y)] = CRGB::Blue;
       } else if (x > 8) {
         leds[XY(x,y)] = CRGB::Red;
       } else {
         leds[XY(x,y)] = CRGB::Black;
       }
     }
   }
   
   leds[XY(6,0)] = CRGB::Black;
   leds[XY(9,0)] = CRGB::Black;

}


uint8_t threeDeeBlinkState = 0;
// Just like threeDee, except it flip-flops red/blue rapidly
void threeDeeBlink() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 100;
  }

  CRGB left;
  CRGB right;

  if (threeDeeBlinkState == 0) {
    left = CRGB::Red;
    right = CRGB::Blue;
  } else {
    left = CRGB::Blue;
    right = CRGB::Red;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = left;
      } else if (x > 8) {
        leds[XY(x, y)] = right;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;

  threeDeeBlinkState = (threeDeeBlinkState + 1) % 2;
}




// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }
  
  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth-1) * rainDir,y)] = CRGB::Black;
  leds[XY((kMatrixWidth-1)*rainDir, randPixel)] = CHSV(cycleHue, 255, 255); 

}

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
    leds[XY(random16(kMatrixWidth),random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255);//CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }
   
}


// Draw slanting bars scrolling across the array, uses current hue
byte slantPos = 0;
void slantBars() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x,y)] = CHSV(cycleHue, 255, quadwave8(x*32+y*32+slantPos));
    }
  }

  slantPos-=4;

}

const uint8_t OutlineTable[] = {
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 43,
   44, 67, 66, 65, 64, 63, 50, 37, 21, 22, 36, 51, 62, 61, 60, 59,
   58, 57, 30, 29};
const uint8_t outlineTableLen = 36;


//leds run around the periphery of the shades, changing color every go 'round
boolean erase = false;
uint8_t x,y = 0;
void shadesOutline(){
  //startup tasks
    if (effectInit == false) {
    effectInit = true;
    erase = false;
    uint8_t x=0;
    effectDelay = 15; 
    FastLED.clear();
    currentPalette = RainbowColors_p;}
    


    leds[OutlineTable[x]] = currentPalette[currentColor];
  if (erase)
    leds[OutlineTable[x]] = CRGB::Black;
   x++; 
  if (x == (sizeof(OutlineTable))) {
    erase = !erase;
    x = 0;
   currentColor += random8(3,6);
   if (currentColor > 15) currentColor -= 16;  
  } 
}


// Like shadesOutline, except the light trail has a set length instead of
// looping all the way around the glasses
uint8_t pos, toOn, toOff;
const uint8_t gapLen = 24;  // The number of dark pixels around the outline
void shadesOutlineChase() {
  if (effectInit == false) {
    effectInit = true;
    erase = false;
    effectDelay = 20;
    pos = 0;
    FastLED.clear();
    currentPalette = PartyColors_p;
    for (uint8_t i = 0; i < outlineTableLen; i++) {
      leds[OutlineTable[i]] == currentPalette[currentColor];
    }
  }

  toOff = pos;
  toOn = (pos + outlineTableLen - gapLen) % outlineTableLen;

  leds[OutlineTable[toOff]] = CRGB::Black;
  leds[OutlineTable[toOn]] = currentPalette[currentColor];

  pos = (pos + 1) % outlineTableLen;
  currentColor = (currentColor + 1) % 16;
}



//hearts that start small on the bottom and get larger as they grow upward
const uint8_t SmHeart[] = {46, 48, 53, 55, 60, 65};
const uint8_t MedHeart[] = {31, 32, 34, 35, 38, 39, 
   41, 42, 46, 47, 48, 55, 54, 53, 54, 55, 60, 65};
const uint8_t LrgHeart[] = {15, 16, 18, 19, 24, 25, 
   27, 28, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42, 
   46, 47, 48, 53, 54, 55, 60, 65};
const uint8_t HugeHeart[] = {0, 1, 3, 4, 9, 10, 12, 
   13, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26, 
   27, 28, 29, 31, 32, 33, 34, 35, 38, 39, 40, 41, 
   42, 46, 47, 48, 53, 54, 55, 60, 65};
void hearts() {
   if (effectInit == false) {
    effectInit = true;
    effectDelay = 250; 
    FastLED.clear();
   int x = 0;
   int y = 0;
    }
    effectDelay = 250;
    if (y==5)
     y = 0;
    if (y == 0)
     for (x = 0; x < 6; x++)
      leds[SmHeart[x]] = CRGB::White; //Tried to transition from pink-ish to red. Kinda worked.
    if (y == 1)
     for (x = 0; x < 18; x++) 
      leds[MedHeart[x]] = CRGB::Green;
    if (y == 2)
     for (x = 0; x < 26; x++)
      leds[LrgHeart[x]] = CRGB::Blue;
    if (y == 3){
     for (x = 0; x < 40; x++)
      leds[HugeHeart[x]] = CRGB::Red;
      effectDelay = 450;} //set the delay slightly longer for HUGE heart.
    if (y == 4)
     FastLED.clear();
  y++;
}

#define NORMAL 0
#define RAINBOW 1
#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, CRGB fgColor, CRGB bgColor) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte bitBuffer[16] = {0};
  static byte bitBufferPointer = 0;


  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    loadCharBuffer(loadStringChar(message, currentMessageChar));
    currentPalette = RainbowColors_p;
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;
  }


  paletteCycle += 15;

  if (currentCharColumn < 5) { // characters are 5 pixels wide
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = charBuffer[currentCharColumn]; // character
  } else {
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = 0; // space
  }

  CRGB pixelColor;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[(bitBufferPointer + x) % kMatrixWidth], y) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = fgColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(x, y)] = pixelColor;
    }
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) { // null character at end of strong
      currentMessageChar = 0;
      nextChar = loadStringChar(message, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

  bitBufferPointer++;
  if (bitBufferPointer > 15) bitBufferPointer = 0;

}


void scrollTextZero() {
  scrollText(0, NORMAL, CRGB::Red, CRGB::Black);
}

void scrollTextOne() {
  scrollText(1, RAINBOW, 0, CRGB::Black);
}

void scrollTextTwo() {
  scrollText(2, NORMAL, CRGB::Green, CRGB(0,0,8));
}

// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman
//
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
//
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
//
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
//

// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
#define SECONDS_PER_PALETTE 10

// Forward declarations of an array of cpt-city gradient palettes, and 
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );

// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void drawcolorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds-1) - pixelnumber;
    
    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

void colorwaves()
{
  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
  }
  
  drawcolorwaves( leds, NUM_LEDS, gCurrentPalette);
}


//Peace signs
const uint8_t Peace[] = {1, 2, 3, 28, 26, 24, 31, 33, 35, 56, 55, 53, 52, 59, 60, 61,
   10, 11, 12, 19, 17, 15, 38, 40, 42, 49, 48, 46, 45, 64, 65, 66};
void peace() {
   if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
    FastLED.clear();
   }
   for (int x = 0; x < 32; x++) {
    leds[Peace[x]] = CHSV(cycleHue, 255, 255);
   }
}


//Pacman
const uint8_t Ghost[] = {10, 11, 12, 19, 17, 15, 38,
   39, 40, 41, 42, 49, 48, 47, 46, 45, 63, 65, 67};
const uint8_t PacManClosed[] = {1, 2, 3, 28, 27, 26, 25,
   24, 31, 32, 33, 34, 35, 56, 55, 54, 53, 52, 59, 60, 61};
const uint8_t PacManMouth[] = {25, 24, 33, 34, 35, 53, 52};
const uint8_t Pellets1[] = {34, 36};
const uint8_t Pellets2[] = {35, 37, 43};
int pacman_step = 0;
void pacman() {
   if (effectInit == false) {
    effectInit = true;
    effectDelay = 175;
    FastLED.clear();
   }
   if (pacman_step==3)
    pacman_step = 0;
   if (pacman_step == 0) {
    for (int x = 0; x < 2; x++) {
     leds[Pellets1[x]] = CRGB::White;
    }
    for (int x = 0; x < 3; x++) {
     leds[Pellets2[x]] = CRGB::Black;
    }
    for (int x = 0; x < 21; x++) {
     leds[PacManClosed[x]] = CRGB::Yellow;
    }
    for (int x = 0; x < 7; x++) {
     leds[PacManMouth[x]] = CRGB::Black;
    }
    for (int x = 0; x < 19; x++) {
     leds[Ghost[x]] = CRGB::Blue;
    }
   }
   if (pacman_step == 1) {
    for (int x = 0; x < 2; x++) {
     leds[Pellets1[x]] = CRGB::Black;
    }
    for (int x = 0; x < 3; x++) {
     leds[Pellets2[x]] = CRGB::White;
    }
   }
   if (pacman_step == 2) {
    for (int x = 0; x < 2; x++) {
     leds[Pellets1[x]] = CRGB::White;
    }
    for (int x = 0; x < 3; x++) {
     leds[Pellets2[x]] = CRGB::Black;
    }
    for (int x = 0; x < 21; x++) {
     leds[PacManClosed[x]] = CRGB::Yellow;
    }
   }
   pacman_step++;
}


//BLINK LEFT AND RIGHT


static bool in = true;
static int count = 15;
void blinkLeft(){
  CRGB color = CRGB::Red;
  if (effectInit == false){
    in=true;
    count=0;
    fillAll(CRGB::Black);
    effectInit = true;
    effectDelay = 30;
  }

  if(in){
    fillCol(count, CRGB::Black);
    count++;
  }else{
    fillCol(count, color);
    count--;
  }
  if(count==8){
    in = false;
  }
  if(count==0){
    in = true;
  }
}

void blinkRight(){
  CRGB color = CRGB::Red;
  if (effectInit == false){
    in=true;
    count=15;
    fillAll(CRGB::Black);
    effectInit = true;
    effectDelay = 30;
  }

  if(in){
    fillCol(count, CRGB::Black);
    count--;
  }else{
    fillCol(count, color);
    count++;
  }
  if(count==7){
    in = false;
  }
  if(count==15){
    in = true;
  }
}

