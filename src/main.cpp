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
#include "statusLedControl/statusLedControl.h"
#include "fan/fanControl.h"

void initEEPROM()
{
  
  Serial.printf("Iniciando EEPROM con %d bytes\n", EEPROM_SIZE);
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
    Serial.print("Detectado primer uso, inicializando EEPROM\n");
    EEPROM.write(1,1);

    EEPROM.writeString(EEPROM_STA_SSID_NAME_ADDRESS, "");
    EEPROM.writeString(EEPROM_STA_PASSWORD_ADDRESS, "");

    EEPROM.write(EEPROM_MDNS_ENABLED_ADDRESS, MDNS_ENABLED);
    EEPROM.writeString(EEPROM_MDNS_HOSTNAME_ADDRESS, MDNS_HOSTNAME);

    EEPROM.writeBool(EEPROM_NTP_ENABLED_ADDRESS, NTP_ENABLED);

    EEPROM.writeBool(EEPROM_SCHEDULE_STATUS_ADDRESS, true);

    int address = EEPROM_FAN_ENABLED_ADDRESS;
    EEPROM.writeBool(address, true);
    address++;
    EEPROM.writeShort(address, pow(2, FAN_PWM_RESOLUTION));
    address += 2;
    EEPROM.writeShort(address, 0);
    address += 2;
    EEPROM.writeShort(address, 40);
    address += 2;
    EEPROM.writeShort(address, 30);
    address += 2;
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

  // Inicializamos la eeprom
  initEEPROM();

  // Inicializamos el control del led de estado
  DomDomStatusLedControl.begin();

  // Inicializa el wifi
  DomDomWifi.begin();

  // Inicializacion del RTC
  DomDomRTC.begin();  

  // Configuracion de canales
  DomDomChannelMgt.loadAll();
  DomDomChannelMgt.begin();

  // Puntos de programacion
  DomDomScheduleMgt.load();

  // Configuracion de programacion
  if (DomDomScheduleMgt.schedulePoints.size() == 0)
  {
    DomDomSchedulePoint *point = new DomDomSchedulePoint(ALL, 13, 36);
    point->value[0] = 100;
    point->value[1] = 100;
    point->value[2] = 100;
    DomDomScheduleMgt.schedulePoints.push_back(point);

    DomDomSchedulePoint *point2 = new DomDomSchedulePoint(ALL, 13, 38);
    point2->value[0] = 100;
    point2->value[1] = 0;
    point2->value[2] = 0;
    DomDomScheduleMgt.schedulePoints.push_back(point2);

    DomDomSchedulePoint *point3 = new DomDomSchedulePoint(ALL, 23, 13);
    point3->value[0] = 100;
    point3->value[1] = 0;
    point3->value[2] = 0;
    DomDomScheduleMgt.schedulePoints.push_back(point3);

    DomDomSchedulePoint *point4 = new DomDomSchedulePoint(ALL, 23, 15);
    point4->value[0] = 100;
    point4->value[1] = 100;
    point4->value[2] = 100;
    DomDomScheduleMgt.schedulePoints.push_back(point4);

  }
  
  // Servidor web
  DomDomWebServer.begin();

  // // Control del ventilador
  DomDomFanControl.load();

}

void loop()
{

  /************************************************
   * 
   *  EN ESTE PROYECTO EL LOOP() ESTARA VACIO 
   *  YA QUE APROVECHAMOS LAS TASK PARA SACARLE
   *  EL MAXIMO PARTIDO A LOS DOS CORES DEL ESP.
   * 
   * *********************************************/

}