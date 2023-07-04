#include <Arduino_GFX_Library.h>
#include "HardwareSerial.h"
#include <Wire.h>
#include "SPI.h"
#include "FS.h"
#include <SD.h>
#include "JpegFunc.h"


// microSD card
#define SD_SCK 12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS 10


#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

#define JPEG_FILENAME_LOGO "/logo.jpg"

#define GFX_BL -1
#define TFT_BL GFX_BL

#define IMAGE_COUNT 1


String image_list[IMAGE_COUNT] = {

  "/image01.jpg",

};

int image_index = 0;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  1 /* CS */, 12 /* SCK */, 11 /* SDA */,
  45 /* DE */, 4/* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
  39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
  0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
  6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */
);

Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
  bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
  true /* IPS */, 480 /* width */, 480 /* height */,
  st7701_type1_init_operations, sizeof(st7701_type1_init_operations),
  true /* BGR */);


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 20, 19);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  gfx->begin();
  gfx->fillScreen(WHITE);


}

static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}



void loop() {

  if (Serial2.available() >= 32) {
    uint8_t buffer[32];//A variable named "buffer" is defined, which is an unsigned array of 8-bit integers of 32 bytes (256 bits) in length.
    Serial2.readBytes(buffer, 32); // If there are 32 bytes available to read in the buffer
    Serial.write(Serial2.read());
    yield();

    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {

      uint16_t frame_length = ((uint16_t)buffer[2] << 8) | buffer[3];
      if (frame_length == 28) {
        uint16_t pm1_0_standard = ((uint16_t)buffer[4] << 8) | buffer[5];
        uint16_t pm2_5_standard = ((uint16_t)buffer[6] << 8) | buffer[7];
        uint16_t pm10_standard = ((uint16_t)buffer[8] << 8) | buffer[9];
        uint16_t pm1_0_atmosphere = ((uint16_t)buffer[10] << 8) | buffer[11];
        uint16_t pm2_5_atmosphere = ((uint16_t)buffer[12] << 8) | buffer[13];
        uint16_t pm10_atmosphere = ((uint16_t)buffer[14] << 8) | buffer[15];
        uint16_t particles_0_3 = ((uint16_t)buffer[16] << 8) | buffer[17];
        uint16_t particles_0_5 = ((uint16_t)buffer[18] << 8) | buffer[19];
        uint16_t particles_1_0 = ((uint16_t)buffer[20] << 8) | buffer[21];
        uint16_t particles_2_5 = ((uint16_t)buffer[22] << 8) | buffer[23];
        uint16_t particles_5_0 = ((uint16_t)buffer[24] << 8) | buffer[25];
        uint16_t particles_10 = ((uint16_t)buffer[26] << 8) | buffer[27];
        uint16_t checksum = ((uint16_t)buffer[30] << 8) | buffer[31];

       
        uint32_t sum = 0;
        for (int i = 0; i < 30; i++) {
          sum += buffer[i];
        }


#if 1
        SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
        if (!SD.begin(SD_CS))
        {
          Serial.println(F("ERROR: SD Mount Failed!"));
          //while(1)
          {
            gfx->fillScreen(WHITE);
            gfx->setTextSize(3);
            gfx->setTextColor(RED);
            gfx->setCursor(50, 180);
            gfx->println(F("ERROR: SD Mount Failed!"));
            delay(3000);
          }
        }
        else
        {
          unsigned long start = millis();
          jpegDraw(JPEG_FILENAME_LOGO, jpegDrawCallback, true /* useBigEndian */,
                   0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
          Serial.printf("Time used: %lums\n", millis() - start);

          gfx->setTextColor(BLACK);
          gfx->setTextSize(3);
          gfx->setCursor(60, 175);
          gfx->print("PM1.0: ");
          gfx->print(pm1_0_standard);
          gfx->println(F("ug/m3"));
          gfx->setTextSize(3);
          gfx->setCursor(60, 290);
          gfx->print("PM2.5: ");
          gfx->print(pm2_5_standard);
          gfx->println(F("ug/m3"));
          gfx->setTextSize(3);
          gfx->setCursor(60, 405);
          gfx->print("PM10 : ");
          gfx->print(pm10_standard);
          gfx->println(F("ug/m3"));


          delay(800);


          //if ((sum & 0xffff) == checksum) {
          Serial.print("PM1.0(μg/m³): ");
          Serial.println(pm1_0_standard);
          Serial.print("PM2.5(μg/m³): ");
          Serial.println(pm2_5_standard);
          Serial.print("PM10(μg/m³): ");
          Serial.println(pm10_standard);
          Serial.print("PM1.0 in atmosphere(μg/m³): ");
          Serial.println(pm1_0_atmosphere);
          Serial.print("PM2.5 in atmosphere(μg/m³): ");
          Serial.println(pm2_5_atmosphere);
          Serial.print("PM10 in atmosphere(μg/m³): ");
          Serial.println(pm10_atmosphere);
          Serial.print("Particles >0.3μm/0.1L: ");
          Serial.println(particles_0_3);
          Serial.print("Particles >0.5μm/0.1L: ");
          Serial.println(particles_0_5);
          Serial.print("Particles >1.0μm/0.1L: ");
          Serial.println(particles_1_0);
          Serial.print("Particles >2.5μm/0.1L: ");
          Serial.println(particles_2_5);
          Serial.print("Particles >5.0μm/0.1L: ");
          Serial.println(particles_5_0);
          Serial.print("Particles >10μm/0.1L: ");
          Serial.println(particles_10);


#endif


        }
      }
    }
  }
}
