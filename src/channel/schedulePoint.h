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
#ifndef DOMDOM_SCHEDULEPOINT_h
#define DOMDOM_SCHEDULEPOINT_h

/**
 * Enumerado para los dias de la semana
 */
enum DomDomDayOfWeek
{
    DOMINGO = 1,
    LUNES = 2,
    MARTES = 4,
    MIERCOLES = 8,
    JUEVES = 16,
    VIERNES = 32,
    SABADO = 64,
    SEMANA = 62,
    FESTIVO = 65,
    ALL = 127,
};

/**
 * Punto de programacion.
 * 
 * Esta clase representa un punto de programacion.
 * Se configura una hora, un valor para cada canal y 
 * un booleano que indica si el cambio es progresivo
 * en el tiempo o no.
 */
class DomDomSchedulePoint
{
    public:
        /**
         * Constructor por defecto para un punto vacio.
         * */
        DomDomSchedulePoint();
        /**
         * Constructor para inicializar con ciertos valores.
         */
        DomDomSchedulePoint(DomDomDayOfWeek dayOfWeek, char hour, char minute, bool fade = true);
        /**
         * Indica si el cambio es progresivo en el tiempo o no.
         */
        bool fade = true;
        /**
         * Dia de la semana a la que afecta el punto de programacion.
         * NO IMPLEMENTADO!!!!
         */
        DomDomDayOfWeek dayOfWeek;
        /**
         * Hora del punto de programacion.
         */
        char hour;
        /**
         * Minuto del punto de programacion.
         */
        char minute;
        /**
         * Array con los valores para cada canal.
         */
        char value[5] = {0,0,0,0,0};
};

#endif /* DOMDOM_SCHEDULEPOINT_h */