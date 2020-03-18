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

#include "rtc.h"
#include "time.h"
#include "sys/time.h"
#include <EEPROM.h>
#include <Wire.h>
#include "../wifi/WiFi.h"
#include "configuration.h"

DomDomRTCClass::DomDomRTCClass(){}

bool DomDomRTCClass::begin()
{
    Serial.println("Inicializando RTC");

    int attemps = 0;
    ready = false;
    while( !ready && attemps < RTC_BEGIN_ATTEMPS)
    {
        ready = rtc.begin();
        if (!ready)
        {
            Serial.printf("ERROR: Couldn't find RTC (%d of %d)\n",(attemps+1), RTC_BEGIN_ATTEMPS);
            attemps++;
            delay(RTC_BEGIN_ATTEMPS_DELAY);
        }
    }

    if (rtc.lostPower()) {
        Serial.println("Warning: RTC lost power, lets set the time!");
        // following line sets the RTC to the date &amp; time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    int unixtime_sec = rtc.now().unixtime();
    timeval epoch = {unixtime_sec, 0};
    settimeofday((const timeval*)&epoch, 0);

    bool use_ntp = EEPROM.read(EEPROM_NTP_ENABLED_ADDRESS);
    if (use_ntp && DomDomWifi.getMode() == 1)
    {
        beginNTP();
    }

    return ready;
}

bool DomDomRTCClass::updateFromNTP()
{
    bool result = false;
    if (DomDomWifi.getMode() == 1)
    {
        result = timeClient->forceUpdate();
        if (result)
        {
            DateTime dt (timeClient->getEpochTime());
            adjust(dt);
        }
    }
    
    return result;
}

void DomDomRTCClass::beginNTP()
{
    if (!_ntpStarted)
    {
        _ntpStarted = true;
        Serial.println("Inicializando servicio NTP...");

        timeClient = new NTPClient(_ntpUDP, NTP_SERVERNAME, NTP_TIMEZONEOFFSET, 60000);
        timeClient->begin();

        xTaskCreate(
            this->NTPTask,        /* Task function. */
            "NTP_Task",             /* String with name of task. */
            10000,                  /* Stack size in bytes. */
            NULL,                   /* Parameter passed as input of the task */
            1,                      /* Priority of the task. */
            this->_ntp_task         /* Task handle. */
        );
    }
}

void DomDomRTCClass::endNTP()
{
    if (_ntpStarted)
    {
        _ntpStarted = false;
        timeClient->end();
        vTaskDelete(_ntp_task);
    }
}

void DomDomRTCClass::NTPTask(void * parameter)
{
    while(true)
    {
        int ms = 0;
        if (DomDomRTC.updateFromNTP())
        {
            ms = NTP_DELAY_ON_SUCCESS;
        }
        else
        {
            ms = NTP_DELAY_ON_FAILURE;
            Serial.println("ERROR: NTP no recibio una respuesta.");
        };
        
        vTaskDelay(ms / portTICK_PERIOD_MS);
    }
}

void DomDomRTCClass::setNTPEnabled(bool enabled)
{
    if (enabled && !_ntpStarted)
    {
        beginNTP();
    }

    if (!enabled && _ntpStarted)
    {
        endNTP();
    }

    EEPROM.write(EEPROM_NTP_ENABLED_ADDRESS, enabled);
    EEPROM.commit();
}

DateTime DomDomRTCClass::now()
{
    struct timeval epoch;
    gettimeofday(&epoch, NULL);

    DateTime dt (epoch.tv_sec);

    return dt;
}

void DomDomRTCClass::adjust(DateTime dt)
{
    struct timeval epoch;
    epoch.tv_sec = dt.unixtime();
    epoch.tv_usec = 0;

    rtc.adjust(dt);
    settimeofday((const timeval*)&epoch, 0);

    Serial.printf("[RTC] Fecha y hora ajustada a %s\n",dt.timestamp().c_str());
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomRTCClass DomDomRTC;
#endif