#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <DFPlayer_Mini_Mp3.h>
SoftwareSerial R2Serial(10, 11); // RX, TX
Servo servo1;

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN   5
#define LED_COUNT 7
#define SERVO_PIN 3
#define VOICE_VOLUME 20


/*
 *  Details on setting HUE color values (and color wheel) for NeoPixel can be found here:
 *  https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
 * 
 */
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


// *** Gaussian Random Emulator ***
int getGaussianRand(int lo, int hi)
{
  if ( lo < hi )
  {
    int range = hi - lo;
    int val = (rand() % range + lo);
    val += (rand() % range + lo);
    val += (rand() % range + lo);
    val += (rand() % range + lo);

    float fVal = (float)val * 0.25f;
    return (int) fVal;
  }
  else
  {
    return 0;
  }
}

// *** class to handle LED data ***
// Start R2LED Class
class R2LED {
  public:
    int id = 0;
    bool running = false;
    bool hsv_mode = false;
    bool switch_mode = false;
    int blue = 0;
    int red = 0;
    int green = 0;
    int white = 0;
    long hue = 0;
    int saturation = 0;
    int brightness = 0;
    int blueTarget = 0;
    int redTarget = 0;
    int greenTarget = 0;
    int whiteTarget = 0;
    long hueTarget = 0;
    int saturationTarget = 0;
    int brightnessTarget = 0;
    
    long duration = 0;
    long loopStep = 0;
    R2LED(int ledId);
    void setColor(int r, int g, int b);
    void setColorTarget(int r, int g, int b);
    void setLed();
    void nextStep();
    void setColorHSV(long hue, int saturation, int brightness);
    void setColorTargetHSV(long hue, int saturation, int brightness);
  private:
    int blueStart = 0;
    int redStart = 0;
    int greenStart = 0;
    int whiteStart = 0;
    long hueStart = 0;
    int saturationStart = 0;
    int brightnessStart = 0;
    
};
R2LED::R2LED( int ledId) {
  id = ledId;
  running = false;
}
void R2LED::setLed() {
  strip.setPixelColor(id, red, green, blue);
  
}
void R2LED::setColorHSV(long h, int s, int b) {
  hue = h;
  saturation = s;
  brightness = b;
  uint32_t color = strip.ColorHSV(hue, saturation, brightness);
  uint8_t result[4];
  blue = (color & 0x000000ff);
  green = (color & 0x0000ff00) >> 8;
  red = (color & 0x00ff0000) >> 16;
  white = (color & 0xff000000) >> 24;
  hsv_mode = true;
  strip.setPixelColor(id, color);
}
void R2LED::setColorTargetHSV(long h, int s, int b) {
  hueTarget = h;
  saturationTarget = s;
  brightnessTarget = b;
  uint32_t color = strip.ColorHSV(hueTarget, saturationTarget, brightnessTarget);
  uint8_t result[4];
  blueTarget = (color & 0x000000ff);
  greenTarget = (color & 0x0000ff00) >> 8;
  redTarget = (color & 0x00ff0000) >> 16;
  whiteTarget = (color & 0xff000000) >> 24;
  hsv_mode = true;
}
void R2LED::setColor(int r, int g, int b) {
  red = r;
  green = g;
  blue = b;
  strip.setPixelColor(id, red, green, blue);
}
void R2LED::setColorTarget (int r, int g, int b) {
  redTarget = r;
  greenTarget = g;
  blueTarget = b;
}
void R2LED::nextStep () {
  if (loopStep == 0) {
    redStart = red;
    greenStart = green;
    blueStart = blue;
    hueStart = hue;
    saturationStart = 255;
    brightnessStart = 255;
    running = true;
  }
  loopStep ++;
  if (loopStep >= duration) {
    running = false;
    loopStep = 0;
  } else {
    double progress = (static_cast<double>(loopStep) / static_cast<double>(duration));
    if (!hsv_mode) {
      // simple Color Fade Mode
      int r = redStart + ((redTarget - redStart) * progress);
      int g = greenStart + ((greenTarget - greenStart) * progress);
      int b = blueStart + ((blueTarget - blueStart) * progress);
      setColor(r,g,b);
    } else {
      long h = hueStart + ((hueTarget - hueStart) * progress);
      int s = saturationStart + ((saturationTarget - saturationStart) * progress);
      int b = brightnessStart + ((brightnessTarget - brightnessStart) * progress);
      setColorHSV(h,s,b);
    }
  }
}
// End R2LED Class

// *** initialize LED objects ***
R2LED r2_Logic1(0);
R2LED r2_Logic2(1);
R2LED r2_FrontPSI(2);
R2LED r2_RearPSI(3);
R2LED r2_Logic3A(4);
R2LED r2_Logic3B(5);
R2LED r2_Logic3C(6);

