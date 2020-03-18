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

#include "statusLedControl.h"
#include "configuration.h"

DomDomStatusLedControlClass::DomDomStatusLedControlClass()
{
    xMutex = xSemaphoreCreateMutex();
};

void DomDomStatusLedControlClass::begin()
{
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);

    min_pwm = 0;
    max_pwm = pow(2, LED_STATUS_RESOLUTION);

    // configure LED PWM functionalitites
    ledcSetup(LED_STATUS_CHANNEL, 5000, LED_STATUS_RESOLUTION);
    
    // attach the channel to be controlled
    ledcDetachPin(LED_BLUE_PIN);
    ledcDetachPin(LED_GREEN_PIN);
    ledcAttachPin(LED_RED_PIN, LED_STATUS_CHANNEL);

    // init PWM
    ledcWrite(LED_STATUS_CHANNEL, max_pwm); 

}

void DomDomStatusLedControlClass::setWifiMode(int mode)
{
    if (mode != wifi_mode)
    {
        if (mode == 2) /* Modo AP mostramos azul */
        {
            ledcDetachPin(LED_RED_PIN);
            ledcAttachPin(LED_BLUE_PIN, LED_STATUS_CHANNEL);
        }
        else /* modo STA mostramos verde */
        {
            ledcDetachPin(LED_RED_PIN);
            ledcAttachPin(LED_GREEN_PIN, LED_STATUS_CHANNEL);
        }
        
        wifi_mode = mode;
    }
}

void DomDomStatusLedControlClass::blink( int times)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );
    for (int i = 0; i < times; i++)
    {
        ledcWrite(LED_STATUS_CHANNEL, min_pwm);
        delay(LED_STATUS_BLINK_DELAY);
        ledcWrite(LED_STATUS_CHANNEL, max_pwm);
        delay(LED_STATUS_BLINK_DELAY);
    }
    xSemaphoreGive( xMutex );
}

void DomDomStatusLedControlClass::blinkError( int times)
{
    xSemaphoreTake( xMutex, portMAX_DELAY );

    int current_mode = wifi_mode;

    ledcDetachPin(LED_BLUE_PIN);
    ledcDetachPin(LED_GREEN_PIN);
    ledcAttachPin(LED_RED_PIN, LED_STATUS_CHANNEL);
    
    for (int i = 0; i < times; i++)
    {
        ledcWrite(LED_STATUS_CHANNEL, min_pwm);
        delay(LED_STATUS_BLINK_DELAY);
        ledcWrite(LED_STATUS_CHANNEL, max_pwm);
        delay(LED_STATUS_BLINK_DELAY);
    }

    wifi_mode = -1;
    setWifiMode(current_mode);
    
    xSemaphoreGive( xMutex );
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomStatusLedControlClass DomDomStatusLedControl;
#endif