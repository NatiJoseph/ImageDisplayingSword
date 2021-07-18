#include "Wire.h"
#include <Adafruit_NeoPixel.h>
#include "SoundData.h";
#include "Photo.h";
#include <MPU6050_light.h>
#include "XT_DAC_Audio.h";
#include <math.h>

#define PIN            16
#define NUMPIXELS      160

MPU6050 mpu(Wire);
unsigned long timer1 = 0;
XT_Wav_Class ForceWithYou(Force);
XT_DAC_Audio_Class DacAudio(25,0);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
bool played_sound = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello!\n");
  while (!Serial) delay(10);
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050

  init_sound();   // Playing sound if Gyro is found

  // Initiating Device
  pixels.setBrightness(50);
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  
  pixels.begin();   // Light up the pixels.
}

void loop() {
  mpu.update();
  float z, rad_z;
  
  z = mpu.getAngleZ();

  // Fixing angle of picture in case is out of bounds
  if(z>90)
    z = 90;
  else if(z < -90)
    z = -90;
  z = z+90;

  // Calculating angle of gyroscope.
  rad_z = z * 3.14 / 180;
  leds_by_angle(rad_z);
  smart_delay(1);
}

// This function receives the degree in radians that the gyroscope is set
// and calculates the pixels of the picture than need to be displayed.
void leds_by_angle(float rad) {
  int x,y,pixel_num;
  for(int r=1; r<=NUMPIXELS; r++) {
    x = int(r*cos(rad)) + NUMPIXELS*3;
    y = int(r*sin(rad));
    pixel_num = 3*NUMPIXELS*2*y + 3*x;
    led_set(Photo[pixel_num], Photo[pixel_num+1], Photo[pixel_num+2], r-1);
  }
  led_show();
}

void led_set(int colorA, int colorB, int colorC, int index) {
  pixels.setPixelColor(index, pixels.Color(colorA, colorB, colorC));
}

void led_show() {
  pixels.show();
}

// This function is responsible for the audio ("May the force be with you")
void init_sound() {
  static uint32_t sound_i=0;
  while(sound_i<4092) {               
    if(ForceWithYou.Completed) {          
      DacAudio.PlayWav(&ForceWithYou);
    }
    sound_i++;
  }  
}

void smart_delay(int period) {
  unsigned long _time = millis();
  while(millis() < _time + period) {
    
  }
}
