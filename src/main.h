//
// Created by lucas on 2/4/2024.
//

#ifndef WEATHERSTATION2_MAIN_H
#define WEATHERSTATION2_MAIN_H
    void fullDisplay(float humidity, float temperature, float lux);
    void temperatureDisplay(float temperature);
    void IRAM_ATTR button_ISR();
    void humidityDisplay(float humidity);
    void debugPrint(const char* text);
    void debugPrintln(const char* text);
    void debugPrintln(IPAddress text);
    void luxDisplay(float lux);
    void timeDisplay();
    void pictureDisplay(float lux, float temperature, float humidity);
#endif //WEATHERSTATION2_MAIN_H
