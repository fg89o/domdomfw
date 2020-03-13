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

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include "configuration.h"
#include "rtc/rtc.h"
#include "wifi/WiFi.h"
#include "channel/ScheduleMgt.h"
#include "channel/channelMgt.h"
#include "webServer/webServer.h"

int _lastMinute = -1;

void initEEPROM()
{
  
  Serial.println("Inicializando EEPROM");
  bool init = false;
  int err_count = 0;
  while(err_count<EEPROM_INIT_RETRIES && !init)
  {
      init = EEPROM.begin(EEPROM_SIZE);
      if (!init)
      {
          err_count++;
          Serial.print(".");
          delay(200);
      }
  }

  // Primer uso
  if (EEPROM.read(1) != 1)
  {
    Serial.print("Poniendo EEPROM a 0\n");
    EEPROM.write(1,1);

    // for (int j=2; j < EEPROM_SIZE; j++)
    // {
    //   EEPROM.write(j,0);
    // }

    EEPROM.writeString(EEPROM_STA_SSID_NAME_ADDRESS, "");
    EEPROM.writeString(EEPROM_STA_PASSWORD_ADDRESS, "");

    EEPROM.write(EEPROM_MDNS_ENABLED_ADDRESS, MDNS_ENABLED);
    EEPROM.writeString(EEPROM_MDNS_HOSTNAME_ADDRESS, MDNS_HOSTNAME);

    EEPROM.commit();

    DomDomChannelMgt.initEEPROM();
  }

}

void setup()
{
  Serial.begin(BAUDRATE);

  Serial.println("===============================================");
  Serial.println(" DomDom Firmware \n Copyright (c) 2020 DomDomFirmware ");
  Serial.println("===============================================");

  initEEPROM();

  // Inicializa el wifi
  DomDomWifi.begin();

  // Inicializacion del RTC
  DomDomRTC.begin();  

  // Puntos de programacion
  DomDomScheduleMgt.load();

  // Configuracion de programacion
  DomDomSchedulePoint *point = new DomDomSchedulePoint(ALL, 13, 44);
  point->value[0] = 100;
  point->value[1] = 100;
  point->value[2] = 100;
  DomDomScheduleMgt.schedulePoints.push_back(point);

  DomDomSchedulePoint *point2 = new DomDomSchedulePoint(ALL, 13, 46);
  point2->value[0] = 100;
  point2->value[1] = 0;
  point2->value[2] = 0;
  DomDomScheduleMgt.schedulePoints.push_back(point2);

  DomDomSchedulePoint *point3 = new DomDomSchedulePoint(ALL, 13, 47);
  point3->value[0] = 100;
  point3->value[1] = 0;
  point3->value[2] = 0;
  DomDomScheduleMgt.schedulePoints.push_back(point3);

  DomDomSchedulePoint *point4 = new DomDomSchedulePoint(ALL, 13, 49);
  point4->value[0] = 100;
  point4->value[1] = 100;
  point4->value[2] = 100;
  DomDomScheduleMgt.schedulePoints.push_back(point4);

  // DomDomScheduleMgt.save();

  // Configuracion de canales
  DomDomChannelMgt.loadAll();
  DomDomChannelMgt.begin();

  // Servidor web
  DomDomWebServer.begin();

}

void loop()
{
  if (_lastMinute != DomDomRTC.rtc.now().minute())
  {
    _lastMinute = DomDomRTC.rtc.now().minute();
    DomDomChannelMgt.update();
  }
  
  delay(1000);
}