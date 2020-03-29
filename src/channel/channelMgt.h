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

/**
 * Clase que contiene los canales disponibles
 */
class DomDomChannelMgtClass
{
    private:
        bool _testInProgress = false;

    public:
        /**
         * Constructor
         */
        DomDomChannelMgtClass();
        /**
         * Array con todos los canales disponibles.
         */
        std::vector<DomDomChannelClass *> channels;
        /**
         * Devuelve un flag indicando si ya hay un test en progreso
         */
        int testInProgress() const {return _testInProgress; };
        /**
         * Inicia todos los canales.
         */
        void begin();
        /**
         * Para todos los canales.
         */
        void end();
        /**
         * Guarda la configuracion de todos los canales en memoria.
         */
        void saveAll();
        /**
         * Invalida la configuracion actual y carga los valores
         * guardados en memoria de todos los canales.
         */
        void loadAll();
        /**
         * Inicializa la memoria para todos los canales.
         */
        void initEEPROM();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomChannelMgtClass DomDomChannelMgt;
#endif

#endif /* DOMDOM_CHANNELMGT_h */