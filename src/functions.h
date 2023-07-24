// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Pixel drawing callback
static int mjpegDrawCallback(JPEGDRAW *pDraw) {
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  unsigned long start = millis();
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  total_show_video += millis() - start;
  return 1;
}

// List files on SD
void getVideoList(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".mjpg.gz") != NULL) {
      M5.Lcd.setTextPadding(200);
      M5.Lcd.setTextColor(TFT_WHITE, TFT_BOOT);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.drawString(entry.name(), 200, 80);

      if (strstr(entry.name(), "-medium") != NULL) {
        videoFilenameMedium[limit] = entry.name();
        limit++;
        delay(50);
      }
    }

    if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL) {
      Serial.println(entry.name());
      getVideoList(entry);
    }

    entry.close();
  }
}

// Check button
void button(void *pvParameters) {
  uint8_t btnA, btnB, btnC;
  
  for (;;) {
    skip = false;

    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();

    if (btnB && load == false) {
      playWav("/HAL9000.wav");
      // skip = true;
    } else if (btnA || btnC) {
      if (btnA) {
        brightnessOld -= 2;
        brightnessOld = (brightnessOld < 2) ? 2 : brightnessOld;
      } else if (btnC) {
        brightnessOld += 2;
        brightnessOld = (brightnessOld >= 252) ? 252 : brightnessOld;
      }

      if (brightnessOld != brightness) {
        brightness = brightnessOld;
        M5.Lcd.setBrightness(brightness);
        preferences.putUInt("brightness", brightness);
        Serial.println(brightness);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// Progress callback
void myProgressCallback(uint8_t progress) {
  static int8_t myLastProgress = -1;
  if (myLastProgress != progress) {
    if (myLastProgress == -1) {
      Serial.print("Progress: ");
    }
    myLastProgress = progress;
    switch (progress) {
      case 0:
        Serial.print("0% ▓");
        break;
      case 25:
        Serial.print(" 25% ");
        break;
      case 50:
        Serial.print(" 50% ");
        break;
      case 75:
        Serial.print(" 75% ");
        break;
      case 100:
        Serial.print("▓ 100%\n");
        myLastProgress = -1;
        break;
      default:
        if (progress < 100) Serial.print("▓");
        break;
    }
    M5.Lcd.fillRect(102, 160, progress * 2, 4, TFT_WHITE);
  }
}

// Set M5GO led
void led() {
  // LED
  RGBColor m5goColor = M5.Lcd.readPixelRGB(100, 0);
  for (uint8_t i = 0; i <= 9; i++) {
    leds[i] = CRGB(m5goColor.r, m5goColor.g, m5goColor.b);
  }
  FastLED.setBrightness(16);
  FastLED.show();
}

// Boot loader
boolean boot() {
  led();

  if (!LittleFS.begin()) {
    Serial.println(F("ERROR: File System Mount Failed!"));
  } else {
    M5.Lcd.drawJpgFile(LittleFS, JPEG_LOGO);

    // Get video files
    root = LittleFS.open("/");

    M5.Lcd.setFont(&Ubuntu_Medium6pt7b);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BOOT);
    M5.Lcd.drawString("HAL9000 Version " + String(VERSION), 200, 20);
    M5.Lcd.drawString(" by F4HWN", 200, 35);
    M5.Lcd.drawString("Loading kernel modules", 200, 65);
    getVideoList(root);

    for (uint8_t i = 0; i < 5; i++) {
      M5.Lcd.drawString(" ", 200, 110);
      delay(250);
      M5.Lcd.drawString("Loading complete", 200, 110);
      delay(250);
    }

    // M5.Lcd.drawFastHLine(90, 120, 220, TFT_WHITE);

    M5.Lcd.drawString("I'm sorry Dave,", 200, 140);
    M5.Lcd.drawString("I'm afraid I can't do that.", 200, 155);

    playWav("/HAL9000.wav");

    M5.Lcd.drawString("HAL 9000 to Dr. Bowman.", 200, 185);
    M5.Lcd.drawString("2001: A space odyssey.", 200, 200);
    delay(1000);
  }
  return true;
}

// Eye loader
boolean eye() {
  M5.Lcd.clear();
  M5.Lcd.setBrightness(0);

  if (!LittleFS.begin()) {
    Serial.println(F("ERROR: File System Mount Failed!"));
  } else {
    M5.Lcd.drawJpgFile(LittleFS, JPEG_EYE);
  }

  for (uint8_t i = 0; i <= brightness; i++) {
    M5.Lcd.setBrightness(i);
    delay(50);
  }

  delay(1000);

  for (uint8_t i = brightness; i >= 1; i--) {
    M5.Lcd.setBrightness(i);
    delay(50);
  }

  return true;
}

// Video medium init
void mediumInit() {
  if (!LittleFS.begin()) {
    Serial.println(F("ERROR: File System Mount Failed!"));
  } else {
    M5.Lcd.clear();
    M5.Lcd.drawJpgFile(LittleFS, JPEG_LOGO);
  }
  M5.Lcd.setBrightness(brightness);
}

// Video medium
void medium() {
  uint8_t counter = 0;
  String cover;

  total_frames       = 0;
  total_read_video   = 0;
  total_decode_video = 0;
  total_show_video   = 0;

  // Gunzip
  tarGzFS.begin();

  while (1) {
    if (RANDOM == 1) {
      while (videoCurrent == videoLast) {
        videoCurrent = random(limit);  // Returns a pseudo-random integer between 0 and number of video files
      }
    } else {
      indice       = (indice++ < (limit - 1)) ? indice : 0;
      videoCurrent = indice;
    }

    Serial.println(videoFilenameMedium[videoCurrent]);

    cover = videoFilenameMedium[videoCurrent];
    cover.replace(".mjpg.gz", ".jpg");

    M5.Lcd.drawJpgFile(LittleFS, "/" + cover, 84, 0);

    led();

    load                   = true;
    GzUnpacker *GZUnpacker = new GzUnpacker();
    GZUnpacker->haltOnError(true);                   // stop on fail (manual restart/reset required)
    GZUnpacker->setupFSCallbacks(targzTotalBytesFn,
                                 targzFreeBytesFn);  // prevent the partition from exploding, recommended
    // GZUnpacker->setGzProgressCallback( BaseUnpacker::defaultProgressCallback ); // targzNullProgressCallback or
    // defaultProgressCallback
    GZUnpacker->setGzProgressCallback(myProgressCallback);  // targzNullProgressCallback or defaultProgressCallback
    GZUnpacker->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);  // gz log verbosity

    if (!GZUnpacker->gzExpander(tarGzFS, ("/" + videoFilenameMedium[videoCurrent]).c_str(), tarGzFS, "/tmp.mjpg")) {
      Serial.printf("gzExpander failed with return code #%d", GZUnpacker->tarGzGetError());
    }

    mjpegFile         = LittleFS.open("/tmp.mjpg", "r");
    uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

    if (!mjpegFile || mjpegFile.isDirectory()) {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilenameMedium[videoCurrent]);
      Serial.println(" file for reading");
    } else {
      if (!mjpegBuf) {
        Serial.println(F("mjpegBuf malloc failed!"));
      } else {
        Serial.println(F("MJPEG start"));

        start_ms = millis();
        curr_ms  = millis();
        mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback, true, 84, 0, 228, 240);
        while (mjpegFile.available()) {
          // Skip
          if (skip) {
            Serial.println(F("MJPEG skip"));
            mjpegFile.close();
            LittleFS.remove("/tmp.mjpg");
            free(mjpegBuf);
            videoLast = videoCurrent;
            counter++;
            return;
          }
          // Read video
          mjpegClass.readMjpegBuf();
          total_read_video += millis() - curr_ms;
          curr_ms = millis();

          // Play video
          mjpegClass.drawJpg();

          if (load == true) {
            led();
            load = false;
          }

          total_decode_video += millis() - curr_ms;
          curr_ms = millis();
          total_frames++;
        }
        Serial.println(F("MJPEG end"));
        mjpegFile.close();
        LittleFS.remove("/tmp.mjpg");
        free(mjpegBuf);
      }
    }

    videoLast = videoCurrent;

    counter++;

    Serial.printf("%d %d \n", counter, limit);
    if (counter >= showEye) {
      eye();
      mediumInit();
      counter = 0;
    }
  }
}