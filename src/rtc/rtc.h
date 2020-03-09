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

#ifndef DOMDOMRTCCLASS_h
#define DOMDOMRTCCLASS_h

#include <Arduino.h>
#include "../lib/RTCLib/RTClib.h"
#include "../lib/NTPClient/NTPClient.h"
#include <WiFiUdp.h>

#define RTC_BEGIN_ATTEMPS 3
#define RTC_BEGIN_ATTEMPS_DELAY 200

class DomDomRTCClass
{
    private:
        WiFiUDP _ntpUDP;
        bool _ntpStarted = false;
        TaskHandle_t *_ntp_task;
        static void NTPUpdate(void * parameter);

    public:
        DomDomRTCClass();
        NTPClient *timeClient;
        RTC_DS3231 rtc;
        bool ready;
        bool begin();
        void beginNTP();
        void endNTP();
        void update();
        void setNTPEnabled(bool enabled);
        bool NTPStarted() const { return _ntpStarted; };
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomRTCClass DomDomRTC;
#endif

#endif /* DOMDOMRTCCLASS_h */