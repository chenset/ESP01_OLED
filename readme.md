当某块esp8266芯片无法使用Platform上传的情况: 
在本机安装官方Arduino IDE跑一遍ArduinoOTA demo, 再继续用本机的PlatformIo

#### env.h  

```   
const char* envWebBenchmarkUrl = "";
const char* envWebBenchmarkFingerprint = "";
const char* ssid = "";
const char* password = "";
```

#### clion

```
pio init --ide clion
```



#### GPIO

```
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
Initialize the OLED display using brzo_i2c
D1 -> SDA
D2 -> SCL
SSD1306Brzo display(0x3c, D1, D2);
```
