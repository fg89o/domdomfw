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
#include "channel/channelMgt.h"
#include "statusLedControl/statusLedControl.h"
#include "channel/ScheduleMgt.h"

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
    _server->on("/red", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, setWifiData);

    // AJAX para los canales
    _server->on("/canales", HTTP_GET, getChannelsData);
    _server->on("/canales", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, setChannelsData);

    // AJAX para el reset
    _server->on("/reset", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, setRestart);

    // AJAX para el ajuste de los canales
    _server->on("/adjCanal", HTTP_GET, getChannelsAdj);
    _server->on("/adjCanal", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, setChannelsAdj);

    _server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
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
   jsonDoc["unixtime"] = DomDomRTC.now().unixtime();

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
        DomDomRTC.adjust(dt);
    }
    else{
        Serial.printf("Habilitado servicio NTP\n");
        DomDomRTC.setNTPEnabled(true);
        DomDomRTC.timeZone = doc["timezoneOffset"];
    }
    
    Serial.printf("Nueva fecha %s\n",DomDomRTC.now().timestamp().c_str());

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::getWifiData(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");
        
    StaticJsonDocument<1024> jsonDoc;
    jsonDoc["mode"] = DomDomWifi.getMode() == WIFI_MODE_AP ? "AP" : "STA";
    jsonDoc["sta_enabled"] = DomDomWifi.sta_enabled;
    jsonDoc["rssi"] = DomDomWifi.RSSI();
    jsonDoc["current_channel"] = WiFi.channel();
    if (DomDomWifi.getMode() == WIFI_MODE_STA)
    {
        jsonDoc["ssid"] = DomDomWifi.ssid;
        jsonDoc["current_gateway"] = WiFi.gatewayIP().toString();
        jsonDoc["current_ip"] = WiFi.localIP().toString();
        jsonDoc["current_dns"] = WiFi.dnsIP().toString();
    }
    else
    {
        jsonDoc["ssid"] = WIFI_AP_SSID_NAME;
        jsonDoc["current_gateway"] = WiFi.softAPIP().toString();
        jsonDoc["current_ip"] = WiFi.softAPIP().toString();
        jsonDoc["current_dns"] = "0.0.0.0";
    }
    
    jsonDoc["mdns_enabled"] = DomDomWifi.mDNS_enabled;
    jsonDoc["mdns_hostname"] = DomDomWifi.mDNS_hostname;

    serializeJson(jsonDoc, *response);
    
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::setWifiData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);
    
    DynamicJsonDocument doc(1024);;
    DeserializationError err = deserializeJson(doc, bodyContent);
    if (err) { 
        request->send(400); 
        return;
    }

    if (doc.containsKey("sta_enabled"))
    {
        DomDomWifi.sta_enabled = doc["sta_enabled"];
        if (DomDomWifi.sta_enabled)
        {
            String recv_ssid = doc["ssid"];
            DomDomWifi.saveSTASSID(recv_ssid);
            String recv_pwd = doc["pwd"];
            DomDomWifi.saveSTAPass(recv_pwd);
        }

        DomDomWifi.saveStatus();
    }
    
    if (doc.containsKey("mdns_enabled"))
    {
        DomDomWifi.mDNS_enabled = doc["mdns_enabled"];
    }

    if (doc.containsKey("mdns_hostname"))
    {
        String name = doc["mdns_hostname"];
        DomDomWifi.mDNS_hostname = name;
    }
    
    DomDomWifi.saveMDNSSettings();

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::getChannelsData(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");
        
    StaticJsonDocument<1024> jsonDoc;
    
    JsonArray ports = jsonDoc.createNestedArray("canales");

    for (int i = 0; i < DomDomChannelMgt.channels.size(); i++)
    {
        JsonObject obj = ports.createNestedObject();
        obj["enabled"] = DomDomChannelMgt.channels[i]->getEnabled();
        obj["channel_num"] = DomDomChannelMgt.channels[i]->getNum();
        obj["resolution"] = DomDomChannelMgt.channels[i]->getResolution();
        obj["min_pwm"] = DomDomChannelMgt.channels[i]->min_limit_pwm;
        obj["max_pwm"] = DomDomChannelMgt.channels[i]->max_limit_pwm;
        obj["current_pwm"] = DomDomChannelMgt.channels[i]->current_pwm();
    }

    serializeJson(jsonDoc, *response);
    
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::setChannelsData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);
    
    DynamicJsonDocument doc(2048);;
    DeserializationError err = deserializeJson(doc, bodyContent);

    if (err) { 
        request->send(400); 
        return;
    }

    if (doc.containsKey("canales"))
    {
        JsonArray canales = doc["canales"].as<JsonArray>();
        for(JsonObject canal : canales)
        {
            DomDomChannelClass *channel = DomDomChannelMgt.channels[canal["channel_num"]];
            channel->setEnabled(canal["enabled"]);
            channel->max_limit_pwm = canal["max_pwm"];
            channel->min_limit_pwm = canal["min_pwm"];
            channel->setPWMValue(channel->current_pwm());
            channel->save();
        }

    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::setRestart(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);
    
    DynamicJsonDocument doc(1024);;
    DeserializationError err = deserializeJson(doc, bodyContent);

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");

    if (err) { 
        request->send(400); 
        return;
    }

    if (doc.containsKey("reset"))
    {
        if (doc["reset"])
        {
            request->send(response);
            
            DomDomStatusLedControl.blink(10);

            ESP.restart();
        }
    }

    request->send(400);
}

