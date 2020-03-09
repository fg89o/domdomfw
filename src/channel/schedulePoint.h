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

class DomDomSchedulePoint
{
    public:
        DomDomSchedulePoint();
        DomDomSchedulePoint(DomDomDayOfWeek dayOfWeek, char hour, char minute, bool fade = true);
        bool fade = true;
        DomDomDayOfWeek dayOfWeek;
        char hour;
        char minute;
        char value[5] = {0,0,0,0,0};
};

#endif /* DOMDOM_SCHEDULEPOINT_h */