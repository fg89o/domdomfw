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

#include "channel.h"
#include <EEPROM.h>
#include "configuration.h"

// Constructor
DomDomChannelClass::DomDomChannelClass(char num)
{
    _ready = false;
    _channel_num = num;
    _enabled = true;
}

// Constructor
DomDomChannelClass::DomDomChannelClass(char num, char pwm_pin, char resolution)
{
    _ready = false;
    _enabled = true;
    _channel_num = num;
    _pwm_pin = pwm_pin;
    _current_pwm = 0;

    setPWMResolution(resolution);
    max_limit_pwm = _max_pwn;
    min_limit_pwm = _min_pwm;

    pinMode(_pwm_pin, OUTPUT);
}

// Incia el control del canal
bool DomDomChannelClass::begin()
{
    if (_pwm_pin == 0 || !_enabled)
    {
        return false;
    }

    // configure LED PWM functionalitites
    ledcSetup(_channel_num, 5000, _resolution);
    
    // attach the channel to be controlled
    ledcAttachPin(_pwm_pin, _channel_num);

    // init PWM
    ledcWrite(_channel_num, _current_pwm); 

    _ready = true;
    return true;
}

bool DomDomChannelClass::end()
{
    _ready = false;
    ledcDetachPin(_pwm_pin);

    return true;
}

bool DomDomChannelClass::setEnabled(bool enabled)
{
    if (_enabled != enabled)
    {
        _enabled = enabled;

        if (_ready && !enabled)
        {
            end();
        }
    }

    return true;
}

// Cambia el valor PWM del canal
bool DomDomChannelClass::setPWMValue(uint16_t value)
{
    if (!_ready)
    {
        return false;
    }

    if (value > max_limit_pwm)
    {
        value =  max_limit_pwm;
        Serial.printf("ERROR: valor pwm mayor que el maximo. Valor cambiado a %d\n", value);
    }

    if (value < min_limit_pwm)
    {
        value = min_limit_pwm;
        Serial.printf("ERROR: valor pwm menor que el minimo. Valor cambiado a %d\n", value);
    }

    ledcWrite(_channel_num, value);

    _current_pwm = value;

    return true;
}

// Establece la resolucion del PWM
bool DomDomChannelClass::setPWMResolution(uint8_t value)
{
    if (value != 8 && value != 10 && value != 12 && value != 15)
    {
        Serial.printf("ERROR: El valor de resolucion no se encuentra entre los permitidos (%d)\n",value);
        return false;
    }

    _resolution = value;
    _min_pwm = 0;
    _max_pwn = (int)pow(2,value);

    return true;
}

// Actualiza el valor de los PWM en funcion de los puntos programados
void DomDomChannelClass::update()
{

    if (!_ready)
    {
        Serial.printf("[Channel %d] WARN: Channel %d not ready. Use begin().\n", _channel_num, _channel_num);
        return;
    }

    DateTime now = DomDomRTC.now();
    Serial.printf("[Channel %d] %d:%d Comprobando programacion\n", _channel_num, now.hour(), now.minute());

    DateTime horaAnterior;
    DateTime horaSiguiente;
    DomDomSchedulePoint *puntoAnterior = nullptr;
    DomDomSchedulePoint *puntoSiguiente = nullptr;

    bool correct;
    correct = DomDomScheduleMgt.getShedulePoint(horaAnterior, puntoAnterior, true);
    if (!correct)
    {
        Serial.println("ERROR: No se encontra una programacion previa.");
        return;
    }

    Serial.printf("[Channel %d] programacion previa obtenida: %d:%d\n", _channel_num, puntoAnterior->hour, puntoAnterior->minute);

    correct = DomDomScheduleMgt.getShedulePoint(horaSiguiente, puntoSiguiente, false);
    if (!correct)
    {
        Serial.println("ERROR: No se encontra una programacion siguiente.");
        return;
    }

    Serial.printf("[Channel %d] programacion posterior obtenida: %d:%d\n", _channel_num, puntoSiguiente->hour, puntoSiguiente->minute);

    if(!puntoSiguiente->fade)
    {
        Serial.printf("[Channel %d] Programacion no lineal\n", _channel_num);
        calcPWMNoLineal(puntoAnterior, puntoSiguiente, horaAnterior, horaSiguiente);
    }else{
        Serial.printf("[Channel %d] Programacion lineal\n", _channel_num);
        calcPWMLineal(puntoAnterior, puntoSiguiente, horaAnterior, horaSiguiente);
    }
    
}

