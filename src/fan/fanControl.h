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

#pragma once
#ifndef DOMDOM_FANCONTROL_h
#define DOMDOM_FANCONTROL_h

#include <Arduino.h>
#include "configuration.h"
#include "OneWire.h"
#include "DallasTemperature.h"

/**
 * Clase para el control del ventilador
 * 
 * Esta clase sera la encargada de controlar el ventilador.
 * 
 */
class DomDomFanControlClass
{
    private:
        /**
         * Indica si el proceso está activado.
         */
        bool _started = false;
        /**
         * Puntero a la tarea del ventilador.
         */
        TaskHandle_t *taskHandle;
        /**
         * Tarea de control.
         */
        static void fanTask(void * parameter);
        /**
         * Valor anterior de temperatura
         */
        float _curr_temp;
        /**
         * Valor de histeresis
         */
        int _histeresis;
        /**
         * Calcula el voltaje actual
         */
        float getVoltaje();
        /**
         * Mutex para el voltaje
         */
        SemaphoreHandle_t volt_xMutex;
        /**
         * Mutex para la temperatura
         */
        SemaphoreHandle_t temp_xMutex;
        /**
         * Objeto para la comunicacion oneWire con la sonda
         */
        OneWire oneWire;
        /**
         * Objeto para la temperatura
         */
        DallasTemperature *sensors;


    public:
        /**
         * Constructor.
         * */
        DomDomFanControlClass();
        /**
         * Nivel PWM para cuando la temperatura maxima.
         */
        uint16_t max_pwm;
        /**
         * Nivel PWM para la temperatura minima.
         */
        uint16_t min_pwm;
        /**
         * PWM actual
         */
        uint16_t curr_pwm;
        /**
         * Temperatura para el valor min_pwm
         */
        uint16_t min_temp;
        /**
         * Temperatura para el valor max_pwm
         */
        uint16_t max_temp;
        /**
         * Voltaje al que esta funcionando el ventilador
         */
        float fan_voltaje;
        /**
         * Inicia el proceso de control del ventialdor.
         */
        void begin();
        /**
         * Para el proceso de control de ventilador.
         */
        void end();
        /**
         * Comprueba el estado actual y actualiza los valores
         */
        void update();
        /**
         * Devuelve la temperatura actual
         */
        float getTemperature();
        /**
         * Indica si el control de ventilador esta activo
         */
        bool isStarted() const {return _started; };
        /**
         * Establece el PWM para el ventialdor
         */
        void setCurrentPWM(uint16_t pwm);
        /**
         * Guarda el estado actual en la memoria
         */
        bool save();
        /**
         * Carga los valores almacenados en memoria
         */
        bool load();
};


#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomFanControlClass DomDomFanControl;
#endif

#endif /* DOMDOM_FANCONTROL_h */