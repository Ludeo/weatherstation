//#include <Arduino.h>
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\cores\esp8266\Arduino.h"
//#include <DHT.h>
#include "../.pio/libdeps/d1_mini/DHT sensor library/DHT.h"
//#include <DHT_U.h>
#include "../.pio/libdeps/d1_mini/DHT sensor library/DHT_U.h"
//#include <Adafruit_GFX.h>
#include "../.pio/libdeps/d1_mini/Adafruit GFX Library/Adafruit_GFX.h"
//#include <Adafruit_SSD1306.h>
#include "../.pio/libdeps/d1_mini/Adafruit SSD1306/Adafruit_SSD1306.h"
//#include <SPI.h>
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\libraries\SPI\SPI.h"
//#include <Wire.h>
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\libraries\Wire\Wire.h"
//#include <ESP8266WiFi.h>
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\libraries\ESP8266WiFi\src\ESP8266WiFi.h"
//#include <WiFiUdp.h>
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\libraries\ESP8266WiFi\src\WiFiUdp.h"
//#include <NTPClient.h>
#include "../.pio/libdeps/d1_mini/NTPClient/NTPClient.h"
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\cores\esp8266\wl_definitions.h"
#include "C:\Users\lucas\.platformio\packages\framework-arduinoespressif8266\cores\esp8266\esp8266_peri.h"
#include "time.h"

#include "main.h"
#include "../include/sun.h"
#include "../include/moon.h"
#include "../include/rain.h"
#include "../include/snow.h"

#define DHT_TYPE DHT11
#define DHTPIN 14
DHT dht(DHTPIN, DHT_TYPE);

#define TEMTPIN A0

#define BUTTONPIN 3
volatile int buttonState = 5;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "iPhone";
const char* password = "JonasJonas";

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define NTP_SERVER "pool.ntp.org"
#define TIMEZONE "CET-1CEST,M3.5.0/02,M10.5.0/03"
time_t now;
tm tm;

#define CIRCLE_OFFSET 42

void IRAM_ATTR button_ISR() {
    buttonState++;
    buttonState = buttonState % 6;
}

void setup() {
    Serial.begin(9600);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    debugPrintln("Connecting to:");
    debugPrintln(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        debugPrint(".");
    }
    debugPrintln("Connected");
    debugPrintln("IP Address: ");
    debugPrintln(WiFi.localIP());
    delay(2000);

    display.clearDisplay();
    display.setCursor(0,0);
    configTime(TIMEZONE, NTP_SERVER);
    debugPrintln("Syncing time");
    while (time(nullptr) < 1600000000)  { // wait for the first time sync
        debugPrint(".");
        delay(100);
    }
    debugPrintln("Time synced");

    pinMode(TEMTPIN, INPUT);

    pinMode(BUTTONPIN, INPUT_PULLUP);
    attachInterrupt(BUTTONPIN, button_ISR, FALLING);

    dht.begin();

    delay(2000);
}

void loop() {
    display.clearDisplay();
    time(&now);
    localtime_r(&now, &tm);

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    float lux = analogRead(TEMTPIN);

    //for debugging purpose
    //lux = 2.5;
    //temperature = 3.2;
    //humidity = 85.4;

    display.drawCircle(CIRCLE_OFFSET + 0, 58, 2, WHITE);
    display.drawCircle(CIRCLE_OFFSET + 8, 58, 2, WHITE);
    display.drawCircle(CIRCLE_OFFSET + 16, 58, 2, WHITE);
    display.drawCircle(CIRCLE_OFFSET + 24, 58, 2, WHITE);
    display.drawCircle(CIRCLE_OFFSET + 32, 58, 2, WHITE);
    display.drawCircle(CIRCLE_OFFSET + 40, 58, 2, WHITE);

    switch(buttonState) {
        case 0:
            fullDisplay(humidity, temperature, lux);
            break;
        case 1:
            temperatureDisplay(temperature);
            break;
        case 2:
            humidityDisplay(humidity);
            break;
        case 3:
            luxDisplay(lux);
            break;
        case 4:
            timeDisplay();
            break;
        case 5:
            pictureDisplay(lux, temperature, humidity);
            break;
    }

    display.display();
    delay(1000);
}

