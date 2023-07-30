// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Version
#define VERSION "1.3.0"
#define AUTHOR  "F4HWN"
#define NAME    "HAL9000"

#define RANDOM 0  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

// Others define
#define HAL9000_FOLDER "/HAL9000"
#define HAL9000_LOGO   "/HAL9000.jpg"
#define HAL9000_EYE    "/HAL9000-eye.jpg"
#define HAL9000_WAV    "/HAL9000.wav"

#define TFT_HAL9000       M5.Displays(0).color565(16, 16, 16)
#define MJPEG_BUFFER_SIZE 228 * 240 * 1  // Memory for a single JPEG frame

#define DEST_FS_USES_LITTLEFS

// Dependencies
#include <Preferences.h>
#include <LittleFS.h>
#include <SD.h>
#include <ESP32-targz.h>
#include <FastLED.h>
#include <M5Unified.h>
#include "MjpegClass.h"

// Preferences
Preferences preferences;

// LED
#define FASTLED_INTERNAL  // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Variables
static MjpegClass mjpegClass;

fs::File root;
fs::File mjpegFile;

String videoFilename[128];

boolean load = false;
boolean skip = false;

int8_t indice          = 0;
uint8_t limit          = 0;
uint8_t videoCurrent   = 0;
uint8_t videoLast      = 0;
uint8_t showEye        = 10;
uint16_t brightness    = BRIGHTNESS;
uint16_t brightnessOld = 0;
