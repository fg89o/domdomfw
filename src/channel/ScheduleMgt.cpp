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

#include "ScheduleMgt.h"
#include <EEPROM.h>
#include "configuration.h"

DomDomScheduleMgtClass::DomDomScheduleMgtClass(/* args */)
{
}

DomDomScheduleMgtClass::~DomDomScheduleMgtClass()
{
}


bool DomDomScheduleMgtClass::getShedulePoint(DateTime &dt, DomDomSchedulePoint *&point, bool previous)
{
    DateTime now = DomDomRTC.rtc.now();
    now = now - TimeSpan(now.second());

    DateTime ultimaHora;
    int puntoAnterior = -1;

    for(int i = 0; i < schedulePoints.size(); i++)
    {
        // Buscamos la hora mayor del dia anterior;
        DateTime scheduleDT (now.year(), now.month(), now.day(), schedulePoints[i]->hour, schedulePoints[i]->minute, 0);
        TimeSpan day (1,0,0,0);

        // Si buscamos la fecha anterior y no es menor que la fecha actual, restamos un dia
        if (previous && scheduleDT >= now)
        {
            scheduleDT = scheduleDT - day;
        }

        // Si buscamos la fecha siiguiente y es menor que la actual sumamos un dia
        if (!previous && scheduleDT < now)
        {
            scheduleDT = scheduleDT + day;
        }

        // Si buscamos la fecha anterior
        if (previous)
        {
            if (scheduleDT < now)
            {
                if (puntoAnterior == -1)
                {
                    ultimaHora = scheduleDT;
                    puntoAnterior = i;
                }
                else if (scheduleDT > ultimaHora)
                {
                    ultimaHora = scheduleDT;
                    puntoAnterior = i;
                }
            }
        }

        // Si buscamos la fecha siguiente
        if (!previous)
        {
            if (scheduleDT >= now)
            {
                if (puntoAnterior == -1)
                {
                    ultimaHora = scheduleDT;
                    puntoAnterior = i;
                }
                else if (scheduleDT < ultimaHora)
                {
                    ultimaHora = scheduleDT;
                    puntoAnterior = i;
                }
            }
        }
        
    }

    if (puntoAnterior == -1)
    {
        return false;
    }

    dt = ultimaHora;
    point = schedulePoints[puntoAnterior];
    return true;
}

bool DomDomScheduleMgtClass::save()
{
    int address = EEPROM_SCHEDULE_FIRST_ADDRESS ;
    EEPROM.write(address++, (uint8_t)schedulePoints.size());
    Serial.println("Guardando programacion...");

    if (schedulePoints.size() > 0)
    {
        for (int i = 0; i < schedulePoints.size(); i++)
        {
            EEPROM.write(address++, schedulePoints[i]->dayOfWeek);
            EEPROM.write(address++, schedulePoints[i]->hour);
            EEPROM.write(address++, schedulePoints[i]->minute);
            EEPROM.write(address++, schedulePoints[i]->fade);

            for(int j = 0; j < 5; j++)
            {
                EEPROM.write(address++, schedulePoints[i]->value[j]);
            }
        };
    }
    
    bool result = EEPROM.commit();

    if (result)
    {
        Serial.println("Programacion guardada correctamente.");
    }
    else
    {
        Serial.println("ERROR: No se pudo guardar la programacion.");
    }
        

    return result;

}

bool DomDomScheduleMgtClass::load()
{
    schedulePoints.clear();

    Serial.println("Cargando programacion...");

    int address = EEPROM_SCHEDULE_FIRST_ADDRESS ;
    uint8_t count = EEPROM.read(address++);

    if (count > 0 && count <= EEPROM_MAX_SCHEDULE_POINTS)
    {
        Serial.printf("Encontrados %d puntos\n", count);
        for (int i = 0; i < count; i++)
        {
            
            DomDomDayOfWeek day = (DomDomDayOfWeek)EEPROM.read(address++);
            uint8_t hour = EEPROM.read(address++);
            uint8_t minute = EEPROM.read(address++);
            uint8_t fade = EEPROM.read(address++);
            
            addSchedulePoint(day,hour,minute,fade);

            for(int j = 0; j < 5; j++)
            {
                schedulePoints[schedulePoints.size()-1]->value[j] = EEPROM.read(address++);
            }
        };

        Serial.printf("Cargados %d puntos de programacion correctamente.\n", count);
    }else{
        Serial.print("No hay puntos de programacion guardados\n");
    }
    
    return true;

}

void DomDomScheduleMgtClass::addSchedulePoint(DomDomDayOfWeek day, uint8_t hour, uint8_t minute, bool fade)
{
    if (schedulePoints.size() >= EEPROM_MAX_SCHEDULE_POINTS)
    {
        Serial.println("ERROR: Numero maximo de programaciones alcanzado");
        return;
    }

    schedulePoints.push_back(new DomDomSchedulePoint(day, hour, minute, fade));
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomScheduleMgtClass DomDomScheduleMgt;
#endif