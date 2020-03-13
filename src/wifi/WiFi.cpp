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

// Constructor
DomDomWifiClass::DomDomWifiClass(){};

// Inicia le proceso de conexcion WIFI.
// Si hay un SSID al que conectarse lo intenta. 
// En caso contrario o de error crea su propio AP para tener conexion.
// Al final inicia el servicio mDNS.
bool DomDomWifiClass::begin()
{
    Serial.println("Iniciando WiFi");
    DomDomWifi.isConnecting = true;

    enabled = WIFI_STA_ENABLED;
    int value = EEPROM.read(EEPROM_STA_ENABLED_ADDRESS);
    if (value >= 0 && value <= 255)
    {
        enabled = value > 0;
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

    xTaskCreate(
        this->tInit,            /* Task function. */
        "WIFIInitTask",         /* String with name of task. */
        10000,                  /* Stack size in bytes. */
        NULL,                   /* Parameter passed as input of the task */
        1,                      /* Priority of the task. */
        initTaskHandle          /* Task handle. */
    );
    
    while(DomDomWifi.isConnecting)
    {
        delay(100);
    }

    return true;
}

// Inicia la tarea del proceso de conexion
void DomDomWifiClass::tInit(void * parameter)
{
    DomDomWifi.isConnecting = true;

    bool connected = false;
    if (DomDomWifi.enabled)
    {
        // Si tenemos red WIFI a la que conectarnos lo intentamos
        if (DomDomWifi.ssid.length() > 0)
        {
            Serial.printf("Intentando conexion Wifi a %s\n", DomDomWifi.ssid.c_str());
            for(int i = 0; i < WIFI_NUM_RETRIES; i++)
            {
                connectSTAWifi();
                int delay = 0;
                while(WiFi.status() != WL_CONNECTED && delay < WIFI_CONNECTION_LATENCY)
                {
                    Serial.print('_');
                    delay += 500;
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                
                connected = WiFi.status() == WL_CONNECTED;

                if (!connected)
                {
                    Serial.print('.');
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }else{
                    Serial.print("\n");
                    DomDomWifi.printWifiInfo();
                    break;
                }
            }

            if (!connected)
            {
                Serial.println("Agotados intentos de conexion!");
            }
        }
        
    }
    
    // Si no nos hemos conectado creamos el punto de acceso
    if (!connected)
    {
        Serial.print("Creando AP");
        for(int i = 0; i < WIFI_NUM_RETRIES; i++)
        {
            connected = createOwnAPWifi();

            if (connected)
            {
                Serial.print("\n");
                DomDomWifi.printWifiInfo();
                break;
            }
            
            Serial.print('.');
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }

    if (!connected)
    {
        Serial.println("No se pudo crear el AP propio del dispositivo. Equipo sin conexion");
    }

    if (connected && DomDomWifi.mDNS_enabled)
    {
        Serial.println("Configurando mDNS");
        initmDNS();
    }

    DomDomWifi.isConnecting = false;

    vTaskDelete(NULL);
}

// Intenta conectarse a un SSID
int DomDomWifiClass::connectSTAWifi()
{
    WiFi.mode(WIFI_STA);

    return WiFi.begin(DomDomWifi.ssid.c_str(), DomDomWifi.pwd.c_str());
}

// Crea un AP propio con la conexion
bool DomDomWifiClass::createOwnAPWifi(){
    WiFi.mode(WIFI_AP);

    return WiFi.softAP(
        WIFI_AP_SSID_NAME, 
        WIFI_AP_PASSWORD, 
        WIFI_AP_CHANNEL,
        WIFI_AP_HIDDEN,
        WIFI_AP_MAX_CONNECTIONS
    );
}

// Inicializa el servicio mDNS
bool DomDomWifiClass::initmDNS()
{

    DomDomWifi.mDNS_hostname = EEPROM.readString(EEPROM_MDNS_HOSTNAME_ADDRESS);
    if (DomDomWifi.mDNS_hostname.length() <= 0)
    {
        DomDomWifi.mDNS_hostname = MDNS_HOSTNAME;
    }

    if (!MDNS.begin(DomDomWifi.mDNS_hostname.c_str())) {
        Serial.println("Error al configurar el servicio MDNS!");
        return false;
    }

    Serial.printf("Configurado mDNS con hostname %s.local\n", DomDomWifi.mDNS_hostname.c_str());
    return true;
}

// Devuelve si se esta conectada a una red WiFi
bool DomDomWifiClass::isConnected(){
    return WiFi.isConnected();
}

int DomDomWifiClass::getMode()
{
    return WiFi.getMode();
}

// Imprime los detalles de la red wifi a la que se esta conectado
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

// Guarda el SSID para STA en la EEPROM
bool DomDomWifiClass::saveSTASSID(const char* ssid)
{
    if (strlen(ssid) > EEPROM_SSID_NAME_LENGTH)
    {
        Serial.println("ERROR: SSID name too long!");
        return false;
    }
    
    EEPROM.writeString(EEPROM_STA_SSID_NAME_ADDRESS, ssid);
    bool result = EEPROM.commit();
    if (!result)
    {
        Serial.println("Error al guardar en la EEPROM!!");
    }
    return result;
}

// Recupera el SSID para STA de la EEPROM
String DomDomWifiClass::readSTASSID()
{
    return EEPROM.readString(EEPROM_STA_SSID_NAME_ADDRESS);
}

// Guarda el SSID para STA en la EEPROM
bool DomDomWifiClass::saveSTAPass(const char* pass)
{
    if (strlen(pass) > EEPROM_STA_PASSWORD_LENGTH)
    {
        Serial.println("ERROR: Password too long!");
        return false;
    }

    EEPROM.writeString(EEPROM_STA_PASSWORD_ADDRESS, pass);
    bool result = EEPROM.commit();
    if (!result)
    {
        Serial.println("Error al guardar en la EEPROM!!");
    }
    return result;
}

// Recupera el SSID para STA de la EEPROM
String DomDomWifiClass::readSTAPass()
{
    return EEPROM.readString(EEPROM_STA_PASSWORD_ADDRESS);
}

bool DomDomWifiClass::saveStatus()
{
    EEPROM.write(EEPROM_STA_ENABLED_ADDRESS, enabled);
    return EEPROM.commit();
}

#if !defined(NO_GLOBAL_INSTANCES)
DomDomWifiClass DomDomWifi;
#endif