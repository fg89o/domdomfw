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
#ifndef DOMDOM_WIFICLASS_h
#define DOMDOM_WIFICLASS_h
#include <Arduino.h>

class DomDomWifiClass
{
    private:
        static bool createOwnAPWifi();
        static int connectSTAWifi();
        static bool initmDNS();
        static void tInit(void * parameter);
        TaskHandle_t *initTaskHandle;
        
    public:
        DomDomWifiClass();
        String ssid;
        String pwd;
        bool enabled = true;
        bool mDNS_enabled;
        String mDNS_hostname;
        bool begin();
        bool isConnected();
        bool isConnecting;
        void printWifiInfo();
        int getMode();
        int8_t RSSI();
        bool saveSTASSID();
        String readSTASSID();
        bool saveSTAPass();
        String readSTAPass();
        bool saveStatus();
        bool saveMDNSSettings();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomWifiClass DomDomWifi;
#endif

#endif /* DOMDOM_WIFICLASS_h */