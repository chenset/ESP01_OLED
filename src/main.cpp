#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "SH1106Brzo.h"
#include "images.h"
#include <ESP8266HTTPClient.h>
#include <TimeLib.h>

//env configure
#include "env.h"

// Chip name
const String chipName = "ASUKA_THREE";

WiFiClient wifiClient;

// NTP time server
// const char *ntpServerName = "time.nist.gov";
// const char *ntpServerName = "cn.ntp.org.cn";
const char *ntpServerName = "1.asia.pool.ntp.org";
// const char *ntpServerName = "time.pool.aliyun.com";

// UDP for ntp server
WiFiUDP Udp;
const unsigned int localPort = 8888; // local port to listen for UDP packets
const int timeZone = +8; //time zone
// NTP var
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming & outgoing
// packets
// return lastNtpTime if unable to get the time
time_t lastNtpTime = 0;
unsigned long lastNtpTimeFix = 0;


//web benchkmark
const char *webApiUrl = envWebApiUrl;
const char *webApiFingerprint = envWebApiFingerprint;
String webApiTimeStr = "-";


// OLED
SH1106Brzo display(0x3c, 0, 2);

// Baud
const int baud = 115200;

//  Time since
unsigned long timeSinceLastWEB = 0;


// funcs
void OLEDDisplayCtl();

void sendNTPpacket(IPAddress &address);

void webApi();

String webResponseStr = "";
String webResponseArr[16];
void trySyncTime();
String getStrValue(String data, char separator, int index);

time_t getNtpTime();

int getCaiYunWeatherIcon(const String &code);

// weather img
// img array
// "晴",
// "多云",
// "阴",
// "阵雨",
// "雷阵雨",
// "雷阵雨伴有冰雹",
// "雨夹雪",
// "小雨",
// "中雨",
// "大雨",
// "暴雨",
// "大暴雨",
// "特大暴雨",
// "阵雪",
// "小雪",
// "中雪",
// "大雪",
// "暴雪",
// "雾",
// "冻雨",
// "沙尘暴",
// "小雨-中雨",
// "中雨-大雨",
// "大雨-暴雨",
// "暴雨-大暴雨",
// "大暴雨-特大暴雨",
// "小雪-中雪",
// "中雪-大雪",
// "大雪-暴雪",
// "浮尘",
// "扬沙",
// "强沙尘暴晴",
// "霾",
const String weatherImgMapping[] = {
        "1", "3", "5", "7", "7", "7", "7", "7", "7", "8", "8",
        "8", "8", "\"", "\"", "\"", "#", "#", "M", "7", "F", "7",
        "8", "8", "8", "8", "\"", "#", "#", "J", "E", "F", "E",
};


