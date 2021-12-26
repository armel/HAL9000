// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board
#define BOARD BASIC

#define BASIC 1
#define GREY  2
#define CORE2 3

#if BOARD == BASIC
  #define M5STACK_MPU6886
  #include <M5Stack.h>
#elif BOARD == GREY
  #define M5STACK_MPU6886
  #include <M5Stack.h>
#elif BOARD == CORE2
  #include <M5Core2.h>
#endif

// Version
#define VERSION "0.0.5"

// Others define
#define JPEG_LOGO   "/HAL9000.jpg"
#define JPEG_PANEL   "/HAL9000-panel.jpg"
#define MJPEG_BUFFER_SIZE 32768 * 2 // memory for a single JPEG frame

// Others include
#include <Arduino_GFX_Library.h>
#include <SD.h>
#include "JpegClass.h"
#include "MjpegClass.h"

/* Variables */

static JpegClass jpegClass;
static MjpegClass mjpegClass;

static int total_frames = 0;
static unsigned long total_read_video = 0;
static unsigned long total_decode_video = 0;
static unsigned long total_show_video = 0;
static unsigned long start_ms, curr_ms;

File root;
File mjpegFile;

String videoFilenameMedium[128];
String videoFilenameSmall[128];

uint8_t indiceMedium = 0;
uint8_t indiceSmall = 0;
uint8_t screenCurrent = 0;
uint8_t screenLast = 0;
uint8_t videoCurrent = 0;
uint8_t videoLast = 0;

Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 14 /* CS */, SCK, MOSI, MISO);
Arduino_GFX *gfx = new Arduino_ILI9342(bus, 33 /* RST */, 0 /* rotation */);