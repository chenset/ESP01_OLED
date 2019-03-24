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


