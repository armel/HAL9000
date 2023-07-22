// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "HAL9000.h"
#include "font.h"
#include "wav.h"
#include "functions.h"

// Setup
void setup() {
  // Init M5
  auto cfg = M5.config();

  cfg.clear_display = true;  // default=true. clear the screen when begin.
  // cfg.output_power  = false;  // default=true. use external port 5V output.
  cfg.internal_imu = true;                      // default=true. use internal IMU.
  cfg.internal_rtc = true;                      // default=true. use internal RTC.
  cfg.internal_spk = true;                      // default=true. use internal speaker.
  cfg.internal_mic = true;                      // default=true. use internal microphone.
  cfg.external_imu = false;                     // default=false. use Unit Accel & Gyro.
  cfg.external_rtc = false;                     // default=false. use Unit RTC.

  cfg.external_display.module_display = true;   // default=true. use ModuleDisplay
  cfg.external_display.atom_display   = true;   // default=true. use AtomDisplay
  cfg.external_display.unit_oled      = false;  // default=true. use UnitOLED
  cfg.external_display.unit_lcd       = false;  // default=true. use UnitLCD
  cfg.external_display.unit_rca       = false;  // default=false. use UnitRCA VideoOutput
  cfg.external_display.module_rca     = false;  // default=false. use ModuleRCA VideoOutput

  M5.begin(cfg);

  // Init Led
  if (M5.getBoard() == m5::board_t::board_M5Stack) {
    FastLED.addLeds<NEOPIXEL, 15>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  } else if (M5.getBoard() == m5::board_t::board_M5StackCore2) {
    FastLED.addLeds<NEOPIXEL, 25>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  }

  // Init Rand
  esp_random();

  // Sound
  auto spk_cfg = M5.Speaker.config();

  if (spk_cfg.use_dac || spk_cfg.buzzer)
  {
  /// Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
    spk_cfg.sample_rate = 192000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
  }
  M5.Speaker.config(spk_cfg);
  
  //M5.Speaker.begin();

  // Init Display
  gfx->begin();
  gfx->setRotation(2);
  gfx->fillScreen(TFT_BOOT);
  gfx->invertDisplay(true);

  // Clean LittleFS
  LittleFS.remove("/tmp.mjpg");

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(button,    // Function to implement the task
                          "button",  // Name of the task
                          8192,      // Stack size in words
                          NULL,      // Task input parameter
                          4,         // Priority of the task
                          NULL,      // Task handle
                          1);        // Core where the task should run

  // Boot
  boot();
}

// Main loop
void loop() {
  medium();
}