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
#include <EEPROM.h>
#include <Wire.h>
#include "../wifi/WiFi.h"
#include "configuration.h"

DomDomRTCClass::DomDomRTCClass(){}

// Inicia le proceso de conexcion WIFI.
// Si hay un SSID al que conectarse lo intenta. 
// En caso contrario o de error crea su propio AP para tener conexion.
// Al final inicia el servicio mDNS.
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

    Serial.printf("%s\n", rtc.now().timestamp().c_str());

    bool use_ntp;
    char saved_enabled = EEPROM.read(EEPROM_NTP_ENABLED_ADDRESS);
    if (saved_enabled != 255)
    {
        use_ntp = saved_enabled == 1;
    }else{
        use_ntp = NTP_ENABLED;
    }

    if (use_ntp && DomDomWifi.getMode() == 1)
    {
        beginNTP();
    }

    return ready;
}

void DomDomRTCClass::update()
{
    timeClient->update();
}

void DomDomRTCClass::beginNTP(){

    if (!_ntpStarted)
    {
        _ntpStarted = true;
        Serial.println("Inicializando servicio NTP...");

        timeClient = new NTPClient(_ntpUDP, NTP_SERVERNAME, NTP_TIMEZONEOFFSET, 60000);
        timeClient->begin();
        
        if (DomDomRTC.timeClient->forceUpdate())
        {
            DateTime dt (DomDomRTC.timeClient->getEpochTime());
            DomDomRTC.rtc.adjust(dt);
        }

        xTaskCreate(
            this->NTPUpdate,        /* Task function. */
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

void DomDomRTCClass::NTPUpdate(void * parameter)
{
    int count = 0;
    while(DomDomRTC.NTPStarted())
    {
        if (count >= 1200 )
        {
            count = 0;
            if (DomDomRTC.timeClient->forceUpdate())
            {
                DateTime dt (DomDomRTC.timeClient->getEpochTime());
                DomDomRTC.rtc.adjust(dt);

                Serial.printf("[NTP] Fecha y hora ajustada a %s por NTP\n",DomDomRTC.rtc.now().timestamp().c_str());
            }
            else
            {
                Serial.println("ERROR: NTP no recibio una respuesta.");
            };
        } 
        
        vTaskDelay(500 / portTICK_PERIOD_MS);
        count++;
    }

    DomDomRTC.endNTP();
    
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

#if !defined(NO_GLOBAL_INSTANCES)
DomDomRTCClass DomDomRTC;
#endif