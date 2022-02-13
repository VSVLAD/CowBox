#pragma once

#include "main.h"
#include <avr/eeprom.h>


#define EEPROM_MAGIC_VALUE_FIRST_RUN    0x3F
#define EEPROM_ADDR_SETTING_FIRST_RUN   0x0
#define EEPROM_ADDR_SETTING_ATTEMPT     EEPROM_ADDR_SETTING_FIRST_RUN + 1
#define EEPROM_ADDR_SETTING_CONTRAST    EEPROM_ADDR_SETTING_ATTEMPT   + 1
#define EEPROM_ADDR_SETTING_FLIP        EEPROM_ADDR_SETTING_CONTRAST  + 1
#define EEPROM_ADDR_SETTING_INVERT      EEPROM_ADDR_SETTING_FLIP      + 1
#define EEPROM_ADDR_SETTING_LIGHT       EEPROM_ADDR_SETTING_INVERT    + 1

// Задать количество попыток PIN
#define SetSettingAttempt(var)  __EEPUT(EEPROM_ADDR_SETTING_ATTEMPT, var)

// Получить количество попыток PIN
#define GetSettingAttempt(var)  __EEGET(var, EEPROM_ADDR_SETTING_ATTEMPT);

// Глобаные настройки
extern uint8_t SettingDisplayContrast;
extern uint8_t SettingDisplayLight;
extern bool SettingDisplayFlip;
extern bool SettingDisplayInvert;

// Сохранение и загрузка настроек
void SaveSettings();
void LoadSettings();

// Проверка на первый запуск и смена состояния
bool IsFirstRun();