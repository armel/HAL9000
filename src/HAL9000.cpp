// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "HAL9000.h"
#include "functions.h"

// Setup
void setup()
{
  Serial.begin(115200);
  M5.begin(true, true, false, false);
  M5.Lcd.setBrightness(128);

  // Init Rand
  esp_random();

  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->invertDisplay(true);

  // Get video files
  root = SD.open("/");

  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.drawString("HAL9000", 160, 20);
  M5.Lcd.drawString("Version " + String(VERSION) + " par F4HWN", 160, 30);
  M5.Lcd.drawString("Loading kernel", 160, 50);
  getVideoList(root);
}

// Main loop
void loop()
{
  medium();
  small();
}