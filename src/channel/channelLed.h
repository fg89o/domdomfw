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
#ifndef DOMDOM_CHANNELLED_h
#define DOMDOM_CHANNELLED_h

#include <Arduino.h>

/**
 * Representa el tipo de led de un canal
 * 
 * A traves de esta clase podremos almacenar la configuracion
 * de leds en cada uno de los canales. Con esta informacion
 * podremos sacar graficas del espectro.
 */
class DomDomChannelLed
{
    
    public:
        /**
         * Constructor.
         */
        DomDomChannelLed(){};
        /**
         * Temnperatura de color.
         */
        uint16_t K;
        /**
         * Potencia
         */
        uint16_t W;
        /**
         * Longitud de onda
         */
        uint16_t nm;
};

#endif /* DOMDOM_CHANNELLED_h */