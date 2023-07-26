// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Version
#define VERSION "1.1.0"
#define AUTHOR  "F4HWN"
#define NAME    "HAL9000"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

// Others define
#define JPEG_LOGO         "/HAL9000.jpg"
#define JPEG_EYE          "/HAL9000-eye.jpg"
#define MJPEG_BUFFER_SIZE 32768 * 2  // Memory for a single JPEG frame
#define TFT_BOOT          M5.Lcd.color565(32, 32, 32)
#define DEST_FS_USES_LITTLEFS

// Dependencies
#include <Preferences.h>
#include <LittleFS.h>
#include <ESP32-targz.h>
#include <FastLED.h>
#include <M5Unified.h>
#include <Arduino_GFX_Library.h>
#include "MjpegClass.h"

// Preferences
Preferences preferences;

// LED
#define FASTLED_INTERNAL  // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Variables
static MjpegClass mjpegClass;

static int total_frames                 = 0;
static unsigned long total_read_video   = 0;
static unsigned long total_decode_video = 0;
static unsigned long total_show_video   = 0;
static unsigned long start_ms, curr_ms;

fs::File root;
fs::File mjpegFile;

String videoFilenameMedium[128];
String videoFilenameSmall[128];

boolean load = false;
boolean skip = false;

int8_t indice         = 0;
uint8_t limit         = 0;
uint8_t videoCurrent  = 0;
uint8_t videoLast     = 0;
uint8_t brightness    = 32;
uint8_t brightnessOld = 0;
uint8_t showEye       = 10;

#if BOARD == CORE
Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 14 /* CS */, SCK, MOSI, MISO);
Arduino_GFX *gfx     = new Arduino_ILI9342(bus, 33 /* RST */, 2 /* rotation */);
#elif BOARD == CORE2
Arduino_DataBus *bus = new Arduino_ESP32SPI(15 /* DC */, 5 /* CS */, SCK, MOSI, MISO);
Arduino_GFX *gfx     = new Arduino_ILI9342(bus, 33 /* RST */, 2 /* rotation */);
#elif BOARD == CORES3
// Todo
#endif
