#pragma once

#include <WiFi.h>
#include <WiFiAP.h>

#include "memory.h"

#define WL_MAC_ADDR_LENGTH 6
#define WiFI_Fallback_PSK "qwerty123456"
#define WiFI_Fallback_SSID "PowerMeter "

void setupWiFiAP()
{
    WiFi.mode(WIFI_AP);

    // Do a little work to get a unique-ish name. Append the
    // last two bytes of the MAC (HEX'd) to "Thing-":
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    String AP_NameString = WiFI_Fallback_SSID + macID;

    char AP_NameChar[AP_NameString.length() + 1];
    memset(AP_NameChar, 0, AP_NameString.length() + 1);

    for (int i = 0; i < AP_NameString.length(); i++)
        AP_NameChar[i] = AP_NameString.charAt(i);

    WiFi.softAP(AP_NameChar, WiFI_Fallback_PSK);

    Serial.printf("Setting up access point with SSID: %s, PSK: %s", AP_NameChar, WiFI_Fallback_PSK);
}

void setupWifi()
{
    Serial.printf("Connecting to: %s with password %s", mem.ssid, mem.password);
    WiFi.begin(mem.ssid, mem.password);

    int wifi_intents = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_intents < 40)
    {
        Serial.print(".");
        delay(500);
        wifi_intents++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        setupWiFiAP();
    }
    else
    {
        Serial.print("Connected, IP: ");
        Serial.println(WiFi.localIP());
    }
}