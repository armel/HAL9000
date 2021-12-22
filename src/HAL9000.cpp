// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board
#define BOARD GREY

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

#include <Arduino_GFX_Library.h>

#define JPEG_FILENAME   "/HAL9000.jpg"
#define MJPEG_FILENAME  "/HAL9000.mjpg"
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

void setup()
{
  Serial.begin(115200);
  M5.begin(true, true, false, false);
  M5.Lcd.setBrightness(128);

  // Init Display
  gfx->begin();
  gfx->invertDisplay(true);

  if (!SD.begin())
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
  else
  {
    jpegClass.draw(&SD,
        (char *)JPEG_FILENAME, jpegDrawCallback, true /* useBigEndian */,
        0 /* x */, 180 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  }
}

void loop()
{
  total_frames = 0;
  total_read_video = 0;
  total_decode_video = 0;
  total_show_video = 0;

  File mjpegFile = SD.open(MJPEG_FILENAME, "r");
  uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

  if (!mjpegFile || mjpegFile.isDirectory())
  {
    Serial.println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
    gfx->println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
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
      mjpegClass.setup(
          &mjpegFile, mjpegBuf, mjpegDrawCallback, true /* useBigEndian */,
          0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);

      while (mjpegFile.available())
      {
        M5.update();

        if (M5.BtnB.isPressed()) {
          for (uint8_t z = 0; z <= 128; z++) {
            mjpegClass.readMjpegBuf();
            total_read_video += millis() - curr_ms;
            curr_ms = millis();
            total_decode_video += millis() - curr_ms;
            total_frames++;
          }
          Serial.printf("Jump Frame %d \n", total_frames);
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
      float fps = 1000.0 * total_frames / time_used;
      total_decode_video -= total_show_video;
      Serial.printf("Total frames: %d\n", total_frames);
      Serial.printf("Time used: %d ms\n", time_used);
      Serial.printf("Average FPS: %0.1f\n", fps);
      Serial.printf("Read MJPEG: %lu ms (%0.1f %%)\n", total_read_video, 100.0 * total_read_video / time_used);
      Serial.printf("Decode video: %lu ms (%0.1f %%)\n", total_decode_video, 100.0 * total_decode_video / time_used);
      Serial.printf("Show video: %lu ms (%0.1f %%)\n", total_show_video, 100.0 * total_show_video / time_used);
      
      free(mjpegBuf);
    }
  }
}