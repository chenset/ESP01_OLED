#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "SH1106Brzo.h"
#include "images.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <TimeLib.h>

//env configure
#include "env.h"

// Chip name
const String chipName = "ASUKA_THREE";

// chip name
const char *host = chipName.c_str();

// NTP time server
// const char *ntpServerName = "time.nist.gov";
// const char *ntpServerName = "cn.ntp.org.cn";
const char *ntpServerName = "1.asia.pool.ntp.org";

// web update server
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

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
const char *webBenchmarkUrl = envWebBenchmarkUrl;
const char *webBenchmarkFingerprint = envWebBenchmarkFingerprint;
String webBenchmarkTimeStr = "-";


// OLED
SH1106Brzo display(0x3c, 0, 2);

// Baud
const int baud = 115200;

//  Time since
unsigned long timeSinceLastWEB = 0;


// funcs
void OLEDDisplayCtl();

void sendNTPpacket(IPAddress &address);
void webBenchmark();

String webResponseStr = "";
String webResponseArr[10];

String getStrValue(String data, char separator, int index);

time_t getNtpTime();


void setup() {
    // Serial
    Serial.begin(baud);

    // init display
    display.init();
    // display.flipScreenVertically();
    display.setFont(Roboto_20);
    display.clear();
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    display.display();

    // connect wifi
    WiFi.mode(WIFI_AP_STA);
    WiFi.hostname(chipName);
    WiFi.begin(ssid, password);
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
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 32, WiFi.localIP().toString());
    display.display();
    delay(500);

    // init NTP UDP
    Udp.begin(localPort);
    // Serial.println(Udp.localPort());
    // Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(86400);

    // web update server
    MDNS.begin(host);
    httpUpdater.setup(&httpServer);
    httpServer.begin();

    MDNS.addService("http", "tcp", 80);
}

void loop() {
    bool delayFlag = true;
    // OTA
    ArduinoOTA.handle();
    // web update server
    httpServer.handleClient();
    MDNS.update();

    //OLED
    if(webResponseArr[0] == "0"){
        display.displayOff();
    }else{
        display.displayOn();
        // OLED refresh
        // if (millis() - timeSinceLastClock >= 1000) {
        OLEDDisplayCtl();
        // timeSinceLastClock = millis();
        // }
    }

    unsigned long requestInterval = (unsigned long)webResponseArr[1].toInt();
    if(requestInterval < 1000){
        requestInterval = 12345; //default
    }
    if (millis() - timeSinceLastWEB >= requestInterval) {
        webBenchmark();
        timeSinceLastWEB = millis();
        delayFlag = false;
    }

    if (delayFlag) {
        delay(1000);
    }
}

void OLEDDisplayCtl() {

    display.clear();


    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Roboto_14);
    // display.drawString(26, 3, (String)temperatureOnline + "-" + (String)humidityOnline + "-" + (String)pm25Online);
    display.drawString(0, 3, webResponseArr[2]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Roboto_14);
    display.drawString(128, 3, webResponseArr[3]);
//    display.drawString(128, 3, weekDay[weekday() - 1]);
//    display.drawString(128, 3,getStrValue(webResponseStr, '\r\n', 2));

//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(Meteocons_Plain_21);
//  display.drawString(0, 0, weatherImgMapping[weatherImg]);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Roboto_Black_24);
    display.drawString(0, 20, webResponseArr[4]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Roboto_14);

    if(webResponseArr[5] == ""){
        display.drawString(128, 27, webBenchmarkTimeStr);
    }else{
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
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 48, webResponseArr[6]);

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(128, 48, hourStr + ":" + minuteStr + ":" + secondStr);

    display.display();
}


void webBenchmark() {
    HTTPClient http;
    int fix;
    if (webBenchmarkFingerprint == "") {
        http.begin(webBenchmarkUrl); // HTTP
        fix = 0;
    } else {
        http.begin(webBenchmarkUrl, webBenchmarkFingerprint); // HTTPS
        fix = 400;
    }
    http.setUserAgent("ESP-01s " + chipName);
    //http.addHeader("Accept-Encoding", "gzip, deflate, sdch");

    // start connection and send HTTP header
    unsigned long start = millis();
    int httpCode = http.GET();

    webBenchmarkTimeStr = (String)(millis() - start - fix);
    if (httpCode > 0) {

        webResponseStr = (String) http.getString();
        webResponseArr[0] = getStrValue(webResponseStr, '\r', 0);
        webResponseArr[1] = getStrValue(webResponseStr, '\r', 1);
        webResponseArr[2] = getStrValue(webResponseStr, '\r', 2);
        webResponseArr[3] = getStrValue(webResponseStr, '\r', 3);
        webResponseArr[4] = getStrValue(webResponseStr, '\r', 4);
        webResponseArr[5] = getStrValue(webResponseStr, '\r', 5);
        webResponseArr[6] = getStrValue(webResponseStr, '\r', 6);

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
