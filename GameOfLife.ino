#include <FastLED.h> // For utility functions
#include <Adafruit_NeoPixel_ZeroDMA.h> // For driving LEDs with DMA

#include "GameOfLife.hpp"

#define RANDOMIZE_BUTTON_PIN 3
#define BRIGHTNESS_PIN A0
#define SPEED_PIN A1
#define HUE_PIN A2
#define SLIDE_SWITCH_PIN 5

#define LEDS_PIN 6

#define ROWS 32
#define COLS 32

Adafruit_NeoPixel_ZeroDMA strip(ROWS * COLS, LEDS_PIN, NEO_GRB);

GameOfLife<ROWS, COLS> gameOfLife;

volatile bool wantsRandomize = false;
volatile long lastRandomizeTm = 0;

void randomize_ISR() {
  // Keep track of last change time to debounce the button
  if (digitalRead(RANDOMIZE_BUTTON_PIN) == LOW && millis() - lastRandomizeTm > 250) {
    wantsRandomize = true;
    lastRandomizeTm = millis();
  }
}

int analogBrightness = 0;
int brightness = 10;

void setup() {
  Serial.begin(115200);

  pinMode(BRIGHTNESS_PIN, INPUT);
  pinMode(SPEED_PIN, INPUT);
  pinMode(HUE_PIN, INPUT);

  pinMode(RANDOMIZE_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RANDOMIZE_BUTTON_PIN), randomize_ISR, FALLING);

  strip.begin();
  strip.setBrightness(brightness);
  strip.show();

  gameOfLife.randomize();
}


void loop()
{
  int newBrightness = max(1, analogRead(BRIGHTNESS_PIN) / 64);
  if ((newBrightness * 16) != analogBrightness) {
    analogBrightness = newBrightness * 16;
    brightness = analogBrightness;
    strip.setBrightness(brightness);
  }

  static uint32_t nextUpdateTm = 0;
  static uint32_t nextRedrawTm = 0;

  if (wantsRandomize) {
    gameOfLife.clear();
    gameOfLife.randomize();
    wantsRandomize = false;

    for (int row = 0; row < gameOfLife.rows(); ++row)
    {
      uint32_t rowColor = colorFromHSV((row * 7) % 256, 200, 255);
      for (int col = 0; col < gameOfLife.cols(); ++col)
      {
        strip.setPixelColor(index_of_pixel(row, col), gameOfLife.isAlive(row, col) ? rowColor : 0);
      }
    }
    nextUpdateTm = millis() + 1000;
    nextRedrawTm = millis() + 500;
  }
  else
  {
    if (millis() >= nextUpdateTm)
    {
      gameOfLife.update();
      nextUpdateTm = millis() + 1000 - (analogRead(SPEED_PIN) / 32) * 31;
    }

    if (millis() >= nextRedrawTm)
    {
      uint32_t color = colorFromHSV(analogRead(HUE_PIN) / 4, 200, 255);
      for (int row = 0; row < gameOfLife.rows(); ++row)
      {
        for (int col = 0; col < gameOfLife.cols(); ++col)
        {
          strip.setPixelColor(index_of_pixel(row, col), gameOfLife.isAlive(row, col) ? color : 0);
        }
      }
      nextRedrawTm = millis() + 10;
    }
  }
  
  strip.show();
}

// Required because of the physical layout of my particular LED matrix
int index_of_pixel(int col, int row) {
  if (col < 16 && row < 16) {
    if (row % 2 == 0) {
      return 15 - col + 16 * (15 - row);
    } else {
      return col + 16 * (15 - row);
    }
  } else if (col >= 16 && row < 16) {
    if (row % 2 == 0) {
      return 256 + (15 - (col - 16)) + 16 * (15 - row);
    } else {
      return 256 + (col - 16) + 16 * (15 - row);
    }
  } else if (col < 16 && row >= 16) {
    if (row % 2 == 0) {
      return 512 + 15 - col + 16 * (15 - (row - 16));
    } else {
      return 512 + col + 16 * (15 - (row - 16));
    }
  } else {
    if (row % 2 == 0) {
      return 768 + (15 - (col - 16)) + 16 * (15 - (row - 16));
    } else {
      return 768 + (col - 16) + 16 * (15 - (row - 16));
    }
  }
}

// The Adafruit library does not support HSV colors
uint32_t colorFromHSV(int hue, int sat, int val)
{
  CRGB hueColor = CHSV(hue, sat, val);
  return strip.Color(hueColor.red, hueColor.green, hueColor.blue);
}
