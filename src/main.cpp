#include <Arduino.h>
#include "wifi_config.h"
#include "memory.h"
#include "web_server.h"
#include "web_socket.h"
#include "meter.h"
#include "buzzer.h"
#include <SPIFFS.h>
#include <ESPmDNS.h>

void setup()
{
  Serial.begin(115200);
  setupMemory();
  setupWifi();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  setupBuzzer();
  setupWebServer();
  setupMeter();

  MDNS.addService("http", "tcp", 80);
}

void loop()
{
  loopMeter();
  ws.cleanupClients();
}