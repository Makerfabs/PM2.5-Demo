#include <SoftwareSerial.h>  //Defines the header files required for the software serial port
#include <Wire.h> // Add the Wire library
#include <Adafruit_GFX.h> // Add the Adafruit_GFX library
#include <Adafruit_SSD1306.h> // Add the Adafruit_GFX library


SoftwareSerial mySerial(2, 3); // Define the software serial, RX, TX  


#define SCREEN_WIDTH 128 // Screen width in pixels
#define SCREEN_HEIGHT 32 // Screen height in pixels
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600); // Initialize serial communication
  mySerial.begin(9600); // Initialize serial communication
  Wire.begin();
 
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Initialize serial communication
    Serial.println(F("SSD1306 initialization failed"));
    for(;;);
    display.display();
  }
}

void loop() {
  if (mySerial.available() >= 32) { // If there are 32 bytes available to read in the buffer
    uint8_t buffer[32];//A variable named "buffer" is defined, which is an unsigned array of 8-bit integers of 32 bytes (256 bits) in length.
    mySerial.readBytes(buffer, 32); // If there are 32 bytes available to read in the buffer

    // Check if the start code is correct
    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {
      // Calculate the frame length
      uint16_t frame_length = ((uint16_t)buffer[2] << 8) | buffer[3];
      if (frame_length == 28) { // Check if the frame length is correct
        // Parse the data
        uint16_t pm1_0_standard = ((uint16_t)buffer[4] << 8) | buffer[5];  // Extract the value of standard particle matter PM1.0 using the 5th and 6th bytes. By parity of reasoning
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

        // Calculate the data checksum
        uint32_t sum = 0;
        for (int i = 0; i < 30; i++) {
          sum += buffer[i];
        }

        if ((sum & 0xffff) == checksum) { //by parity of reasoning
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

          
          display.clearDisplay(); // clear the display

          display.setTextSize(1); // Set text size
          display.setTextColor(WHITE); // Set text color
          display.setCursor(0, 0); // Set cursor position
          display.print("PM1.0: "); // Display PM1.0 data
          display.print(pm1_0_standard);
          display.println("ug/m3");

          display.setCursor(0, 10); // Set cursor position
          display.print("PM2.5: "); // Display PM2.5 data
          display.print(pm2_5_standard);
          display.println("ug/m3");

          display.setCursor(0, 20); // Set cursor position
          display.print("PM10: "); // Display PM10 data
          display.print(pm10_standard);
          display.println("ug/m3");

          display.display(); // Update screen display
        }
      }
    }
  }
}
