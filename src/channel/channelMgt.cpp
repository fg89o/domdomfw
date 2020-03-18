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

#include "channelMgt.h"
#include "configuration.h"
#include <EEPROM.h>

DomDomChannelMgtClass::DomDomChannelMgtClass()
{
    int count = 0;
    channels.clear();
    channels.push_back(new DomDomChannelClass(count++, CHANNEL_1_PWM_PIN, CHANNEL_1_RESOLUTION));
    channels.push_back(new DomDomChannelClass(count++, CHANNEL_2_PWM_PIN, CHANNEL_2_RESOLUTION));
    channels.push_back(new DomDomChannelClass(count++, CHANNEL_3_PWM_PIN, CHANNEL_3_RESOLUTION));
    channels.push_back(new DomDomChannelClass(count++, CHANNEL_4_PWM_PIN, CHANNEL_4_RESOLUTION));
    channels.push_back(new DomDomChannelClass(count++, CHANNEL_5_PWM_PIN, CHANNEL_5_RESOLUTION));
};

void DomDomChannelMgtClass::begin()
{
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->begin();
    }
};

void DomDomChannelMgtClass::end()
{
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->end();
    }
}

void DomDomChannelMgtClass::loadAll()
{
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->loadFromEEPROM();
    }
}

void DomDomChannelMgtClass::saveAll()
{
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->save();
    }
}

void DomDomChannelMgtClass::initEEPROM()
{
    Serial.print("...Incializando EEPROM de canales...\n");
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->initEEPROM();
    }
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomChannelMgtClass DomDomChannelMgt;
#endif