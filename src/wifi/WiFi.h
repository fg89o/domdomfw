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
#ifndef DOMDOM_WIFICLASS_h
#define DOMDOM_WIFICLASS_h
#include <Arduino.h>

/**
 * Clase encargada de la conexion Wifi, tanto STA como AP.
 * 
 * Al iniciar la conexion la clase intentara conectarse a la
 * red wifi que tenga en memoria, en caso de no poder conectarse
 * o de no tener ninguna en memoria creara su propia red para que 
 * el equipo siempre sea accesible. Una vez establecida una conexion,
 * en funcion de los parametros, el equipo activara el servicio mDNS,
 * para que se pueda acceder mediante un nombre.
 * 
 */
class DomDomWifiClass
{
    private:
        /**
         * Intenta la configuracion AP.
         */
        bool createOwnAPWifi();
        /**
         * Intenta conectarse en modo STA.
         */
        int connectSTAWifi();
        /**
         * Inicia el proceso de conexion STA o AP.
         */
        void connect();
        /**
         * Indica si hay alguna red conectada STA o AP.
         */
        bool _connected;

    public:
        /**
         * Constructor
         */
        DomDomWifiClass();
        /**
         * SSID de la red a la que nos conectaremos.
         */
        String ssid;
        /**
         * Password de la red guardada en el ssid.
         */
        String pwd;
        /**
         * Indica si la conexion STA esta habilitada.
         */
        bool sta_enabled = true;
        /**
         * Indica si el servicio mDNS esta habilitado.
         */
        bool mDNS_enabled;
        /**
         * Nombre de host para el servicio mDNS.
         */
        String mDNS_hostname;
        /**
         * Inicia el proceso de conexion.
         */
        bool begin();
        /**
         * Inicia el servicio mDNS.
         */
        bool beginmDNS();
        /**
         * Devuelve un booleano indicando si el equipo esta conectado,
         * devuelve true tanto si la conexion es AP como STA.
         */
        bool isConnected();
        /**
         * Imprime la informacion de la conexion por el Serial.
         */
        void printWifiInfo();
        /**
         * Devuelve un entero que corresponde con el modo configurado 
         * en el equipo (AP, STA).
         */
        int getMode();
        /**
         * Valor con la potencia de la señal Wifi a la que se+
         * esta conectado. Solamente valido para conexiones STA.
         */
        int8_t RSSI();
        /**
         * Guarda el ssid en memoria.
         */
        bool saveSTASSID(String ssid);
        /**
         * Lee el ssid almacenado en memoria.
         */
        String readSTASSID();
        /**
         * Guarda el password en memoria.
         */
        bool saveSTAPass(String pwd);
        /**
         * Lee el password almacenado en memoria.
         */
        String readSTAPass();
        /**
         * Guarda el estado habilitado o no en memoria.
         */
        bool saveStatus();
        /**
         * Guarda las opciones del servicio mDNS en memoria.
         */
        bool saveMDNSSettings();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern DomDomWifiClass DomDomWifi;
#endif

#endif /* DOMDOM_WIFICLASS_h */