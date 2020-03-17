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

class DomDomScheduleMgtClass
{
    private:
        bool _started = false;
        TaskHandle_t *taskHandle;
        static void tInit(void * parameter);

    public:
        std::vector<DomDomSchedulePoint *> schedulePoints;
        bool getShedulePoint(DateTime &dt, DomDomSchedulePoint *&point, bool previous);
        bool save();
        bool load();
        bool begin();
        bool end();
        bool isStarted() const { return _started; };
        void addSchedulePoint(DomDomDayOfWeek day, uint8_t hour, uint8_t minute, bool fade);
        DomDomScheduleMgtClass();
        ~DomDomScheduleMgtClass();
};


#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomScheduleMgtClass DomDomScheduleMgt;
#endif

#endif /* DOMDOM_SCHEDULEMGT_h */