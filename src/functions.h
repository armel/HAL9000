// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Pixel drawing callback
static int mjpegDrawCallback(JPEGDRAW *pDraw) {
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  for (uint8_t d = 0; d < displayCount; d++) {
    M5.Displays(d).pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  }
  return 1;
}

// Copy file from LittleFS to SD
void copy(String filename) {
  LittleFS.begin();
  if (SD.begin(GPIO_NUM_4, SPI, 10000000)) {
    if (!SD.exists(HAL9000_FOLDER)) {
      SD.mkdir(HAL9000_FOLDER);
    }

    if (!SD.exists(filename)) {
      File sourceFile = LittleFS.open("/tmp.mjpg");
      File destFile   = SD.open(String(HAL9000_FOLDER) + "/tmp.mjpg", FILE_WRITE);
      static uint8_t buf[512];

      Serial.printf("Start copy of %s on SD\n", filename.c_str());
      while (sourceFile.read(buf, 512)) {
        destFile.write(buf, 512);
      }

      destFile.close();
      sourceFile.close();

      SD.rename(String(HAL9000_FOLDER) + "/tmp.mjpg", filename);
      Serial.println("Stop copy");
    } else {
      Serial.printf("File %s exist\n", filename.c_str());
    }
  }
}

// List files on LittleFS
void getVideoList(File dir) {
  char module[4];

  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".mjpg.gz") != NULL) {
      sprintf(module, "%02d", limit + 1);
      videoFilename[limit] = entry.name();
      limit++;

      for (uint8_t d = 0; d < displayCount; d++) {
        M5.Displays(d).setTextPadding(200);
        M5.Displays(d).setTextColor(TFT_WHITE, TFT_HAL9000);
        M5.Displays(d).setTextDatum(CC_DATUM);
        M5.Displays(d).drawString("Loading kernel modules " + String(module), 200, 65);
        M5.Displays(d).drawString(entry.name(), 200, 80);
        M5.Displays(d).fillRect(102, 94, map(limit, 1, 60, 1, 200), 4, TFT_WHITE);
      }

      delay(50);
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
  uint8_t step = 2;
  uint8_t min  = 4;
  uint8_t max  = 255;

  uint8_t btnA = 0;
  uint8_t btnB = 0;
  uint8_t btnC = 0;

  struct Button {
    String name;     // name
    int x;           // x
    int y;           // y
    int w;           // width
    int h;           // height
    int d;           // distance
    boolean active;  // active, if true, check this button, else bypass
    boolean read;    // read, if true, button is push, else false
  };

#if BOARD == CORES3
  Button myBtn[] = {
    {"myBtnA", 0, 160, 100, 80, 1000, true, false},
    {"myBtnB", 110, 160, 100, 80, 1000, true, false},
    {"myBtnC", 220, 160, 100, 80, 1000, true, false},
  };
#else
  Button myBtn[] = {
    {"myBtnA", 0, 240, 100, 80, 1000, true, false},
    {"myBtnB", 110, 240, 100, 80, 1000, true, false},
    {"myBtnC", 220, 240, 100, 80, 1000, true, false},
  };
#endif

  for (;;) {
    skip = false;

    M5.update();

    if (M5.getBoard() == m5::board_t::board_M5Stack || M5.getBoard() == m5::board_t::board_M5StackCore2) {
      step = 4;
      min  = 4;
      max  = 255;

      btnA = M5.BtnA.isPressed();
      btnB = M5.BtnB.isPressed();
      btnC = M5.BtnC.isPressed();
    } else if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
      step = 16;
      min  = 64;
      max  = 255;

      auto t = M5.Touch.getDetail();
      auto c = M5.Touch.getCount();

      uint8_t limit = sizeof myBtn / sizeof myBtn[0];

      int distanceBtn     = 0;
      int distanceMin     = 1000;
      int distanceCurrent = 1000;

      if (c == 1) {
        for (uint8_t i = 0; i < limit; i++) {
          myBtn[i].read = false;
          if (myBtn[i].active == true) {
            distanceCurrent = (int)(sqrt(pow(t.x - (myBtn[i].x + (myBtn[i].w / 2)), 2) +
                                         pow(t.y - (myBtn[i].y + (myBtn[i].h / 2)), 2)));
            myBtn[i].d      = distanceCurrent;
            if (distanceCurrent < distanceMin) {
              distanceMin = distanceCurrent;
              distanceBtn = i;
            }
          }
        }

        if (t.state == 1 || t.state == 3 || t.state == 5) {
          myBtn[distanceBtn].read = true;
        }
      }

      btnA = myBtn[0].read;
      btnB = myBtn[1].read;
      btnC = myBtn[2].read;
    }

    if (btnB && load == false) {
      playWav(HAL9000_WAV);
      // skip = true;
    } else if (btnA || btnC) {
      if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      if (btnA) {
        brightnessOld -= step;
        brightnessOld = (brightnessOld <= min) ? min : brightnessOld;
      } else if (btnC) {
        brightnessOld += step;
        brightnessOld = (brightnessOld >= max) ? max : brightnessOld;
      }

      if (brightnessOld != brightness) {
        brightness = brightnessOld;
        M5.Displays(0).setBrightness(brightness);
        preferences.putUInt("brightness", brightness);
        Serial.println(brightness);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// Gunzip progress callback
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
    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).fillRect(102, 160, progress * 2, 4, TFT_WHITE);
    }
  }
}

