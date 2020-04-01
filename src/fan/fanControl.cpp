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

#include "fanControl.h"
#include "configuration.h"
#include <EEPROM.h>

DomDomFanControlClass::DomDomFanControlClass()
{
    volt_xMutex = xSemaphoreCreateMutex();
    temp_xMutex = xSemaphoreCreateMutex();

    pinMode(FAN_PWM_PIN, OUTPUT);
    pinMode(FAN_PWM_FEEDBACK_PIN, INPUT);

    _histeresis = FAN_HISTERESIS;
    min_pwm = 0;
    curr_pwm = 0;
    max_pwm = pow(2, FAN_PWM_RESOLUTION);

    min_temp = FAN_MIN_TEMP;
    max_temp = FAN_MAX_TEMP;
    _curr_temp = 0;

    oneWire = OneWire(FAN_TEMP_SENSOR_PIN);
    sensors = new DallasTemperature(&oneWire);
    sensors->begin();

    // configure LED PWM functionalitites
    ledcSetup(FAN_PWM_CHANNEL, 5000, FAN_PWM_RESOLUTION);
    
    // attach the channel to be controlled
    ledcAttachPin(FAN_PWM_PIN, LED_STATUS_CHANNEL);

    // init PWM
    ledcWrite(LED_STATUS_CHANNEL, min_pwm);   
}

void DomDomFanControlClass::begin()
{
    if (!_started)
    {
        _started = true;

        xTaskCreate(
            this->fanTask,          /* Task function. */
            "fantTask",             /* String with name of task. */
            10000,                  /* Stack size in bytes. */
            NULL,                   /* Parameter passed as input of the task */
            1,                      /* Priority of the task. */
            taskHandle              /* Task handle. */
        );
    }
}

void DomDomFanControlClass::end()
{
    if (_started)
    {
        _started = false;
    }
}

void DomDomFanControlClass::fanTask(void * parameter)
{
    while(DomDomFanControl.isStarted())
    {
        DomDomFanControl.update();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

void DomDomFanControlClass::update()
{
    fan_voltaje = getVoltaje();

    int prev_temp = _curr_temp;         /* temperatura previa */
    _curr_temp = getTemperature();    /* temperatura del sensor */

    int calc_min_temp = min_temp;
    if (_curr_temp < prev_temp && curr_pwm == 0) /* si la temperatura va descendiento y esta activo el pwm*/
    {
        calc_min_temp -= _histeresis;
    }

    int new_pwm = 0;
    if (_curr_temp >= max_temp)
    {
        new_pwm = max_pwm;
    }
    else if(_curr_temp <= calc_min_temp)
    {
        new_pwm = min_pwm;
    }
    else
    {
        int rango_total = max_temp - calc_min_temp;
        
        int valor_actual = _curr_temp - calc_min_temp;
        double porcentaje_actual = (valor_actual * 100) / rango_total / double(100);
        new_pwm = min_pwm + (max_pwm - min_pwm) * porcentaje_actual;
    }

    if (new_pwm != curr_pwm)
    {
        setCurrentPWM(new_pwm);
    }
    
}

float DomDomFanControlClass::getTemperature()
{
    // xSemaphoreTake(temp_xMutex, portMAX_DELAY);
    // sensors -> requestTemperatures(); 
    // float temp = sensors->getTempCByIndex(0);
    // xSemaphoreGive(temp_xMutex);

    return 25; /** Devolvemos un valor fijo hasta que tengamos la sonda instalada */
}

float DomDomFanControlClass::getVoltaje()
{
    xSemaphoreTake(volt_xMutex, portMAX_DELAY);

    int samples_num = 10;

    float ADBits = 4095;
    float r1 = FAN_FEEDBACK_R1;
    float r2 = FAN_FEEDBACK_R2;
    float uPvolts = 3.29f;
    float offset = FAN_FEEDBACK_OFFSET;

    float value = 0.0f;
    for (int i = 0; i < samples_num; i++)
    {
        value += analogRead(FAN_PWM_FEEDBACK_PIN);
        delay(5);
    }

    xSemaphoreGive(volt_xMutex);

    value = value / samples_num;
    value = uPvolts * value / ADBits / r2 * (r1+r2) + offset;

    return value;
}

void DomDomFanControlClass::setCurrentPWM(uint16_t pwm)
{
    uint16_t max = pow(2,FAN_PWM_RESOLUTION);
    if (pwm > max)
    {
        Serial.printf("[FAN] ERROR: valor %hu pwm mayor que el maximo. Valor cambiado a %hu\n", pwm, max);
        pwm =  max;
    }

    if (pwm < 0)
    {
        Serial.printf("[FAN] ERROR: valor %d pwm menor que 0. Valor cambiado a 0\n", pwm);
        pwm = 0;
    }

    ledcWrite(FAN_PWM_CHANNEL, pwm);

    curr_pwm = pwm;
}

bool DomDomFanControlClass::save()
{
    int address = EEPROM_FAN_ENABLED_ADDRESS;
    EEPROM.writeBool(address, _started);
    address += 1;
    EEPROM.writeUShort(address, max_pwm);
    address += 2;
    EEPROM.writeUShort(address, min_pwm);
    address += 2;
    EEPROM.writeUShort(address, max_temp);
    address += 2;
    EEPROM.writeUShort(address, min_temp);
    address += 2;
    EEPROM.writeUShort(address, curr_pwm);

    return EEPROM.commit();
}

bool DomDomFanControlClass::load()
{
    if (isStarted())
    {
        end();
    }

    int address = EEPROM_FAN_ENABLED_ADDRESS;
    address+=1;
    max_pwm = EEPROM.readUShort(address);
    address += 2;
    min_pwm = EEPROM.readUShort(address);
    address += 2;
    max_temp = EEPROM.readUShort(address);
    address += 2;
    min_temp = EEPROM.readUShort(address);
    address += 2;
    setCurrentPWM(EEPROM.readUShort(address));

    if (EEPROM.readBool(EEPROM_FAN_ENABLED_ADDRESS))
    {
        begin();
    }

    return true;
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomFanControlClass DomDomFanControl;
#endif