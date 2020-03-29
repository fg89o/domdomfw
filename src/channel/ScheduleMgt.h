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
#ifndef DOMDOM_SCHEDULEMGT_h
#define DOMDOM_SCHEDULEMGT_h

#include <Arduino.h>
#include "schedulePoint.h"
#include "../rtc/rtc.h"

/**
 * Clase encargada de la programacion.
 * 
 * Una vez iniciada esta clase comprobara los
 * puntos de programacion cargados e ira modificando
 * los canales en funcion del valor que le corresponda.
 */
class DomDomScheduleMgtClass
{
    private:
        /**
         * Indica si hay un test en progreso
         */
        bool _testInProgress = false;
        /**
         * Indica si el proceso está activado.
         */
        bool _started = false;
        /**
         * Puntero a la tarea del programador.
         */
        TaskHandle_t *taskHandle;
        /**
         * Tarea del programador.
         */
        static void scheduleTask(void * parameter);
        /**
         * Tarea del programador.
         */
        static void testTask(void * parameter);
        /**
         * Devuelve el valor proporcional entre @prevValue y @nextValue en base a @anterior y @siguiente.
         */
        int calcFadeValue(int prevValue, int nextValue, int min_value, int max_value, DateTime anterior, DateTime siguiente);

    public:
        /**
         * Constructor.
         */
        DomDomScheduleMgtClass();
        /**
         * Destructor.
         */
        ~DomDomScheduleMgtClass();
        /**
         * Indica si hay algun test en proceso
         */
        bool testInProgress() const { return _testInProgress; };
        /**
         * Puntos de programacion cargados.
         */
        std::vector<DomDomSchedulePoint *> schedulePoints;
        /**
         * Guarda los puntos de programacion cargados en la memoria y el estado
         */
        bool save();
        /**
         * Borra los puntos de programacion actuales y 
         * carga los valores guardados en memoria.
         */
        bool load();
        /**
         * Inicia el programador.
         * */
        bool begin();
        /**
         * Comprueba la programacion.
         */
        void update();
        /**
         * Para el programador.
         */
        bool end();
        /**
         * Indica si el programador esta en marcha o no.
         */
        bool isStarted() const { return _started; };
        /**
         * Añade un nuevo punto de programacion con los valores pasados por parametro.
         */
        void addSchedulePoint(DomDomDayOfWeek day, uint8_t hour, uint8_t minute, bool fade);
        /**
         * Devuelve un punto de programacion.
         * 
         * Si @previous es verdadero rellena @point con el punto anterior mas cercano al parametro @dt,
         * Si @previous es falso rellena @point con el punto siguiente mas cercano al parametro @dt
         * Devuelve un booleano indicando si se ha encontrado un punto de programacion o no.
         */
        bool getShedulePoint(DateTime &dt, DomDomSchedulePoint *&point, bool previous);
        /**
         * Realiza un test con los valores pasados por parametros
         */
        void startTest(uint16_t pwm[]);
        /**
         * Para el test
         */
        void stopTest();
};


#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomScheduleMgtClass DomDomScheduleMgt;
#endif

#endif /* DOMDOM_SCHEDULEMGT_h */