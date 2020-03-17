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
#ifndef DOMDOM_CHANNELMGT_h
#define DOMDOM_CHANNELMGT_h

#include <Arduino.h>
#include "channel.h"

class DomDomChannelMgtClass
{
    public:
        DomDomChannelMgtClass();
        std::vector<DomDomChannelClass *> channels;
        void begin();
        void end();
        void update();
        void saveAll();
        void loadAll();
        void initEEPROM();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomChannelMgtClass DomDomChannelMgt;
#endif

#endif /* DOMDOM_CHANNELMGT_h */