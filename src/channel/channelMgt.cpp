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
    channels.clear();

    int pins[CHANNEL_SIZE] = CHANNEL_PWM_PINS;
    int resolution[CHANNEL_SIZE] = CHANNEL_RESOLUTIONS;

    for (int i = 0; i < CHANNEL_SIZE; i++)
    {
        DomDomChannelClass *channel = new DomDomChannelClass(i, pins[i], resolution[i]);
        channels.push_back(channel);

        DomDomChannelLed *led = new DomDomChannelLed();
        led->K = 6500;
        led->nm = 0;
        led->W = 1;

        channel->leds.push_back(led);
    }

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
    for (int i = 0; i < channels.size() ; i++)
    {
        channels[i]->initEEPROM();
    }
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomChannelMgtClass DomDomChannelMgt;
#endif