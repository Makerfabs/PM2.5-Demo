#include <SoftwareSerial.h>
#include <Wire.h> // 加入Wire库
#include <Adafruit_GFX.h> // 加入Adafruit_GFX库
#include <Adafruit_SSD1306.h> // 加入Adafruit_SSD1306库


SoftwareSerial mySerial(2, 3); // RX, TX


#define SCREEN_WIDTH 128 // 
#define SCREEN_HEIGHT 32 // 
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600); // 初始化串口通信
  mySerial.begin(9600); // 初始化软件串口通信
  Wire.begin();
 
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C地址为0x3C
    Serial.println(F("SSD1306初始化失败"));
    for(;;);
    display.display();
  }
}

void loop() {
  if (mySerial.available() >= 32) { // 如果缓冲区中有32个字节可读（16进制数据每个字节占1个字符）
    uint8_t buffer[32];//A variable named "buffer" is defined, which is an unsigned array of 8-bit integers of 32 bytes (256 bits) in length.
    mySerial.readBytes(buffer, 32); // 读取16进制数据并存储到字节数组buffer中

    // 判断起始符是否正确
    if (buffer[0] == 0x42 && buffer[1] == 0x4d) {
      // 计算帧长度
      uint16_t frame_length = ((uint16_t)buffer[2] << 8) | buffer[3];
      if (frame_length == 28) { // 判断帧长度是否正确
        // 解析数据
        uint16_t pm1_0_standard = ((uint16_t)buffer[4] << 8) | buffer[5];  // 使用第5、6个字节解析出标准颗粒物PM1.0的值
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

        // 计算数据和校验位
        uint32_t sum = 0;
        for (int i = 0; i < 30; i++) {
          sum += buffer[i];
        }

        if ((sum & 0xffff) == checksum) { // 判断校验是否正确
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
                display.clearDisplay(); // 清空屏幕

      display.setTextSize(1); // 文字大小
      display.setTextColor(WHITE); // 文字颜色
      display.setCursor(0, 0); // 设置光标位置
      display.print("PM1.0: "); // 显示PM1.0数据
      display.print(pm1_0_standard);
      display.println("ug/m3");

      display.setCursor(0, 10); // 设置光标位置
      display.print("PM2.5: "); // 显示PM2.5数据
      display.print(pm2_5_standard);
      display.println("ug/m3");

      display.setCursor(0, 20); // 设置光标位置
      display.print("PM10: "); // 显示PM10数据
      display.print(pm10_standard);
      display.println("ug/m3");

      display.display(); // 更新屏幕显示
        }
      }
    }
  }
}
