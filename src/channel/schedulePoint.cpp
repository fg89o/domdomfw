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

#include "schedulePoint.h"

DomDomSchedulePoint::DomDomSchedulePoint(){};
DomDomSchedulePoint::DomDomSchedulePoint(DomDomDayOfWeek _day, char _hour, char _minute, bool _fade)
{
    dayOfWeek = _day;
    hour = _hour;
    minute = _minute;
    fade = _fade;
}