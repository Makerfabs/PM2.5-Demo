#include <Arduino_GFX_Library.h>  // Include the Arduino GFX Library
#include "HardwareSerial.h"  // Include the HardwareSerial Library
#include <Wire.h>  // Include the Wire Library for I2C communication
#include "SPI.h"  // Include the SPI Library
#include "FS.h"  // Include the FS Library

#define I2C_SDA_PIN 17  // Define the I2C SDA pin number
#define I2C_SCL_PIN 18  // Define the I2C SCL pin number

#define GFX_BL -1  // Define the backlight pin number for GFX display
#define TFT_BL GFX_BL

// Create an instance of the Arduino_ESP32RGBPanel class for controlling the RGB panel
Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  1 /* CS */, 12 /* SCK */, 11 /* SDA */,
  45 /* DE */, 4/* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
  39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
  0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
  6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */
);

// Create an instance of the Arduino_ST7701_RGBPanel class for controlling the TFT display
Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
  bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
  true /* IPS */, 480 /* width */, 480 /* height */,
  st7701_type1_init_operations, sizeof(st7701_type1_init_operations),
  true /* BGR */
);

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud rate
  Serial2.begin(9600, SERIAL_8N1, 20, 19);  // Initialize Serial2 with custom settings and specified RX/TX pins

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);  // Initialize I2C communication with specified SDA and SCL pins
  gfx->begin();  // Initialize the TFT display
  gfx->fillScreen(WHITE);  // Fill the screen with white color
}

void loop() {
  if (Serial2.available() >= 32) {  // Check if there are at least 32 bytes available to read from Serial2 buffer
    uint8_t buffer[32];  // Define a buffer to store the received data, which is an array of 32 bytes

    Serial2.readBytes(buffer, 32);  // Read 32 bytes from Serial2 into the buffer
    Serial.write(Serial2.read());  // Write a single byte from Serial2 to Serial for debugging purposes
    yield();

    // Check if the start sequence is correct
    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {
      // Calculate the frame length
      uint16_t frame_length = ((uint16_t)buffer[2] << 8) | buffer[3];

      if (frame_length == 28) {  // Check if the frame length is correct
        // Parse the data
        uint16_t pm1_0_standard = ((uint16_t)buffer[4] << 8) | buffer[5];  // Extract the PM1.0 value in standard units
        uint16_t pm2_5_standard = ((uint16_t)buffer[6] << 8) | buffer[7];  // Extract the PM2.5 value in standard units
        uint16_t pm10_standard = ((uint16_t)buffer[8] << 8) | buffer[9];  // Extract the PM10 value in standard units
        uint16_t pm1_0_atmosphere = ((uint16_t)buffer[10] << 8) | buffer[11];  // Extract the PM1.0 value in atmospheric conditions
        uint16_t pm2_5_atmosphere = ((uint16_t)buffer[12] << 8) | buffer[13];  // Extract the PM2.5 value in atmospheric conditions
        uint16_t pm10_atmosphere = ((uint16_t)buffer[14] << 8) | buffer[15];  // Extract the PM10 value in atmospheric conditions
        uint16_t particles_0_3 = ((uint16_t)buffer[16] << 8) | buffer[17];  // Extract the number of particles greater than 0.3μm/0.1L
        uint16_t particles_0_5 = ((uint16_t)buffer[18] << 8) | buffer[19];  // Extract the number of particles greater than 0.5μm/0.1L
        uint16_t particles_1_0 = ((uint16_t)buffer[20] << 8) | buffer[21];  // Extract the number of particles greater than 1.0μm/0.1L
        uint16_t particles_2_5 = ((uint16_t)buffer[22] << 8) | buffer[23];  // Extract the number of particles greater than 2.5μm/0.1L
        uint16_t particles_5_0 = ((uint16_t)buffer[24] << 8) | buffer[25];  // Extract the number of particles greater than 5.0μm/0.1L
        uint16_t particles_10 = ((uint16_t)buffer[26] << 8) | buffer[27];  // Extract the number of particles greater than 10μm/0.1L
        uint16_t checksum = ((uint16_t)buffer[30] << 8) | buffer[31];  // Extract the checksum value

        // Calculate the sum of all data bytes for checksum verification
        uint32_t sum = 0;
        for (int i = 0; i < 30; i++) {
          sum += buffer[i];
        }

        if ((sum & 0xffff) == checksum) {  // Check if the calculated checksum matches the received checksum
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

          // Display the PM values on the TFT display
          gfx->fillScreen(WHITE); 
          gfx->setTextSize(4);
          gfx->setTextColor(BLACK);
          gfx->setCursor(100, 50);
          gfx->println(F("Makerfabs"));       
          gfx->setTextSize(3);
          gfx->setCursor(30, 100);
          gfx->print("PM1.0: ");
          gfx->print(pm1_0_standard);
          gfx->println(F("ug/m3"));
          gfx->setTextSize(3);
          gfx->setCursor(30, 200);
          gfx->print("PM2.5: ");
          gfx->print(pm2_5_standard);
          gfx->println(F("ug/m3"));
          gfx->setTextSize(3);
          gfx->setCursor(30, 300);
          gfx->print("PM10: ");
          gfx->print(pm10_standard);
          gfx->println(F("ug/m3"));

          delay(800);
        }
      }
    }
  }
}

        
