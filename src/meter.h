#pragma once

#include <Adafruit_ADS1X15.h>
static Adafruit_ADS1115 ads;

static float currentRMS = 0.0f;
static float power = 0.0f;

#include "web_socket.h"
#include "buzzer.h"

// Pin connected to the ALERT/RDY signal for new sample notification.
constexpr int READY_PIN = GPIO_NUM_19; // G21

// This is required on ESP32 to put the ISR in IRAM. Define as
// empty for other platforms. Be careful - other platforms may have
// other requirements.
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

volatile bool new_data = false;
void IRAM_ATTR NewDataReadyISR()
{
    new_data = true;
}

void setupMeter()
{
    Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
    Serial.println("ADC Range: +/- 1.024V  1 bit = 0.5mV    0.03125mV");

    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    //                                                                ADS1015  ADS1115
    //                                                                -------  -------
    // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    ads.setGain(GAIN_FOUR); // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }

    ads.setDataRate(RATE_ADS1115_860SPS);

    pinMode(READY_PIN, INPUT);
    // We get a falling edge every time a new sample is ready.
    attachInterrupt(digitalPinToInterrupt(READY_PIN), NewDataReadyISR, FALLING);

    // Start continuous conversions.
    ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1, /*continuous=*/true);
}

float getCurrentRMS()
{
    static const float FACTOR = 30; // 30A/1V
    static const float multiplier = 0.03125F;

    float voltage;
    float corriente;
    float sum = 0;
    long tiempo = millis();
    int counter = 0;

    while (millis() - tiempo < 1000)
    {
        voltage = ads.readADC_Differential_0_1() * multiplier;
        corriente = voltage * FACTOR;
        corriente /= 1000.0;

        sum += sq(corriente);
        counter = counter + 1;
    }

    corriente = sqrt(sum / counter);
    return (corriente);
}

void printMeasure(String prefix, float value, String postfix)
{
    Serial.print(prefix);
    Serial.print(value, 3);
    Serial.println(postfix);
}

void loopMeter()
{
    currentRMS = getCurrentRMS();
    power = 240.0 * currentRMS;

    ws.textAll(String(currentRMS, 4));
    printMeasure("Irms: ", currentRMS, "A ,");
    printMeasure("Potencia: ", power, "W");
    printMeasure("Alarm threshold: ", config.alarmPower, "W");
    Serial.println("");

    EasyBuzzer.update();
}