// Set led on M5GO module
void led() {
  RGBColor m5goColor = M5.Displays(0).readPixelRGB(100, 0);
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
    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawJpgFile(LittleFS, HAL9000_LOGO);
    }

    // Get video files
    root = LittleFS.open("/");

    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).setFont(&Ubuntu_Medium6pt7b);
      M5.Displays(d).setTextDatum(CC_DATUM);
      M5.Displays(d).setTextColor(TFT_WHITE, TFT_HAL9000);
      M5.Displays(d).drawString(String(NAME) + " Version " + String(VERSION), 200, 20);
      M5.Displays(d).drawString(" by " + String(AUTHOR), 200, 35);
    }

    getVideoList(root);

    for (uint8_t d = 0; d < displayCount; d++) {
      for (uint8_t i = 0; i < 5; i++) {
        M5.Displays(d).drawString(" ", 200, 110);
        delay(250);
        M5.Displays(d).drawString("Loading complete", 200, 110);
        delay(250);
      }

      M5.Displays(d).drawString("I'm sorry Dave,", 200, 140);
      M5.Displays(d).drawString("I'm afraid I can't do that.", 200, 155);
    }

    playWav(HAL9000_WAV);

    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawString("HAL 9000 to Dr. Bowman.", 200, 185);
      M5.Displays(d).drawString("2001: A space odyssey.", 200, 200);
    }
    delay(1000);
  }
  return true;
}

// Eye loader
boolean eye() {
  if (!LittleFS.begin()) {
    Serial.println(F("ERROR: File System Mount Failed!"));
  } else {
    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawJpgFile(LittleFS, HAL9000_EYE);
    }
  }

  /*
  for (uint8_t i = 0; i <= 128; i++) {
    M5.Displays(0).setBrightness(i);
    delay(50);
  }

  delay(1000);

  for (uint8_t i = 128; i >= 1; i--) {
    M5.Displays(0).setBrightness(i);
    delay(50);
  }
  */

  delay(2000);

  for (uint8_t i = 0; i <= 120; i++) {
    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawFastHLine(0, i - 1, 320, TFT_BLACK);
      M5.Displays(d).drawFastHLine(0, i, 320, TFT_WHITE);
      M5.Displays(d).drawFastHLine(0, 240 - i, 320, TFT_WHITE);
      M5.Displays(d).drawFastHLine(0, 240 - i + 1, 320, TFT_BLACK);
      delay(5);
    }
  }

  for (uint8_t d = 0; d < displayCount; d++) {
    M5.Displays(d).drawFastHLine(0, 120, 320, TFT_BLACK);
  }

  delay(200);

  return true;
}

// Video init
void videoInit() {
  if (!LittleFS.begin()) {
    Serial.println(F("ERROR: File System Mount Failed!"));
  } else {
    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawJpgFile(LittleFS, HAL9000_LOGO);
    }
  }
  M5.Displays(0).setBrightness(brightness);
}

