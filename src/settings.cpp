#include "settings.h"

// Глобальные переменные для настроек
uint8_t SettingDisplayContrast = 75;
uint8_t SettingDisplayLight = 120;
bool SettingDisplayFlip = false;
bool SettingDisplayInvert = false;

// Сохранение настроек
void SaveSettings() {
    __EEPUT(EEPROM_ADDR_SETTING_CONTRAST, SettingDisplayContrast);
    __EEPUT(EEPROM_ADDR_SETTING_FLIP, SettingDisplayFlip);
    __EEPUT(EEPROM_ADDR_SETTING_INVERT, SettingDisplayInvert);
    __EEPUT(EEPROM_ADDR_SETTING_LIGHT, SettingDisplayLight);
}

// Загрузка настроек
void LoadSettings() {
    _EEGET(SettingDisplayContrast, EEPROM_ADDR_SETTING_CONTRAST);
    _EEGET(SettingDisplayFlip, EEPROM_ADDR_SETTING_FLIP);
    _EEGET(SettingDisplayInvert, EEPROM_ADDR_SETTING_INVERT);
    _EEGET(SettingDisplayLight, EEPROM_ADDR_SETTING_LIGHT);
}

// Загрузка настроек
bool IsFirstRun() {
    uint8_t bt;
    __EEGET(bt, EEPROM_ADDR_SETTING_FIRST_RUN);
    bool flagInit = bt != EEPROM_MAGIC_VALUE_FIRST_RUN;

    // Если прочитали не спец-значение, значит запустились впервые и изменяем значение
    if (flagInit)
        __EEPUT(EEPROM_ADDR_SETTING_FIRST_RUN, EEPROM_MAGIC_VALUE_FIRST_RUN);
    
    return flagInit;
}
