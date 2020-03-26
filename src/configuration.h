
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
//============================ STATUS LED CONTROL  ==========================
//===========================================================================

#define LED_STATUS_RESOLUTION 10
#define LED_STATUS_CHANNEL 10
#define LED_STATUS_BLINK_DELAY 100

#define LED_RED_PIN     5
#define LED_GREEN_PIN   18
#define LED_BLUE_PIN    19

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

#define CHANNEL_SIZE            3
#define CHANNEL_PWM_PINS        {25, 26, 27}
#define CHANNEL_RESOLUTIONS     {10, 10, 10}

//===========================================================================
//===================== RTC Y  NTP SECTION ==================================
//===========================================================================

#define RTC_BEGIN_ATTEMPS 3
#define RTC_BEGIN_ATTEMPS_DELAY 200

#define NTP_ENABLED 1
#define NTP_SERVERNAME "pool.ntp.org"
#define NTP_TIMEZONEOFFSET 3600
#define NTP_DELAY_ON_FAILURE    10000
#define NTP_DELAY_ON_SUCCESS    3600000

//===========================================================================
//============================ FAN SECTION ==================================
//===========================================================================

#define FAN_TEMP_SENSOR_PIN             33
#define FAN_PWM_PIN                     5
#define FAN_PWM_FEEDBACK_PIN            34
#define FAN_PWM_RESOLUTION              10
#define FAN_PWM_CHANNEL                 11
#define FAN_MIN_TEMP                    30
#define FAN_MAX_TEMP                    40
#define FAN_HISTERESIS                  2
#define FAN_FEEDBACK_R1                 6800.0f
#define FAN_FEEDBACK_R2                 4700.0f
#define FAN_FEEDBACK_OFFSET             0.31f

//===========================================================================
//============================ EEPROM SECTION ===============================
//===========================================================================


#define EEPROM_SIZE                             2048
#define EEPROM_INIT_RETRIES                     10
#define EEPROM_STA_ENABLED_ADDRESS              4
#define EEPROM_STA_SSID_NAME_ADDRESS            EEPROM_STA_ENABLED_ADDRESS + 1
#define EEPROM_SSID_NAME_LENGTH                 32
#define EEPROM_STA_PASSWORD_ADDRESS             EEPROM_STA_SSID_NAME_ADDRESS + EEPROM_SSID_NAME_LENGTH
#define EEPROM_STA_PASSWORD_LENGTH              64
#define EEPROM_CHANNEL_MEMORY_SIZE              8
#define EEPROM_CHANNEL_FIRST_ADDRESS            EEPROM_STA_PASSWORD_ADDRESS +  EEPROM_STA_PASSWORD_LENGTH
#define EEPROM_SCHEDULE_STATUS_ADDRESS          1 + EEPROM_CHANNEL_FIRST_ADDRESS * EEPROM_CHANNEL_MEMORY_SIZE
#define EEPROM_SCHEDULE_FIRST_ADDRESS           EEPROM_SCHEDULE_STATUS_ADDRESS + 1
#define EEPROM_MAX_SCHEDULE_POINTS              50
#define EEPROM_NTP_ENABLED_ADDRESS              EEPROM_SCHEDULE_FIRST_ADDRESS + EEPROM_MAX_SCHEDULE_POINTS * 9
#define EEPROM_MDNS_ENABLED_ADDRESS             EEPROM_NTP_ENABLED_ADDRESS + 1
#define EEPROM_MDNS_HOSTNAME_ADDRESS            EEPROM_MDNS_ENABLED_ADDRESS + 1
#define EEPROM_MDNS_HOSTNAME_LENGTH             32
#define EEPROM_FAN_ENABLED_ADDRESS              EEPROM_MDNS_HOSTNAME_ADDRESS + EEPROM_MDNS_HOSTNAME_LENGTH

#endif /* GLOBAL_CONFIGURACION_h */