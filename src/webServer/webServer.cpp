/**
 * DomDom Firmware
 * Copyright (c) 2020 DomDomFirmware
 *

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "webServer.h"
#include <FS.h>
#include "../../lib/AsyncTCP/AsyncTCP.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "rtc/rtc.h"
#include "configuration.h"
#include "wifi/WiFi.h"

String GetBodyContent(uint8_t *data, size_t len)
{
  String content = "";
  for (size_t i = 0; i < len; i++) {
    content .concat((char)data[i]);
  }
  return content;
}

DomDomWebServerClass::DomDomWebServerClass(){}

void DomDomWebServerClass::begin()
{
    _server = new AsyncWebServer (WEBSERVER_HTTP_PORT);

    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    _server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    // AJAX para el reloj
    _server->on("/rtc", HTTP_GET, getRTCData);
    _server->on("/rtc", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, setRTCData);
    
    // AJAX para el wifi
    _server->on("/red", HTTP_GET, getWifiData);
    
    _server->onNotFound([](AsyncWebServerRequest *request) {
        Serial.print("[Web server] Not found\n");
        AsyncWebServerResponse *response = request->beginResponse(400, "text/plain", "Not found");
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
    });
    
    _server->begin();

    Serial.println("HTTP server started");
};

void DomDomWebServerClass::getRTCData(AsyncWebServerRequest *request)
{
   AsyncResponseStream *response = request->beginResponseStream("application/json");
    
   StaticJsonDocument<1024> jsonDoc;
   jsonDoc["ready"] = DomDomRTC.ready;
   jsonDoc["enabled"] = DomDomRTC.NTPStarted();
   jsonDoc["servername"] = NTP_SERVERNAME;
   jsonDoc["timezoneOffset"] = NTP_TIMEZONEOFFSET;
   jsonDoc["unixtime"] = DomDomRTC.rtc.now().unixtime();

   serializeJson(jsonDoc, *response);
   
   response->addHeader("Access-Control-Allow-Origin", "*");
   request->send(response);
}

void DomDomWebServerClass::setRTCData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);
    
    DynamicJsonDocument doc(1024);;
    DeserializationError err = deserializeJson(doc, bodyContent);
    if (err) { 
        switch (err.code()) {
            case DeserializationError::Ok:
                Serial.print(F("Deserialization succeeded"));
                break;
            case DeserializationError::InvalidInput:
                Serial.print(F("Invalid input!"));
                break;
            case DeserializationError::NoMemory:
                Serial.print(F("Not enough memory"));
                break;
            default:
                Serial.print(F("Deserialization failed"));
                break;
        };
        request->send(400); 
        return;
    }

    if (!doc.containsKey("enabled"))
    {
        Serial.println("No se recibidio un JSON valido");
        request->send(400); 
        return;
    }
    
    if (!doc["enabled"])
    {
        DomDomRTC.setNTPEnabled(false);
        uint32_t unixtime = doc["unixtime"];
        Serial.printf("Recibido cambio de fecha a: %d\n", unixtime);
        DateTime dt(unixtime);
        DomDomRTC.rtc.adjust(dt);
    }
    else{
        Serial.printf("Habilitado servicio NTP\n");
        DomDomRTC.setNTPEnabled(true);
        DomDomRTC.timeClient->setTimeOffset(doc["timezoneOffset"]);
    }
    
    Serial.printf("Nueva fecha %s\n",DomDomRTC.rtc.now().timestamp().c_str());

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::getWifiData(AsyncWebServerRequest *request)
{
   AsyncResponseStream *response = request->beginResponseStream("application/json");
    
   StaticJsonDocument<1024> jsonDoc;
   jsonDoc["mode"] = DomDomWifi.getMode() == WIFI_MODE_AP ? 'AP' : 'STA';
   jsonDoc["sta_enabled"] = DomDomWifi.enabled;
   if (DomDomWifi.getMode() == WIFI_MODE_STA)
   {
       jsonDoc["ssid"] = DomDomWifi.ssid;
   }
   jsonDoc["mdns_enabled"] = DomDomWifi.mDNS_enabled;
   jsonDoc["mdns_hostname"] = DomDomWifi.mDNS_hostname;

   serializeJson(jsonDoc, *response);
   
   response->addHeader("Access-Control-Allow-Origin", "*");
   request->send(response);
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomWebServerClass DomDomWebServer;
#endif