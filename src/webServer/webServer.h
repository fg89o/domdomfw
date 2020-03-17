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
#pragma once
#ifndef DOMDOM_WEBSERVERCLASS_h
#define DOMDOM_WEBSERVERCLASS_h

#include <Arduino.h>

#include <ESPAsyncWebServer.h>

#define WEBSERVER_HTTP_PORT 80

class DomDomWebServerClass
{
    private:
        AsyncWebServer *_server;

    public:
        DomDomWebServerClass();
        void begin();
        static void getRTCData(AsyncWebServerRequest *request);
        static void setRTCData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
        static void getWifiData(AsyncWebServerRequest *request);
        static void setWifiData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
        static void getChannelsData(AsyncWebServerRequest *request);
        static void setChannelsData(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
        static void setRestart(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomWebServerClass DomDomWebServer;
#endif

#endif /* DOMDOM_WEBSERVERCLASS_h */