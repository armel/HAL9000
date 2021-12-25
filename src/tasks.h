// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Process_1
void process_1(void *pvParameters)
{
  static File mjpegFile;

  uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

  uint16_t x = 0;
  uint16_t y = 0;
  uint8_t screen = 0;
  uint8_t current = 0;
  uint8_t last = 0;

  static MjpegClass mjpegClass;
  static int total_frames = 0;
  static unsigned long total_read_video = 0;
  static unsigned long total_decode_video = 0;
  static unsigned long start_ms, curr_ms;

  for (;;)
  {
    total_frames = 0;
    total_read_video = 0;
    total_decode_video = 0;
    total_show_video_1 = 0;

    screen = random(3);
    y = 89 * screen;

    while(current == last) {
      current = random(indice);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilename[current]);
    mjpegFile = SD.open(videoFilename[current], "r");

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilename[current]);
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
        mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback_1, true, x, y, gfx->width(), gfx->height());

        while (mjpegFile.available())
        {
          // Read video
          mjpegClass.readMjpegBuf();
          total_read_video += millis() - curr_ms;
          curr_ms = millis();

          // Play video
          mjpegClass.drawJpg();
          total_decode_video += millis() - curr_ms;

          curr_ms = millis();
          total_frames++;
          vTaskDelay(pdMS_TO_TICKS(10));
        }
        int time_used = millis() - start_ms;
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        //free(mjpegBuf);
      }
    }
    
    last = current;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void process_2(void *pvParameters)
{
  File mjpegFile;

  uint8_t *mjpegBuf_2 = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

  uint16_t x = 210;
  uint16_t y = 0;
  uint8_t screen = 0;
  uint8_t current = 0;
  uint8_t last = 0;

  static MjpegClass mjpegClass_2;
  static int total_frames_2 = 0;
  static unsigned long total_read_video_2 = 0;
  static unsigned long total_decode_video_2 = 0;
  static unsigned long start_ms, curr_ms;

  for (;;)
  {
    total_frames_2 = 0;
    total_read_video_2 = 0;
    total_decode_video_2 = 0;
    total_show_video_2 = 0;

    screen = random(3);
    y = 89 * screen;

    while(current == last) {
      current = random(indice);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilename[current]);
    mjpegFile = SD.open(videoFilename[current], "r");

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilename[current]);
      Serial.println(" file for reading");
    }
    else
    {
      if (!mjpegBuf_2)
      {
        Serial.println(F("mjpegBuf malloc failed!"));
      }
      else
      {
        Serial.println(F("MJPEG start"));
        
        start_ms = millis();
        curr_ms = millis();
        mjpegClass_2.setup(&mjpegFile, mjpegBuf_2, mjpegDrawCallback_2, true, x, y, gfx->width(), gfx->height());

        while (mjpegFile.available())
        {
          // Read video
          mjpegClass_2.readMjpegBuf();
          total_read_video_2 += millis() - curr_ms;
          curr_ms = millis();

          // Play video
          mjpegClass_2.drawJpg();
          total_decode_video_2 += millis() - curr_ms;

          curr_ms = millis();
          total_frames_2++;
          vTaskDelay(pdMS_TO_TICKS(10));
        }
        int time_used = millis() - start_ms;
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        //free(mjpegBuf);
      }
    }

    last = current;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