void DomDomWebServerClass::getChannelsAdj(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");
        
    StaticJsonDocument<1024> jsonDoc;
    
    jsonDoc["modo_programado"] = DomDomScheduleMgt.isStarted();
    JsonArray ports = jsonDoc.createNestedArray("canales");

    for (int i = 0; i < DomDomChannelMgt.channels.size(); i++)
    {
        JsonObject obj = ports.createNestedObject();
        obj["enabled"] = DomDomChannelMgt.channels[i]->getEnabled();
        obj["channel_num"] = DomDomChannelMgt.channels[i]->getNum();
        obj["resolution"] = DomDomChannelMgt.channels[i]->getResolution();
        obj["min_pwm"] = DomDomChannelMgt.channels[i]->min_limit_pwm;
        obj["max_pwm"] = DomDomChannelMgt.channels[i]->max_limit_pwm;
        obj["current_pwm"] = DomDomChannelMgt.channels[i]->current_pwm();
    }

    serializeJson(jsonDoc, *response);
    
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void DomDomWebServerClass::setChannelsAdj(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);
    
    DynamicJsonDocument doc(2048);;
    DeserializationError err = deserializeJson(doc, bodyContent);

    if (err) { 
        request->send(400); 
        return;
    }

    if (doc.containsKey("modo_programado"))
    {
        if (doc["modo_programado"])
        {
            Serial.printf("[Schedule] Programacion iniciada.\n");
            DomDomScheduleMgt.begin();
        }
        else
        {
            Serial.printf("[Schedule] Programacion parada.\n");
            DomDomScheduleMgt.end();
        }
        DomDomScheduleMgt.save();
    }
    
    if (!DomDomScheduleMgt.isStarted() && doc.containsKey("canales"))
    {
        Serial.printf("[MANUAL] Comprobando canales recibidos...\n");
        DomDomStatusLedControl.blink(1);
        JsonArray canales = doc["canales"].as<JsonArray>();
        for(JsonObject canal : canales)
        {
            DomDomChannelClass *channel = DomDomChannelMgt.channels[canal["channel_num"]];
            channel->setPWMValue(canal["current_pwm"]);
            channel->save();
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}


#if !defined(NO_GLOBAL_INSTANCES)
DomDomWebServerClass DomDomWebServer;
#endif