class R2Servo {
  public:
    int heading = 0;
    int targetHeading = 0;
    int duration = 0;
    int maxHeading = 180;
    int minHeading = 0;
    long loopStep = 0;
    bool isIdle = true;
    void moveTo(int h);
    void idle();
    void conn();
    void nextStep();
};
void R2Servo::nextStep () {
  loopStep ++;

  if (loopStep == 800) {
    digitalWrite(LED_BUILTIN, HIGH);
    servo1.detach();
  }
  if (loopStep == 850) {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  if (loopStep == duration) {
    idle();
  }
}
void R2Servo::moveTo(int h) {
  //conn();
  targetHeading = h;
  loopStep = 0;
  servo1.write(h);
}
void R2Servo::conn() {
  isIdle = false;
  servo1.attach(SERVO_PIN, 560, 2340); // default values
}
void R2Servo::idle() {
  isIdle = true;
  servo1.detach();
  heading = servo1.read();
}

R2Servo headServo;

long loopCounter = 0;

class R2Sounds {
  public: 
    int duration = 0;
    int loopStep = 0;
    int minSound = 1; // first MP3 file
    int maxSound = 73; // last MP3 file
    bool playing = false;
    void nextStep();
    void speak(int mp3File);
};

void R2Sounds::nextStep() {
  loopStep++;
  if (loopStep > duration) {
    playing = false;
  }
}

void R2Sounds::speak(int mp3File) {
  loopStep = 0;
  playing = true;
  mp3_play (mp3File);
}

R2Sounds r2_Voice; 

void setup() {
  // Prepare random seed
  Serial.begin (9600);
  randomSeed(analogRead(0));  
  
  pinMode(LED_BUILTIN, OUTPUT); // arduino internal LED
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  // *** prepare neoPixels ***
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  // *** prepare Servo ***
  headServo.conn();
  headServo.moveTo(90);
  delay(1000);  // wait for the servo to initialize
  headServo.idle();

  // *** repare Sound ***
  R2Serial.begin (9600);
  mp3_set_serial (R2Serial);  //set softwareSerial for DFPlayer-mini mp3 module 
  delay(5);  //wait 1/5ms for mp3 module to set volume
  mp3_set_volume (VOICE_VOLUME);       // value 0~30
  mp3_set_EQ(0);
  randomSeed(analogRead(0));
  mp3_play (1);
}

void loop() {
  
  // *** set Top Front Logic Display to a random red or blue tone and fade to it in random duration
  if (!r2_Logic1.running) {
    // fade to a shade of blue or white
    if(random(2) == 1) {
      r2_Logic1.blueTarget = random(255);
      r2_Logic1.redTarget = random (r2_Logic1.blueTarget);
      r2_Logic1.greenTarget = r2_Logic1.redTarget;
    } else {
      r2_Logic1.redTarget = random(255);
      r2_Logic1.blueTarget = random (r2_Logic1.redTarget);
      r2_Logic1.greenTarget = r2_Logic1.blueTarget;
    }
    r2_Logic1.duration = random (768);
  }
  r2_Logic1.nextStep();

  // *** set Bottom Fromt Logic Display to a random red or blue tone and fade to it in random duration
  if (!r2_Logic2.running) {
    // fade to a shade of blue or white
    if(random(2) == 1) {
      r2_Logic2.blueTarget = random(255);
      r2_Logic2.redTarget = random (r2_Logic2.blueTarget);
      r2_Logic2.greenTarget = r2_Logic2.redTarget;
    } else {
      r2_Logic2.redTarget = random(255);
      r2_Logic2.blueTarget = random (r2_Logic2.redTarget);
      r2_Logic2.greenTarget = r2_Logic2.blueTarget;
    }
    r2_Logic2.duration = random (1024);
  }
  r2_Logic2.nextStep();

  // *** set Front PSI to a random hue, saruration and brightness and hsv-fade to it in random duration
  if(!r2_FrontPSI.running) {
    long targetHue = 42000 + random(25000); // hue range from cyan to red
    r2_FrontPSI.setColorTargetHSV(targetHue,random(128)+128,random(128)+128);
    r2_FrontPSI.duration = random(1536);
  }
  r2_FrontPSI.nextStep();

  // *** set Rear PSI to a random hue, saruration and brightness and hsv-fade to it in random duration
  if(!r2_RearPSI.running) {
    long targetHue = 9000 + random(11000); // hue range from cyan to red
    r2_RearPSI.setColorTargetHSV(targetHue,random(128)+128,random(128)+128);
    r2_RearPSI.duration = random(2048);
  }
  r2_RearPSI.nextStep();

  // rear Logic Display
  if (!r2_Logic3A.running) 
  {
    long targetHue = 0 + random(23000); // hue range from red to green
    r2_Logic3A.setColorTargetHSV(targetHue,random(128)+128,random(128)+128);
    r2_Logic3A.duration = random(1536);
  }
  r2_Logic3A.nextStep();
  
  if (!r2_Logic3B.running) 
  {
    long targetHue = -1000 + random(23000); // hue range from red to green
    r2_Logic3B.setColorTargetHSV(targetHue,random(128)+128,random(128)+128);
    r2_Logic3B.duration = random(1536);
  }
  r2_Logic3B.nextStep();

  if (!r2_Logic3C.running) 
  {
    long targetHue = -1000 + random(23000); // hue range from red to green
    r2_Logic3C.setColorTargetHSV(targetHue,random(128)+128,random(128)+128);
    r2_Logic3C.duration = random(1536);
  }
  r2_Logic3C.nextStep();
  strip.show();

  // *** Control Servo Head
  if (headServo.isIdle) {
    headServo.duration = 2000 + random(5000); // at least 2 seconds between moves
    headServo.conn();
    headServo.moveTo( getGaussianRand(headServo.minHeading, headServo.maxHeading));
    //headServo.moveTo(random(headServo.minHeading, headServo.maxHeading));
  }
  headServo.nextStep();
  
  // *** SOUND!!! ***
  if (!r2_Voice.playing) {
    r2_Voice.duration = 2000 + random(5000);
    r2_Voice.speak(random(r2_Voice.minSound,r2_Voice.maxSound));
  }
  r2_Voice.nextStep();
  
  
}