void setup() {
    // Serial
    Serial.begin(baud);

    // init display
    display.init();
    // display.flipScreenVertically();
    //display.setFont((const uint8_t *)Roboto_20);
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, (const uint8_t*)WiFi_Logo_bits);
    display.display();

    // connect wifi
    WiFi.mode(WIFI_STA);
    WiFi.hostname(chipName);
    WiFi.begin((const char*)ssid, (const char*)password);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("WiFi Ready");
    } else {
        Serial.println("WiFi Failed");
    }

    // OTA
    ArduinoOTA.onStart([]() { Serial.println("Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();

    Serial.println("");
    Serial.println("WiFi connected");

    // Print the IP address
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont((const uint8_t *)ArialMT_Plain_16);
    display.drawString(64, 32, WiFi.localIP().toString());
    display.display();
    delay(500);

    // init NTP UDP
    Udp.begin(localPort);
    // Serial.println(Udp.localPort());
    // Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(86400);
}

void loop() {
    bool delayFlag = true;
    // OTA
    ArduinoOTA.handle();

    //OLED
    if (webResponseArr[0] == "0") {
        display.displayOff();
    } else {
        display.displayOn();
        // OLED refresh
        // if (millis() - timeSinceLastClock >= 1000) {
        OLEDDisplayCtl();
        // timeSinceLastClock = millis();
        // }
    }

    unsigned long requestInterval = (unsigned long) webResponseArr[1].toInt();
    if (requestInterval < 1000) {
        requestInterval = 12345; //default
    }
    if (millis() - timeSinceLastWEB >= requestInterval) {
        webApi();
        timeSinceLastWEB = millis();
        delayFlag = false;
    }

    //try to sync time when if not synced
    if (now() < 100000000) {
        trySyncTime();
    }

    if (delayFlag) {
        delay(1000);
    }
}

void trySyncTime() {
    long timestamp = (long)webResponseArr[8].toInt();
    if(timestamp > 100000000){
        //manual change timezone 8*3600  +8 TZ
        setTime(timestamp+8*3600);
    }
}

void OLEDDisplayCtl() {
    display.clear();

    // weather icon
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // display.setFont((const uint8_t *)Meteocons_Plain_21);
    display.setFont(ArialMT_Plain_10);
//    int iconIndex = webResponseArr[7].toInt();
//    if (iconIndex > 30 || iconIndex < 0) {
//        iconIndex = 0;
//    }
    display.drawString(0, 0, weatherImgMapping[getCaiYunWeatherIcon(webResponseArr[7])]);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // display.setFont((const uint8_t *)Roboto_14);
    display.setFont(ArialMT_Plain_10);
    // display.drawString(26, 3, (String)temperatureOnline + "-" + (String)humidityOnline + "-" + (String)pm25Online);
    display.drawString(23, 3, webResponseArr[2]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    // display.setFont((const uint8_t *)Roboto_14);
    display.setFont(ArialMT_Plain_10);
    display.drawString(128, 3, webResponseArr[3]);
//    display.drawString(128, 3, weekDay[weekday() - 1]);
//    display.drawString(128, 3,getStrValue(webResponseStr, '\r\n', 2));

//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(Meteocons_Plain_21);
//  display.drawString(0, 0, weatherImgMapping[weatherImg]);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // display.setFont((const uint8_t *)Roboto_Black_24);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, webResponseArr[4]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    // display.setFont((const uint8_t *)Roboto_14);
    display.setFont(ArialMT_Plain_10);

    if (webResponseArr[5] == "") {
        display.drawString(128, 27, webApiTimeStr);
    } else {
        display.drawString(128, 27, webResponseArr[5]);
    }

    //hour
    time_t hourInt = hour();
    String hourStr;
    if (10 > hourInt) {
        hourStr = "0" + (String) hourInt;
    } else {
        hourStr = (String) hourInt;
    }
    //minute
    time_t minuteInt = minute();
    String minuteStr;
    if (10 > minuteInt) {
        minuteStr = "0" + (String) minuteInt;
    } else {
        minuteStr = (String) minuteInt;
    }
    //second
    time_t secondInt = second();
    String secondStr;
    if (10 > secondInt) {
        secondStr = "0" + (String) secondInt;
    } else {
        secondStr = (String) secondInt;
    }

    //month
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont((const uint8_t *)ArialMT_Plain_16);
    display.drawString(0, 48, webResponseArr[6]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont((const uint8_t *)ArialMT_Plain_16);
    display.drawString(128, 48, hourStr + ":" + minuteStr + ":" + secondStr);

    display.display();
}


void webApi() {
    HTTPClient http;
    // int fix;
    // if ( strcmp ((const char*)webApiFingerprint,"") == 0) {
        http.begin(wifiClient,webApiUrl); // HTTP
        // fix = 0;
    // } else {
        // http.begin(wifiClient, webApiUrl, (const uint8_t*)webApiFingerprint); // HTTPS
        // fix = 400;
    // }
    http.setUserAgent("ESP-01s " + chipName);
    //http.addHeader("Accept-Encoding", "gzip, deflate, sdch");

    // start connection and send HTTP header
    unsigned long start = millis();
    int httpCode = http.GET();

    webApiTimeStr = (String) (millis() - start);
    if (httpCode == 200) {

        webResponseStr = (String) http.getString();
        webResponseArr[0] = getStrValue(webResponseStr, '\r', 0);
        webResponseArr[1] = getStrValue(webResponseStr, '\r', 1);
        webResponseArr[2] = getStrValue(webResponseStr, '\r', 2);
        webResponseArr[3] = getStrValue(webResponseStr, '\r', 3);
        webResponseArr[4] = getStrValue(webResponseStr, '\r', 4);
        webResponseArr[5] = getStrValue(webResponseStr, '\r', 5);
        webResponseArr[6] = getStrValue(webResponseStr, '\r', 6);
        webResponseArr[7] = getStrValue(webResponseStr, '\r', 7);
        webResponseArr[8] = getStrValue(webResponseStr, '\r', 8);

    } else {
        webResponseStr = "";
    }

    http.end();
}

String getStrValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}


time_t getNtpTime() {
    IPAddress ntpServerIP; // NTP server's ip address

    while (Udp.parsePacket() > 0); // discard any previously received packets
    Serial.println("Transmit NTP Request");
    // get a random server from the pool
    WiFi.hostByName(ntpServerName, ntpServerIP);
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 5000) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Serial.println("Receive NTP Response");
            Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long) packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long) packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long) packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long) packetBuffer[43];
            lastNtpTime = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
            lastNtpTimeFix = millis();
            return lastNtpTime;
        }
    }
    Serial.println("No NTP Response :-(");
    return lastNtpTime + ((int) (millis() - lastNtpTimeFix) /
                          1000); // return lastNtpTime if unable to get the time
}

int getCaiYunWeatherIcon(const String &code) {
//https://open.caiyunapp.com/%E5%AE%9E%E5%86%B5%E5%A4%A9%E6%B0%94%E6%8E%A5%E5%8F%A3/v2.2
//天气现象	代码
//晴（白天）	CLEAR_DAY
//晴（夜间）	CLEAR_NIGHT
//多云（白天）	PARTLY_CLOUDY_DAY
//多云（夜间）	PARTLY_CLOUDY_NIGHT
//阴	CLOUDY
//大风	WIND
//雾霾	HAZE
//雨	RAIN
//雪	SNOW
    if (code == "CLEAR_DAY") {
        return 0;
    }
    if (code == "CLEAR_NIGHT") {
        return 0;
    }
    if (code == "PARTLY_CLOUDY_DAY") {
        return 1;
    }
    if (code == "PARTLY_CLOUDY_NIGHT") {
        return 1;
    }
    if (code == "CLOUDY") {
        return 2;
    }
    if (code == "WIND") {
        return 30;
    }
    if (code == "HAZE") {
        return 32;
    }
    if (code == "RAIN") {
        return 8;
    }
    if (code == "SNOW") {
        return 15;
    }

    return 0;
}
