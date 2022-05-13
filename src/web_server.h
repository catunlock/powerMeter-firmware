#pragma once

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "meter.h"

#define PARAM_MESSAGE message

static AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "404 Not found");
}

void setupWebServer()
{

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    // Send a GET request to <IP>/get?message=<message>
    server.on("/measurement", HTTP_GET, [](AsyncWebServerRequest *request)
              { String measurement(currentRMS);
                    request->send(200, "text/plain", measurement.c_str()); });

    server.on("/measurement_json", HTTP_GET, [](AsyncWebServerRequest *request)
              { String measurement(currentRMS);
        String response = "[ {\" target \" : \" my_metric \", \" datapoints \" : [ [ 2, 1525320000000 ] ]} ]";
        request->send(200, "text/plain", measurement.c_str()); });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  if (request->hasParam("ssid", true) && request->hasParam("psk", true))
                  {
                      String ssid = request->getParam("ssid", true)->value();
                      String psk = request->getParam("psk", true)->value();

                      Serial.printf("Updating wifi config to: %s - %s", ssid, psk.c_str());
                      strcpy(mem.ssid, ssid.c_str());
                      strcpy(mem.password, psk.c_str());

                      EEPROM.put(0, mem);
                      EEPROM.commit();
                      request->send(200, "text/plain", "Wifi config updated.");
                  }
                  else
                  {
                      request->send(400, "text/plain", "ssid or psk fields missing.");
                  } });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  if (request->hasParam("alarmPower", true))
                  {
                      config.alarmPower = request->getParam("alarmPower", true)->value().toFloat();

                      Serial.printf("Updating alarm power to: %f", config.alarmPower);
                      EEPROM.put(200, config);
                      EEPROM.commit();

                      request->send(200, "text/plain", "Alarm power threshold updated.");
                  }
                  else
                  {
                      request->send(400, "text/plain", "alarmPower field missing.");
                  } });

    server.onNotFound(notFound);

    server.begin();
}