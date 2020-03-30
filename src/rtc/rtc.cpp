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

#include "zones.h"

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

    adjust(rtc.now().unixtime());

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
        result = timeClient->forceUpdate();;
        if (result)
        {
            Serial.printf("[NTP] tiempo recibido: %ld\n", timeClient->getEpochTime());
            adjust(timeClient->getEpochTime());
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

        timeClient = new NTPClient(_ntpUDP, NTP_SERVERNAME, 0, 0);
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
    }
}

void DomDomRTCClass::NTPTask(void * parameter)
{
    while(DomDomRTC.NTPStarted())
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

    vTaskDelete(NULL);
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
    time_t now;
    time(&now);
    
    DateTime dt (now);
    return dt;
}

void DomDomRTCClass::adjust(time_t dt)
{
    // Ajustamos el reloj externo a UTC
    DateTime nDate = DateTime(dt);
    rtc.adjust(nDate);

    Serial.printf("[RTC] RTC externo ajustado a %s\n",nDate.timestamp().c_str());

    // Reloj interno
    tm timeinfo;
    const char* zone = _ntpPosixZone.c_str();
    setenv("TZ", zone, 1);
    tzset();

    localtime_r(&dt, &timeinfo);

    nDate = DateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    );

    timeval epoch;
    epoch.tv_sec = nDate.unixtime();
    epoch.tv_usec = 0;

    settimeofday((const timeval*)&epoch, 0);

    Serial.printf("[RTC] RTC interno ajustado a %s\n",nDate.timestamp().c_str());
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomRTCClass DomDomRTC;
#endif