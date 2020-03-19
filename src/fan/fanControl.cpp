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

DomDomFanControlClass::DomDomFanControlClass()
{
    xMutex = xSemaphoreCreateMutex();

    pinMode(FAN_PWM_PIN, OUTPUT);
    pinMode(FAN_PWM_FEEDBACK_PIN, INPUT);

    min_pwm = 0;
    curr_pwm = 0;
    max_pwm = pow(2, FAN_PWM_RESOLUTION);

    _histeresis = FAN_HISTERESIS;
    min_temp = FAN_MIN_TEMP;
    max_temp = FAN_MAX_TEMP;

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
        vTaskDelete(taskHandle);
    }
}

void DomDomFanControlClass::fanTask(void * parameter)
{
    while(true)
    {
        DomDomFanControl.update();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    ledcWrite(FAN_PWM_CHANNEL, new_pwm);
}

int DomDomFanControlClass::getTemperature()
{
    // Aqui va el codigo para leer de la sonda
    // de momento devolvemos un valor fijo.

    return 25;
}

float DomDomFanControlClass::getVoltaje()
{
    xSemaphoreTake(xMutex, portMAX_DELAY);

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

    xSemaphoreGive(xMutex);

    value = value / samples_num;
    value = uPvolts * value / ADBits / r2 * (r1+r2) + offset;

    return value;
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomFanControlClass DomDomFanControl;
#endif