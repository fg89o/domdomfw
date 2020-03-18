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

#include "wifi.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include "configuration.h"

DomDomWifiClass::DomDomWifiClass()
{
    _connected = false;
};

bool DomDomWifiClass::begin()
{
    Serial.println("Iniciando WiFi");

    sta_enabled = WIFI_STA_ENABLED;
    int value = EEPROM.read(EEPROM_STA_ENABLED_ADDRESS);
    if (value >= 0 && value <= 255)
    {
        sta_enabled = value > 0;
    }

    ssid = DomDomWifi.readSTASSID();
    pwd = DomDomWifi.readSTAPass();
    if (!(ssid.length() > 0 && ssid.length() < EEPROM_SSID_NAME_LENGTH))
    {
        Serial.println("...Usando valores wifi por defecto...");
        ssid = WIFI_STA_SSID_NAME;
        pwd = WIFI_STA_PASSWORD;
    }

    mDNS_enabled = EEPROM.read(EEPROM_MDNS_ENABLED_ADDRESS);
    mDNS_hostname = EEPROM.readString(EEPROM_MDNS_HOSTNAME_ADDRESS);

    connect();

    return true;
}

// Inicia la tarea del proceso de conexion
void DomDomWifiClass::connect()
{
    if (sta_enabled)
    {
        // Si tenemos red WIFI a la que conectarnos lo intentamos
        if (ssid.length() > 0)
        {
            Serial.printf("Intentando conexion Wifi a %s\n", ssid.c_str());
            for(int i = 0; i < WIFI_NUM_RETRIES; i++)
            {
                connectSTAWifi();
                int delay_ms = 0;
                while(WiFi.status() != WL_CONNECTED && delay_ms < WIFI_CONNECTION_LATENCY)
                {
                    Serial.print('_');
                    delay_ms += 500;
                    delay(delay_ms);
                }
                
                _connected = WiFi.status() == WL_CONNECTED;

                if (!_connected)
                {
                    Serial.print('.');
                    delay(1000);
                }else{
                    Serial.print("\n");
                    printWifiInfo();
                    break;
                }
            }

            if (!_connected)
            {
                Serial.println("Agotados intentos de conexion!");
            }
        }
        
    }
    
    // Si no nos hemos conectado creamos el punto de acceso
    if (!_connected)
    {
        Serial.print("Creando AP");
        for(int i = 0; i < WIFI_NUM_RETRIES; i++)
        {
            _connected = createOwnAPWifi();

            if (_connected)
            {
                Serial.print("\n");
                printWifiInfo();
                break;
            }
            
            Serial.print('.');
            delay(2000);
        }
    }

    if (!_connected)
    {
        Serial.println("No se pudo crear el AP propio del dispositivo. Equipo sin conexion");
    }

    if (_connected && mDNS_enabled)
    {
        Serial.println("Configurando mDNS");
        beginmDNS();
    }
}

int DomDomWifiClass::connectSTAWifi()
{
    WiFi.mode(WIFI_STA);

    return WiFi.begin(ssid.c_str(), pwd.c_str());
}

bool DomDomWifiClass::createOwnAPWifi()
{
    WiFi.mode(WIFI_AP);

    return WiFi.softAP(
        WIFI_AP_SSID_NAME, 
        WIFI_AP_PASSWORD, 
        WIFI_AP_CHANNEL,
        WIFI_AP_HIDDEN,
        WIFI_AP_MAX_CONNECTIONS
    );
}

bool DomDomWifiClass::beginmDNS()
{

    mDNS_hostname = EEPROM.readString(EEPROM_MDNS_HOSTNAME_ADDRESS);
    if (mDNS_hostname.length() <= 0)
    {
        mDNS_hostname = MDNS_HOSTNAME;
    }

    if (!MDNS.begin(mDNS_hostname.c_str())) {
        Serial.println("Error al configurar el servicio MDNS!");
        return false;
    }

    Serial.printf("Configurado mDNS con hostname %s.local\n", mDNS_hostname.c_str());
    return true;
}

bool DomDomWifiClass::isConnected(){
    return _connected;
}

int DomDomWifiClass::getMode()
{
    return WiFi.getMode();
}

int8_t DomDomWifiClass::RSSI()
{
    return WiFi.RSSI();
}

void DomDomWifiClass::printWifiInfo()
{
    String modo;
    switch(WiFi.getMode())
    {
        case WIFI_MODE_AP:
            modo = "AP";
            break;
        case WIFI_MODE_STA:
            modo = "STA";
            break;
        case WIFI_MODE_APSTA:
            modo = "STA+AP";
            break;
        case WIFI_MODE_NULL:
            modo = "No inicializado";
            break;
        case WIFI_MODE_MAX:
            modo = "MAX";
            break;
    };

    Serial.println("=== Wifi Info ===");
    
    if (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA)
    {
        Serial.print("· Modo: AP\n");
        Serial.printf("· SSID: %s\n", WIFI_AP_SSID_NAME);
        Serial.printf("· IP Local: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("· Network ID: %s\n",WiFi.softAPNetworkID().toString().c_str());
        Serial.printf("· MAC: %s\n", WiFi.softAPmacAddress().c_str());
    }
    
    if (WiFi.getMode() != WIFI_MODE_AP)
    {
        Serial.printf("· Modo: STA\n");
        Serial.printf("· Estado: %s\n", (char *)(WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado"));
        Serial.printf("· SSID: %s\n", WIFI_STA_SSID_NAME);
        Serial.printf("· IP Local: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("· Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("· DNS: %s\n", WiFi.dnsIP().toString().c_str());
    }
}

bool DomDomWifiClass::saveSTASSID(String str)
{
    if (strlen(str.c_str()) > EEPROM_SSID_NAME_LENGTH)
    {
        Serial.println("ERROR: SSID name too long!");
        return false;
    }
    
    EEPROM.writeString(EEPROM_STA_SSID_NAME_ADDRESS, str);
    bool result = EEPROM.commit();
    if (!result)
    {
        Serial.println("Error al guardar en la EEPROM!!");
    }
    return result;
}

String DomDomWifiClass::readSTASSID()
{
    return EEPROM.readString(EEPROM_STA_SSID_NAME_ADDRESS);
}

bool DomDomWifiClass::saveSTAPass(String password)
{
    if (strlen(password.c_str()) > EEPROM_STA_PASSWORD_LENGTH)
    {
        Serial.println("ERROR: Password too long!");
        return false;
    }

    EEPROM.writeString(EEPROM_STA_PASSWORD_ADDRESS, password);
    bool result = EEPROM.commit();
    if (!result)
    {
        Serial.println("Error al guardar en la EEPROM!!");
    }
    return result;
}

String DomDomWifiClass::readSTAPass()
{
    return EEPROM.readString(EEPROM_STA_PASSWORD_ADDRESS);
}

bool DomDomWifiClass::saveStatus()
{
    EEPROM.write(EEPROM_STA_ENABLED_ADDRESS, sta_enabled);
    return EEPROM.commit();
}

bool DomDomWifiClass::saveMDNSSettings()
{
    EEPROM.writeBool(EEPROM_MDNS_ENABLED_ADDRESS, mDNS_enabled);
    EEPROM.writeString(EEPROM_MDNS_HOSTNAME_ADDRESS, mDNS_hostname);
    return EEPROM.commit();
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomWifiClass DomDomWifi;
#endif