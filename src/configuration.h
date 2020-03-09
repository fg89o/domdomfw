
#pragma once

#ifndef DOMDOM_GLOBAL_CONFIGURACION_h
#define DOMDOM_GLOBAL_CONFIGURACION_h

#define DEBUG

//===========================================================================
//============================ Serial Settings  =============================
//===========================================================================
// Define la velocidad de comunicación del puerto COM
#define BAUDRATE 9600

//===========================================================================
//============================ WIFI Settings  ===============================
//===========================================================================

// Define si se intentara conectar a una red wifi
#define WIFI_STA_ENABLED 1

// Define la red wifi a la que intetara conectase por defecto
#define WIFI_STA_SSID_NAME "MIWIFI_2G_QQPJ"

// Define el password de la red wifi por defecto
#define WIFI_STA_PASSWORD "aFG6nxrr"

// Nombre del AP que se creara para conectarse al equipo (32 digitos max.)
#define WIFI_AP_SSID_NAME "DomDom"

// Contraseña para el AP (entre 8 y 63 max.)
#define WIFI_AP_PASSWORD ""

// Numero maximo de conexiones al punto de acceso ( 8 max. )
#define WIFI_AP_MAX_CONNECTIONS 4

// Canal (entre 1 y 13)
#define WIFI_AP_CHANNEL 6

// Estado del ssid
#define WIFI_AP_HIDDEN 0

// Numero de intentos para conectar el wifi
#define WIFI_NUM_RETRIES 3

// Tiempo de espera entre intentos
#define WIFI_CONNECTION_LATENCY 10000

//===========================================================================
//============================ mDNS SECTION =================================
//===========================================================================

#define MDNS_ENABLED 1

// Nombre mDNS para localizar el equipo
#define MDNS_HOSTNAME "DomDom"

//===========================================================================
//============================ CHANNELS SECTION =============================
//===========================================================================

#define CHANNEL_1_PWM_PIN       18  // AZUL
#define CHANNEL_2_PWM_PIN       17  // VERDE
#define CHANNEL_3_PWM_PIN       16  // ROJO
#define CHANNEL_4_PWM_PIN       0
#define CHANNEL_5_PWM_PIN       0

#define CHANNEL_1_RESOLUTION    8
#define CHANNEL_2_RESOLUTION    8
#define CHANNEL_3_RESOLUTION    8
#define CHANNEL_4_RESOLUTION    8
#define CHANNEL_5_RESOLUTION    8

//===========================================================================
//============================ NTP SECTION ==================================
//===========================================================================

#define NTP_ENABLED 1
#define NTP_SERVERNAME "pool.ntp.org"
#define NTP_TIMEZONEOFFSET 3600

//===========================================================================
//============================ FAN SECTION ==================================
//===========================================================================

#define FAN_TEMPERATURE_SENSOR_INPUT 1
#define FAN_PWM_PIN 5
#define FAN_PWM_FEEDBACK_PIN 4

//===========================================================================
//============================ EEPROM SECTION ===============================
//===========================================================================

#define EEPROM_SIZE                             1024
#define EEPROM_INIT_RETRIES                     10
#define EEPROM_STA_ENABLED_ADDRESS              4
#define EEPROM_STA_SSID_NAME_ADDRESS            5
#define EEPROM_SSID_NAME_LENGTH                 32
#define EEPROM_STA_PASSWORD_ADDRESS             38
#define EEPROM_STA_PASSWORD_LENGTH              64
#define EEPROM_CHANNEL_FIRST_ADDRESS            103      // 5 canales * 7 bytes
#define EEPROM_SCHEDULE_FIRST_ADDRESS           139      // 1 byte por la cantidad + 50 puntos * 9 byte cada punto = 271
#define EEPROM_MAX_SCHEDULE_POINTS              50
#define EEPROM_NTP_ENABLED_ADDRESS              591
#define EEPROM_MDNS_ENABLED                     592
#define EEPROM_MDNS_HOSTNAME_ADDRESS            593
#define EEPROM_MDNS_HOSTNAME_LENGTH             32

#endif /* GLOBAL_CONFIGURACION_h */