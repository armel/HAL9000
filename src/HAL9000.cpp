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
#define VERSION "0.0.3"

#include <Arduino_GFX_Library.h>
#include <SD.h>

#define JPEG_LOGO   "/HAL9000.jpg"
#define JPEG_PANEL   "/HAL9000-panel.jpg"
#define MJPEG_BUFFER_SIZE 32768 // memory for a single JPEG frame

Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 14 /* CS */, SCK, MOSI, MISO);
Arduino_GFX *gfx = new Arduino_ILI9342(bus, 33 /* RST */, 0 /* rotation */);

#include "JpegClass.h"
#include "MjpegClass.h"

static JpegClass jpegClass;
static MjpegClass mjpegClass;

/* Variables */
static int total_frames = 0;
static unsigned long total_read_video = 0;
static unsigned long total_decode_video = 0;
static unsigned long total_show_video = 0;
static unsigned long start_ms, curr_ms;

File root;
String videoFilenameMedium[255];
String videoFilenameSmall[255];

uint8_t indiceMedium = 0;
uint8_t indiceSmall = 0;
uint8_t screen = 0;
uint8_t current = 0;
uint8_t last = 0;
uint8_t mode = 0;
uint8_t modeLast = 1;

void medium();
void small();

// Pixel drawing callback
static int mjpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  unsigned long start = millis();
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  total_show_video += millis() - start;
  return 1;
}

// Pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// List files on SD
void getVideoList(File dir)
{
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if(strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".mjpg") != NULL) {
      Serial.println(entry.name());
      if(strstr(entry.name(), "-medium") != NULL) {
        videoFilenameMedium[indiceMedium] = entry.name();
        indiceMedium++;
        /*
        if(indiceMedium==3) {
          break;
        }
        */
      }
      else {
        videoFilenameSmall[indiceSmall] = entry.name();
        indiceSmall++;
      }
    }

    if (entry.isDirectory()) {
      getVideoList(entry);
    }

    entry.close();
  }
}

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
  getVideoList(root);

  // Multitasking task for retreive multi video
  /*
  xTaskCreatePinnedToCore(
      process_1,    // Function to implement the task
      "process_1",  // Name of the task
      1024 * 32,    // Stack size in words
      NULL,         // Task input parameter
      1,            // Priority of the task
      NULL,         // Task handle
      0);           // Core where the task should run

  xTaskCreatePinnedToCore(
      process_2,    // Function to implement the task
      "process_2",  // Name of the task
      1024 * 32,    // Stack size in words
      NULL,         // Task input parameter
      1,            // Priority of the task
      NULL,         // Task handle
      1);           // Core where the task should run
  */
}

boolean button()
{
  M5.update();

  if (M5.BtnB.wasPressed()) {
    return true;
  }
  return false;
}

void medium()
{
  File mjpegFile;

  total_frames = 0;
  total_read_video = 0;
  total_decode_video = 0;
  total_show_video = 0;

  gfx->fillScreen(BLACK);

  if (!SD.begin())
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
  else
  {
    jpegClass.draw(&SD,
        (char *)JPEG_LOGO, jpegDrawCallback, true /* useBigEndian */,
        0 /* x */, 180 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  }

  while(1)
  {
    while(current == last) {
      current = random(indiceMedium);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilenameMedium[current]);

    mjpegFile = SD.open(videoFilenameMedium[current], "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilenameMedium[current]);
      Serial.println(" file for reading");
    }
    else
    {
      if (!mjpegBuf)
      {
        Serial.println(F("mjpegBuf malloc failed!"));
      }
      else
      {
        Serial.println(F("MJPEG start"));

        start_ms = millis();
        curr_ms = millis();
        mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback, true, 0, 0, 320, 180);

        while (mjpegFile.available())
        {
          if(button() == true)
          {
            mjpegFile.close(); 
            free(mjpegBuf);
            return;
          }
          // Read video
          mjpegClass.readMjpegBuf();
          total_read_video += millis() - curr_ms;
          curr_ms = millis();

          // Play video
          mjpegClass.drawJpg();

          total_decode_video += millis() - curr_ms;
          curr_ms = millis();
          total_frames++;
        }
        int time_used = millis() - start_ms;
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        free(mjpegBuf);
      }
    }
    
    last = current;
  }
}

void small()
{
  File mjpegFile;

  uint16_t x = 0;
  uint16_t y = 0;
  
  total_frames = 0;
  total_read_video = 0;
  total_decode_video = 0;
  total_show_video = 0;

  gfx->fillScreen(BLACK);

  if (!SD.begin())
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
  else
  {
    jpegClass.draw(&SD,
        (char *)JPEG_PANEL, jpegDrawCallback, true /* useBigEndian */,
        119 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  }

  M5.Lcd.fillRect(110, 0, 9, 240, BLACK);
  M5.Lcd.fillRect(201, 0, 9, 240, BLACK);

  while(1)
  {
    screen = random(6);

    if(screen > 2) {
      x = 210;
      y = 89 * (screen - 3);
    }
    else {
      x = 0;
      y = 89 * screen;
    }

    while(current == last) {
      current = random(indiceSmall);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilenameSmall[current]);

    mjpegFile = SD.open(videoFilenameSmall[current], "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilenameSmall[current]);
      Serial.println(" file for reading");
    }
    else
    {
      if (!mjpegBuf)
      {
        Serial.println(F("mjpegBuf malloc failed!"));
      }
      else
      {
        Serial.println(F("MJPEG start"));

        start_ms = millis();
        curr_ms = millis();
        mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback, true, x, y, 110, 62);

        while (mjpegFile.available())
        {
          if(button() == true)
          {
            mjpegFile.close(); 
            free(mjpegBuf);
            return;
          }
          // Read video
          mjpegClass.readMjpegBuf();
          total_read_video += millis() - curr_ms;
          curr_ms = millis();

          // Play video
          mjpegClass.drawJpg();
          gfx->fillRect(110, y, 9, 62, BLACK);

          total_decode_video += millis() - curr_ms;
          curr_ms = millis();
          total_frames++;
        }
        int time_used = millis() - start_ms;
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        free(mjpegBuf);
      }
    }
    
    last = current;
  }
}

void loop()
{
  medium();
  small();
}