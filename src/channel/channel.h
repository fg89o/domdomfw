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
#ifndef DOMDOM_CHANNEL_h
#define DOMDOM_CHANNEL_h

#include <Arduino.h>
#include "ScheduleMgt.h"
#include "../rtc/rtc.h"


class DomDomChannelClass
{
    private:
        uint8_t _pwm_pin;
        uint16_t _current_pwm;
        uint8_t _channel_num;
        uint8_t _resolution;
        uint16_t _min_pwm;
        uint16_t _max_pwn;
        bool _ready;
        bool _enabled;
        void calcPWMLineal(DomDomSchedulePoint *previous, DomDomSchedulePoint *next, DateTime &horaAnterior, DateTime &horaSiguiente);
        void calcPWMNoLineal(DomDomSchedulePoint *previous, DomDomSchedulePoint *next, DateTime &horaAnterior, DateTime &horaSiguiente);

    public:
        DomDomChannelClass(char num, char pwm_pin, char resolution);
        DomDomChannelClass(char num);
        uint16_t max_limit_pwm;
        uint16_t min_limit_pwm;
        uint8_t pwm_pin() const {return _pwm_pin; };
        uint16_t current_pwm() const {return _current_pwm; }
        uint8_t getNum() const { return _channel_num; }
        uint8_t getResolution() const { return _resolution; }
        bool getEnabled() const {return _enabled; };
        bool begin();
        bool end();
        bool setPWMValue(uint16_t value);
        bool setPWMResolution(uint8_t value);
        bool setEnabled(bool enabled);
        void update();
        bool save();
        bool loadFromEEPROM();
        bool saveCurrentPWM();
        void initEEPROM();
};

#endif /* DOMDOM_CHANNEL_h */