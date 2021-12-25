// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
      M5.Lcd.setTextPadding(320);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.drawString(entry.name(), 160, 60);

      Serial.println(entry.name());
      if(strstr(entry.name(), "-medium") != NULL) {
        videoFilenameMedium[indiceMedium] = entry.name();
        indiceMedium++;
        /*
        if(indiceMedium==10) {
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

// Check button
boolean button()
{
  M5.update();

  if (M5.BtnB.wasPressed()) {
    return true;
  }
  return false;
}

// video medium
void medium()
{
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
    jpegClass.draw(&SD,(char *)JPEG_LOGO, jpegDrawCallback, true, 0, 180, gfx->width(), gfx->height());
  }

  while(1)
  {
    while(videoCurrent == videoLast) {
      videoCurrent = random(indiceMedium);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilenameMedium[videoCurrent]);

    mjpegFile = SD.open(videoFilenameMedium[videoCurrent], "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilenameMedium[videoCurrent]);
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
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        free(mjpegBuf);
      }
    }
    
    videoLast = videoCurrent;
  }
}

// videoCurrent small
void small()
{
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
    jpegClass.draw(&SD,(char *)JPEG_PANEL, jpegDrawCallback, true, 119, 0, gfx->width(), gfx->height());
  }

  M5.Lcd.fillRect(110, 0, 9, 240, BLACK);
  M5.Lcd.fillRect(201, 0, 9, 240, BLACK);

  for(screenCurrent = 0; screenCurrent < 6; screenCurrent++) {
    videoCurrent = random(indiceSmall);
    if(screenCurrent > 2) {
      x = 210;
      y = 89 * (screenCurrent - 3);
    }
    else {
      x = 0;
      y = 89 * screenCurrent;
    }

    mjpegFile = SD.open(videoFilenameSmall[videoCurrent], "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);
    mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback, true, x, y, 110, 62);
    mjpegClass.readMjpegBuf();
    mjpegClass.drawJpg();
    
    gfx->fillRect(110, y, 9, 62, BLACK);

    mjpegFile.close();        
    free(mjpegBuf);
  }

  while(1)
  {
    while(screenCurrent == screenLast) {
      screenCurrent = random(6);
    }

    if(screenCurrent > 2) {
      x = 210;
      y = 89 * (screenCurrent - 3);
    }
    else {
      x = 0;
      y = 89 * screenCurrent;
    }

    while(videoCurrent == videoLast) {
      videoCurrent = random(indiceSmall);      // Returns a pseudo-random integer between 0 and number of video files
    }

    Serial.println(videoFilenameSmall[videoCurrent]);

    mjpegFile = SD.open(videoFilenameSmall[videoCurrent], "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

    if (!mjpegFile || mjpegFile.isDirectory())
    {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilenameSmall[videoCurrent]);
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
        Serial.println(F("MJPEG end"));
        mjpegFile.close();        
        free(mjpegBuf);
      }
    }
    
    videoLast = videoCurrent;
    screenLast = screenCurrent;
  }
}