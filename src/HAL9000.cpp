// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "HAL9000.h"
#include "functions.h"

// Setup
void setup()
{
  Serial.begin(115200);
  M5.begin(true, true, false, false);
  M5.Lcd.setBrightness(brightness);

  // Init Rand
  esp_random();

  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->invertDisplay(true);

  // Boot
  boot();
}

// Main loop
void loop()
{
  medium();
  small();
}