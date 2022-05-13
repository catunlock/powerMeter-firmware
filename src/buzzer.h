#pragma once
#include "EasyBuzzer.h"
#include "meter.h"

const static unsigned int frequency = 1000;
const static unsigned int onDuration = 50;
const static unsigned int offDuration = 100;
const static unsigned int beeps = 2;
const static unsigned int pauseDuration = 500;
const static unsigned int cycles = 4;

static TaskHandle_t buzzTask;

void done()
{
    Serial.print("Beep Done!");
}

void buzzCode(void *parameter)
{
    for (;;)
    {
        Serial.print(power);
        Serial.print(" > ");
        Serial.println(config.alarmPower);
        if (power > config.alarmPower)
        {
            EasyBuzzer.beep(
                frequency,     // Frequency in hertz(HZ).
                onDuration,    // On Duration in milliseconds(ms).
                offDuration,   // Off Duration in milliseconds(ms).
                beeps,         // The number of beeps per cycle.
                pauseDuration, // Pause duration.
                cycles,        // The number of cycle.
                done           // [Optional] Callback. A function to call when the sequence ends.
            );
            vTaskDelay(2000);
        }
    }
}

void setupBuzzer()
{
    EasyBuzzer.setPin(GPIO_NUM_18);

    xTaskCreatePinnedToCore(
        buzzCode,  /* Function to implement the task */
        "Task1",   /* Name of the task */
        10000,     /* Stack size in words */
        NULL,      /* Task input parameter */
        0,         /* Priority of the task */
        &buzzTask, /* Task handle. */
        0);        /* Core where the task should run */
}
