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
#include "channelMgt.h"
#include "statusLedControl/statusLedControl.h"

DomDomScheduleMgtClass::DomDomScheduleMgtClass(/* args */)
{
}

DomDomScheduleMgtClass::~DomDomScheduleMgtClass()
{
}


bool DomDomScheduleMgtClass::getShedulePoint(DateTime &dt, DomDomSchedulePoint *&point, bool previous)
{
    DateTime now = DomDomRTC.now();
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

    Serial.printf("[Debug] %d", puntoAnterior);
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
            EEPROM.write(address++, schedulePoints[i]->value.size());

            for(int j = 0; j < schedulePoints[i]->value.size(); j++)
            {
                EEPROM.write(address++, schedulePoints[i]->value[j]);
            }
        };
    }
    
    EEPROM.writeBool(EEPROM_SCHEDULE_STATUS_ADDRESS, _started);

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
            
            uint8_t count = EEPROM.read(address++);
            for(int j = 0; j < count; j++)
            {
                schedulePoints[schedulePoints.size()-1]->value[j] = EEPROM.read(address++);
            }
        };

        Serial.printf("Cargados %d puntos de programacion correctamente.\n", count);
    }else{
        Serial.print("No hay puntos de programacion guardados\n");
    }
    
    if (EEPROM.readBool(EEPROM_SCHEDULE_STATUS_ADDRESS))
    {
        begin();
    }

    return true;

}

void DomDomScheduleMgtClass::addSchedulePoint(DomDomDayOfWeek day, uint8_t hour, uint8_t minute, bool fade)
{
    if (schedulePoints.size() >= EEPROM_MAX_SCHEDULE_POINTS)
    {
        Serial.println("ERROR: Numero maximo de programaciones alcanzado!");
        return;
    }

    schedulePoints.push_back(new DomDomSchedulePoint(day, hour, minute, fade));
}

bool DomDomScheduleMgtClass::begin()
{
    if (!_started)
    {
        _started = true;

        xTaskCreate(
            this->scheduleTask,            /* Task function. */
            "ScheduleInitTask",     /* String with name of task. */
            10000,                  /* Stack size in bytes. */
            NULL,                   /* Parameter passed as input of the task */
            1,                      /* Priority of the task. */
            taskHandle              /* Task handle. */
        );
    }

    return true;
}

bool DomDomScheduleMgtClass::end()
{
    if (_started)
    {
        _started = false;
    }

    return true;
}

void DomDomScheduleMgtClass::update()
{
    DomDomStatusLedControl.blink(1);

    DateTime now = DomDomRTC.now();
    Serial.printf("[Schedule] %d:%d Comprobando programacion\n", now.hour(), now.minute());

    DateTime horaAnterior;
    DateTime horaSiguiente;
    DomDomSchedulePoint *puntoAnterior = nullptr;
    DomDomSchedulePoint *puntoSiguiente = nullptr;

    bool correct;
    correct = getShedulePoint(horaAnterior, puntoAnterior, true);
    if (!correct)
    {
        Serial.println("ERROR: No se encontra una programacion previa.");
        return;
    }

    correct = getShedulePoint(horaSiguiente, puntoSiguiente, false);
    if (!correct)
    {
        Serial.println("ERROR: No se encontra una programacion siguiente.");
        return;
    }

    Serial.printf("[Schedule] intervalo obtenido: %d:%d - %d:%d\n", puntoAnterior->hour, puntoAnterior->minute, puntoSiguiente->hour, puntoSiguiente->minute);

    for(int i = 0; i < DomDomChannelMgt.channels.size(); i++)
    {
        DomDomChannelClass *channel = DomDomChannelMgt.channels[i];
        int channel_num = channel->getNum();
        if (!puntoSiguiente->fade || puntoAnterior->value[channel_num] == puntoSiguiente->value[channel_num])
        {
            int pwm = 0;
            double porcentaje = puntoSiguiente->value[channel_num] / 100;
            pwm = channel->min_limit_pwm + ((channel->max_limit_pwm - channel->min_limit_pwm) * porcentaje);
            channel->setPWMValue(pwm);
        }
        else
        {
            int pwm = calcFadeValue(puntoAnterior->value[channel_num], 
                                    puntoSiguiente->value[channel_num],
                                    channel->min_limit_pwm,
                                    channel->max_limit_pwm,
                                    horaAnterior,
                                    horaSiguiente);
            
            channel->setPWMValue(pwm);
        }
    }
}

void DomDomScheduleMgtClass::scheduleTask(void *parameter)
{
    int offset = 5;
    while(DomDomScheduleMgt.isStarted())
    {
        DomDomScheduleMgt.update();
        const int next_ms = (60 - DomDomRTC.now().second() + offset) * 1000;
        vTaskDelay(next_ms / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

int DomDomScheduleMgtClass::calcFadeValue(int prevValue, int nextValue, int min_value, int max_value, DateTime anterior, DateTime siguiente)
{
    int minutes_total = (siguiente - anterior).totalseconds() / 60;

    // Si la hora es la misma ajustamos directamente el valor.
    if (minutes_total == 0)
    {
        Serial.println("WARN: Misma hora de inicio y de final. ¿Error en programacion?");
        int new_pwm = 0;
        double porcentaje = nextValue / 100;
        new_pwm = min_value + ((max_value - min_value) * porcentaje);
        
        return new_pwm;
    }
    else
    {
        DateTime now = DomDomRTC.now();
        DateTime now_noseconds (now.year(),now.month(),now.day(),now.hour(), now.minute(), 0);

        int minutes = (now_noseconds - anterior).totalseconds() / 60;
        double minutes_porcentaje = (minutes * 100) / minutes_total / double(100);

        int porcentaje_valor = (nextValue - prevValue) * minutes_porcentaje;
        int new_pwm = min_value + (max_value - min_value) * ((prevValue + porcentaje_valor) / double(100));

        return new_pwm;
    }   

    return 0;
}

void DomDomScheduleMgtClass::startTest(uint16_t pwm[])
{
    if (_testInProgress)
    {
        _testInProgress = false;
        delay(500);
    }

    _testInProgress = true;
    end();

    for(int i = 0; i < DomDomChannelMgt.channels.size(); i++)
    {
        DomDomChannelMgt.channels[i]->setPWMValue(pwm[i]);
    }

    xTaskCreate(
        this->testTask,     /* Task function. */
        "testTask",         /* String with name of task. */
        10000,              /* Stack size in bytes. */
        NULL,               /* Parameter passed as input of the task */
        1,                  /* Priority of the task. */
        NULL                /* Task handle. */
    );

}

void DomDomScheduleMgtClass::testTask(void *parameter)
{
    for(int i = 0; i < 300; i++)
    {
        if (!DomDomScheduleMgt.testInProgress())
        {
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    DomDomScheduleMgt.stopTest();
    DomDomScheduleMgt.begin();

    vTaskDelete(NULL);
}

void DomDomScheduleMgtClass::stopTest()
{
    _testInProgress = false;
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomScheduleMgtClass DomDomScheduleMgt;
#endif