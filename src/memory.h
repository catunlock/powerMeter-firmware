#pragma once

#include <EEPROM.h>

#define EEPROM_SIZE 1024

struct Memory
{
    char ssid[100];
    char password[100];
};

struct Config
{
    float alarmPower;
};

static Config config;
static Memory mem;

void setupMemory()
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, mem);
    EEPROM.get(200, config);
}