void fullDisplay(float humidity, float temperature, float lux) {
    display.fillCircle(CIRCLE_OFFSET + 0, 58, 2, WHITE);
    display.setTextSize(1);

    display.drawLine(63, 0, 63, 32, SSD1306_WHITE);
    display.drawLine(0, 32, 128, 32, SSD1306_WHITE);
    display.setCursor(5, 5);
    display.print(temperature);
    display.print(" ");
    display.print((char)247);
    display.print("C");
    display.setCursor(5, 15);
    display.print(humidity);
    display.print(" %");

    if (lux < 15) {
        display.drawXBitmap(64, 0, moon_bits, moon_width, moon_height, WHITE);
    } else {
        display.drawXBitmap(64, 0, sun_bits, sun_width, sun_height, WHITE);
    }

    if(temperature < 4) {
        display.drawXBitmap(64, 0, snow_bits, snow_width, snow_height, WHITE);
    }

    if(humidity > 80) {
        display.drawXBitmap(64, 0, rain_bits, rain_width, rain_height, WHITE);
    }

    display.setCursor(10, 42);
    display.print(daysOfTheWeek[tm.tm_wday]);
    display.print(", ");
    if(tm.tm_hour < 10) {
        display.print("0");
    }
    display.print(tm.tm_hour);
    display.print(":");
    if(tm.tm_min < 10) {
        display.print("0");
    }
    display.print(tm.tm_min);
    display.print(":");
    if(tm.tm_sec < 10) {
        display.print("0");
    }
    display.print(tm.tm_sec);
}

void temperatureDisplay(float temperature) {
    display.fillCircle(CIRCLE_OFFSET + 8, 58, 2, WHITE);

    display.setTextSize(2);
    display.setCursor(10, 25);
    display.print(temperature);
    display.print(" ");
    display.print((char)247);
    display.print("C");
}

void humidityDisplay(float humidity) {
    display.fillCircle(CIRCLE_OFFSET + 16, 58, 2, WHITE);

    display.setTextSize(2);
    display.setCursor(10, 25);
    display.print(humidity);
    display.print(" ");
    display.print("%");
}

void luxDisplay(float lux) {
    display.fillCircle(CIRCLE_OFFSET + 24, 58, 2, WHITE);

    display.setTextSize(2);
    display.setCursor(10, 25);
    display.print(lux);
    display.print(" ");
    display.print("lx");
}

void timeDisplay() {
    display.fillCircle(CIRCLE_OFFSET + 32, 58, 2, WHITE);

    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print(daysOfTheWeek[tm.tm_wday]);
    display.setCursor(10, 35);
    if(tm.tm_hour < 10) {
        display.print("0");
    }
    display.print(tm.tm_hour);
    display.print(":");
    if(tm.tm_min < 10) {
        display.print("0");
    }
    display.print(tm.tm_min);
    display.print(":");
    if(tm.tm_sec < 10) {
        display.print("0");
    }
    display.print(tm.tm_sec);
}

void pictureDisplay(float lux, float temperature, float humidity) {
    display.fillCircle(CIRCLE_OFFSET + 40, 58, 2, WHITE);

    if (lux < 15) {
        display.drawXBitmap(32, 10, moon_bits, moon_width, moon_height, WHITE);
    } else {
        display.drawXBitmap(32, 10, sun_bits, sun_width, sun_height, WHITE);
    }

    if(temperature < 4) {
        display.drawXBitmap(32, 10, snow_bits, snow_width, snow_height, WHITE);
    }

    if(humidity > 80) {
        display.drawXBitmap(32, 10, rain_bits, rain_width, rain_height, WHITE);
    }
}

void debugPrint(const char* text) {
    Serial.print(text);
    display.print(text);
    display.display();
}

void debugPrintln(const char* text) {
    Serial.println(text);
    display.println(text);
    display.display();
}

void debugPrintln(IPAddress text) {
    Serial.println(text);
    display.println(text);
    display.display();
}