// Video
void video() {
  uint8_t counter = 0;
  String screenshot, filename;

  // SD
  SD.begin(GPIO_NUM_4, SPI, 10000000);

  // Gunzip
  tarGzFS.begin();

  while (1) {
    if (RANDOM == 1) {
      while (videoCurrent == videoLast) {
        videoCurrent = random(limit);  // Returns a pseudo-random integer between 0 and number of video files
      }
    } else {
      videoCurrent = indice;
      indice       = (indice++ < (limit - 1)) ? indice : 0;
    }

    Serial.println(videoFilename[videoCurrent]);

    screenshot = videoFilename[videoCurrent];
    screenshot.replace(".mjpg.gz", ".jpg");

    filename = videoFilename[videoCurrent];
    filename.replace(".mjpg.gz", ".mjpg");
    filename = String(HAL9000_FOLDER) + "/" + filename;

    for (uint8_t d = 0; d < displayCount; d++) {
      M5.Displays(d).drawJpgFile(LittleFS, "/" + screenshot, 84, 0);
    }

    // Set leds on M5GO module
    led();

    load = true;

    SD.begin(GPIO_NUM_4, SPI, 10000000);

    Serial.printf("Check on SD %s\n", filename.c_str());

    if (SD.exists(filename)) {
      Serial.printf("%s IS on SD\n", filename.c_str());
      mjpegFile = SD.open(filename, FILE_READ);
      delay(2000);
    } else {
      Serial.printf("%s IS NOT on SD\n", filename.c_str());
      LittleFS.remove("/tmp.mjpg");

      GzUnpacker *GZUnpacker = new GzUnpacker();
      GZUnpacker->haltOnError(true);  // stop on fail (manual restart/reset required)
      GZUnpacker->setupFSCallbacks(targzTotalBytesFn,
                                   targzFreeBytesFn);         // prevent the partition from exploding, recommended
      GZUnpacker->setGzProgressCallback(myProgressCallback);  // targzNullProgressCallback or defaultProgressCallback
      GZUnpacker->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);  // gz log verbosity

      if (!GZUnpacker->gzExpander(tarGzFS, ("/" + videoFilename[videoCurrent]).c_str(), tarGzFS, "/tmp.mjpg")) {
        Serial.printf("gzExpander failed with return code #%d", GZUnpacker->tarGzGetError());
      }

      // Copy file on SD if possible
      copy(filename);

      mjpegFile = LittleFS.open("/tmp.mjpg", FILE_READ);
    }

    if (!mjpegFile || mjpegFile.isDirectory()) {
      Serial.print("ERROR: Failed to open ");
      Serial.print(videoFilename[videoCurrent]);
      Serial.println(" file for reading");
    } else {
      uint8_t *mjpegBuf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);

      if (!mjpegBuf) {
        Serial.println(F("mjpegBuf malloc failed!"));
        free(mjpegBuf);
      } else {
        Serial.println(F("MJPEG start"));
        mjpegClass.setup(&mjpegFile, mjpegBuf, mjpegDrawCallback, true, 84, 0, 228, 240);
        while (mjpegFile.available()) {
          // Read video
          mjpegClass.readMjpegBuf();

          // Play video
          mjpegClass.drawJpg();

          // Set leds on M5GO module
          if (load == true) {
            led();
            load = false;
          }

          // Small delay if no dual screen with Display module
          if (displayCount == 1) delay(10);
        }
        Serial.println(F("MJPEG end"));
        mjpegFile.close();
        free(mjpegBuf);
      }
      Serial.printf("%d kb %d kb %d kb %d kb\n", ESP.getHeapSize() / 1024, ESP.getFreeHeap() / 1024,
                    ESP.getPsramSize() / 1024, ESP.getFreePsram() / 1024);
    }
    SD.end();

    videoLast = videoCurrent;

    counter++;

    Serial.printf("%d %d \n", counter, limit);
    if (counter >= showEye) {
      eye();
      videoInit();
      counter = 0;
    }
  }
}