void DomDomChannelClass::calcPWMLineal(DomDomSchedulePoint *puntoAnterior, DomDomSchedulePoint *puntoSiguiente, DateTime &horaAnterior, DateTime &horaSiguiente)
{

    int minutes_total = (horaSiguiente - horaAnterior).totalseconds() / 60;

    // Si la hora es la misma ajustamos directamente el valor.
    if (minutes_total == 0)
    {
        Serial.println("WARN: Misma hora de inicio y de final. ¿Error en programacion?");
        int new_pwm = 0;
        double porcentaje = puntoSiguiente->value[_channel_num] / 100;
        new_pwm = min_limit_pwm + ((max_limit_pwm - min_limit_pwm) * porcentaje);
        if (new_pwm != _current_pwm)
        {
            Serial.printf("[Channel %d] Cambiando PWM a %d (%d)\n", _channel_num, puntoSiguiente->value[_channel_num], new_pwm);
            setPWMValue(new_pwm);
        }
    }
    else
    {
        DateTime now = DomDomRTC.now();
        DateTime now_noseconds (now.year(),now.month(),now.day(),now.hour(), now.minute(), 0);

        int minutes = (now_noseconds - horaAnterior).totalseconds() / 60;
        Serial.printf("Minutos totales: %d | Minutos transcurridos: %d\n", minutes_total, minutes);

        double minutes_porcentaje = (minutes * 100) / minutes_total / double(100);
        Serial.printf("Porcentaje de tiempo: %f\n", minutes_porcentaje);

        int porcentaje_valor = (puntoSiguiente->value[_channel_num] - puntoAnterior->value[_channel_num]) * minutes_porcentaje;
        Serial.printf("Punto anterior: %d | Punto siguiente: %d | Porcentaje a aplicar: %d\n", puntoAnterior->value[_channel_num], puntoSiguiente->value[_channel_num] , porcentaje_valor);

        int new_pwm = min_limit_pwm + (max_limit_pwm - min_limit_pwm) * ((puntoAnterior->value[_channel_num] + porcentaje_valor) / double(100));
        Serial.printf("PWM: %d\n", new_pwm);
        if (new_pwm != _current_pwm)
        {
            Serial.printf("[Channel %d] Cambiando PWM a %d (%d)\n", _channel_num, puntoAnterior->value[_channel_num] + porcentaje_valor, new_pwm);
            setPWMValue(new_pwm);
        }
    }   
}

void DomDomChannelClass::calcPWMNoLineal(DomDomSchedulePoint *puntoAnterior, DomDomSchedulePoint *puntoSiguiente, DateTime &horaAnterior, DateTime &horaSiguiente)
{
    int new_pwm = 0;
    double porcentaje = puntoAnterior->value[_channel_num] / 100;
    new_pwm = min_limit_pwm + ((max_limit_pwm - min_limit_pwm) * porcentaje);
    if (new_pwm != _current_pwm)
    {
        Serial.printf("[Channel %d] Cambiando PWM a %d (%d)\n", _channel_num, puntoAnterior->value[_channel_num], new_pwm);
        setPWMValue(new_pwm);
    }
}

bool DomDomChannelClass::saveCurrentPWM()
{
    int address = EEPROM_CHANNEL_FIRST_ADDRESS ;
    address += _channel_num  * EEPROM_CHANNEL_MEMORY_SIZE;
    address += 6;

    EEPROM.writeUShort(address, _current_pwm);
    return EEPROM.commit();
}

bool DomDomChannelClass::save()
{
    int address = EEPROM_CHANNEL_FIRST_ADDRESS ;
    address += _channel_num * EEPROM_CHANNEL_MEMORY_SIZE;

    EEPROM.write(address, (_channel_num+1));
    address += 1;
    EEPROM.writeBool(address, _enabled);
    address += 1;
    EEPROM.writeUShort(address, max_limit_pwm);
    address += 2;
    EEPROM.writeUShort(address, min_limit_pwm);
    address += 2;
    saveCurrentPWM();
    
    bool result = EEPROM.commit();

    if (result)
    {
        Serial.printf("[Channel %d] Guardado en EEPROM\n", _channel_num);
    }

    return result;
};

bool DomDomChannelClass::loadFromEEPROM()
{
    int address = EEPROM_CHANNEL_FIRST_ADDRESS ;
    address += _channel_num * EEPROM_CHANNEL_MEMORY_SIZE;

    if (EEPROM.read(address) == (_channel_num+1))
    {
        address += 1;
        setEnabled(EEPROM.readBool(address));
        address += 1;
        max_limit_pwm = EEPROM.readUShort(address);
        address += 2;
        min_limit_pwm = EEPROM.readUShort(address);
        address += 2;
        uint16_t pwm = EEPROM.readUShort(address);
        setPWMValue(pwm);
        
        Serial.printf("[CHANNEL %d] Configuracion cargada correctamente.\n", _channel_num);
        
    } else {
        Serial.printf("[CHANNEL %d] No se encontro informacion guardada del canal\n", _channel_num);
        save();
        return false;
    }

    return true;
}

void DomDomChannelClass::initEEPROM()
{
    int address = EEPROM_CHANNEL_FIRST_ADDRESS ;
    address += _channel_num * EEPROM_CHANNEL_MEMORY_SIZE;

    EEPROM.write(address,0);
    address += 1;
    EEPROM.write(address,0);
    address += 1;
    EEPROM.writeUShort(address,0);
    address += 2;
    EEPROM.writeUShort(address,0);
    address += 2;
    EEPROM.writeUShort(address,0);
}