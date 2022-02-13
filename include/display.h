#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include "GyverOLED.h"

// Прочитать строку из PROGMEM
#define GetStringPgm(sourcePrg, buffer) strcpy_P(buffer, sourcePrg)

#ifdef MORE_MENU_ITEMS
	#define UINTX uint16_t
	#define MENU_ROOT_ID UINT16_MAX
#else
	#define UINTX uint8_t
	#define MENU_ROOT_ID UINT8_MAX
#endif

#define MAX_TITLE            50
#define MAX_ACCOUNT_LOGIN    30
#define MAX_ACCOUNT_PASSWORD 30

#define MENU_OPTIONS_ID             MENU_ROOT_ID - 1
#define MENU_OPTIONS_CONTRAST_ID    MENU_ROOT_ID - 2
#define MENU_OPTIONS_INVERT_ID      MENU_ROOT_ID - 3
#define MENU_OPTIONS_FLIP_ID        MENU_ROOT_ID - 4
#define MENU_OPTIONS_LIGHT_ID       MENU_ROOT_ID - 5
#define MENU_HELP_ID                MENU_ROOT_ID - 6
#define MENU_LOGO_ID                MENU_ROOT_ID - 7
#define MENU_OPTIONS_RESET_ID       MENU_ROOT_ID - 8

#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  32
#define DISPLAY_COLS    21
#define DISPLAY_ROWS    4

#define BMP_SIZE             7
#define BMP_ICONS_FOLDER     0
#define BMP_ICONS_FILE       1
#define BMP_ICONS_CAT        2
#define BMP_ICONS_ARROW_UP   3
#define BMP_ICONS_ARROW_DOWN 4
#define BMP_ICONS_ARROW_LEFT 5
#define BMP_ICONS_TOOL       6
#define BMP_ICONS_HELP       7
#define BMP_ICONS_WARNING    8

#define OFFSET_X_MENU_ITEM_TEXT 12
#define OFFSET_X_MENU_ITEM_ICON 1

// Режим клавиатурного ввода
enum ScenarioMode {
    SCENARIO_MODE_PASSWORD,
    SCENARIO_MODE_PASSWORD_ENTER,
    SCENARIO_MODE_LOGIN_TAB_PASSWORD,
    SCENARIO_MODE_LOGIN_TAB_PASSWORD_ENTER,
    SCENARIO_MODE_MANUAL,
    SCENARIO_MODE_CUSTOM
};

// Тип элемента в меню. В зависимости от типа рисуется свой значок и анализируются доступные действия
enum MenuType {
    MENU_TYPE_FOLDER,
    MENU_TYPE_NOTE,
    MENU_TYPE_ROOT,
    MENU_TYPE_HELP,
	MENU_TYPE_LOGO,
    MENU_TYPE_AUTH,
    MENU_TYPE_OPTIONS,
	MENU_TYPE_OPTIONS_CONTRAST,
	MENU_TYPE_OPTIONS_FLIP,
	MENU_TYPE_OPTIONS_INVERT,
    MENU_TYPE_OPTIONS_LIGHT,
    MENU_TYPE_OPTIONS_RESET
};

// Структура для пункта меню PGM
struct MenuItemPGM {
    
    // Основные
    MenuType type;
    UINTX id;
    UINTX parentId;
    PGM_P title;

    // Данные по аккаунту
    PGM_P login;
    PGM_P password;
    ScenarioMode mode;
};

// Структура для пункта меню RAM
struct MenuItem {

    // Основные
    MenuType type;
    UINTX id;
    UINTX parentId;
    char title[MAX_TITLE];

    // Данные по аккаунту
    char login[MAX_ACCOUNT_LOGIN];
    char password[MAX_ACCOUNT_PASSWORD];
    ScenarioMode mode;
};

// Выбранный ID текущий пункт меню открытый на экране  (корень, папка, файл)
extern UINTX SelectedMenuItemId;

// Активный ID пункт меню сейчас под курсором
extern UINTX ActiveMenuItemId;

// Текущий режим отображения
extern MenuType ViewMode;

// Параметры курсоров
extern int GlobalCursorMin;
extern int GlobalCursorMax;
extern int GlobalCursorY;

extern int LocalCursorMin;
extern int LocalCursorMax;
extern int LocalCursorY;

// Массив с значками 7х7
const uint8_t bmp_icons_pgm[][7] PROGMEM = {
    {0x7f, 0x41, 0x41, 0x42, 0x42, 0x42, 0x7c},  // Папка
    {0x1c, 0x22, 0x1c, 0x08, 0x18, 0x08, 0x38},  // Файл
    {0x3f, 0x42, 0x54, 0x44, 0x54, 0x42, 0x3f},  // Кот
    {0x10, 0x18, 0x1c, 0x1e, 0x1c, 0x18, 0x10},  // Стрелка вверх
    {0x04, 0x0c, 0x1c, 0x3c, 0x1c, 0x0c, 0x04},  // Стрелка вниз
    {0x00, 0x08, 0x1c, 0x3e, 0x7f, 0x00, 0x00},  // Стрелка влево
    {0x60, 0x70, 0x3e, 0x19, 0x11, 0x10, 0x0c},  // Настройки
    {0x01, 0x03, 0x7f, 0x5d, 0x55, 0x55, 0x7f},  // Справка
    {0x70, 0x7c, 0x7e, 0x53, 0x7e, 0x7c, 0x70}   // Предупреждение
};

// Логотип 128х32
const uint8_t bmp_logo_pgm[] PROGMEM = {
	0xfe, 0xfe, 0x06, 0x06, 0x06, 0x06, 0xe6, 0xe6, 0xe6, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xcf, 0x87, 0x87, 0xcf, 0xdf, 0x9f, 0x3f, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xe0, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfd, 
	0xf8, 0xf0, 0xfd, 0xfd, 0xfd, 0xfd, 0x8d, 0x8d, 0x8d, 0xdd, 0xdd, 0xcc, 0xe4, 0xf1, 0xf9, 0xfd, 
	0xfd, 0xf8, 0xf0, 0xf0, 0xf0, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x03, 0x03, 0x03, 0x03, 
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00
};

// Флажки 16х16
const uint8_t bmp_checkbox_pgm[][32] PROGMEM = {
    {0xfc, 0xfe, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0xfe, 0xfc,
	 0x3f, 0x7f, 0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0x7f, 0x3f},  // Флажка нет
	{0xfc, 0xfe, 0x07, 0x03, 0x83, 0x83, 0x03, 0x03, 0x03, 0xc3, 0xf3, 0x33, 0x03, 0x07, 0xfe, 0xfc,
	 0x3f, 0x7f, 0xe0, 0xc0, 0xc1, 0xc3, 0xc7, 0xc6, 0xc7, 0xc3, 0xc0, 0xc0, 0xc0, 0xe0, 0x7f, 0x3f}   // Флажок есть
};


// Заголовки для меню и текстовки
const char menu_options_text_pgm[]   PROGMEM = "Настройки";
const char menu_contrast_text_pgm[]  PROGMEM = "Яркость";
const char menu_invert_text_pgm[]    PROGMEM = "Инверсия цвета";
const char menu_flip_text_pgm[]      PROGMEM = "Поворот экрана";
const char menu_light_text_pgm[]     PROGMEM = "Время подсветки";
const char menu_reset_text_pgm[]     PROGMEM = "Перезагрузка";
const char menu_help_text_pgm[]      PROGMEM = "Справка";
const char menu_about_text_pgm[]     PROGMEM = "О прошивке";

const char data_about_text_pgm[]     PROGMEM = "VS PROJECT";
const char data_help_text_pgm[]      PROGMEM = "Менеджер паролей позволяет выполнять ввод логинов и паролей от веб-сайтов и приложений";

const char data_space_text_pgm[]     PROGMEM = "                     ";
const char data_auth_text_pgm[]      PROGMEM = "Введите PIN";
const char data_auth_error_pgm[]     PROGMEM = "Много неверных PIN";

const char data_print_accept_text_pgm[]    PROGMEM = "Подтвердить";
const char data_print_login_text_pgm[]     PROGMEM = "Ввод аккаунта ?";
const char data_print_password_text_pgm[]  PROGMEM = "Ввод пароля ?";


// Подключаем пользовательские настройки
#include "notes.h"

// Добавляем PIN
#ifdef PIN_MASTER_PASSWORD
    const char master_password_pgm[]     PROGMEM = PIN_MASTER_PASSWORD;
#else
    const char master_password_pgm[]     PROGMEM = "0000";
#endif

// Добавляем версию
#ifdef COMPILE_VERSION
    const char data_about_version_pgm[]  PROGMEM = COMPILE_VERSION;
#else
    const char data_about_version_pgm[]  PROGMEM = "XXXX-XX-XX";
#endif


#if defined(COMPILE_ENABLE_CONTRAST) || defined(COMPILE_ENABLE_LIGHT) || defined(COMPILE_ENABLE_INVERT) || defined(COMPILE_ENABLE_FLIP)
    #define COMPILE_ENABLE_ANY_OPTIONS
#endif

// Методы инициализации дисплея и подготовка курсоров
void InitializeMenu();

void ChangeViewMode(MenuType type, UINTX id = MENU_ROOT_ID);
void DrawMenuActiveText(const MenuItem m);

// Методы отрисовки элементов
void DrawMenuFolder(const MenuItem mp);
void DrawMenuHelp();
void DrawMenuLogo();
void DrawMenuInteger(const char title[25], int16_t value, bool isPercent);
void DrawMenuBoolean(const char title[25], bool value);
void DrawMenuAuth(const char password[5], int8_t indexActive, bool masked);
void DrawMenuNote(const MenuItem m, int8_t buttonMode = 0);
void DrawMenuMessage(const char message[20]);


// Общие методы
uint8_t GetIconByMenuType(MenuType type);
MenuItem GetMenuItem(UINTX index);
MenuItem GetMenuItemById(UINTX menuId);
UINTX GetMenuLengthAll();
UINTX GetMenuLengthById(UINTX parentId);

// Управление подсветкой
void LightTick();
void LightOn();

// Применить настройки дисплея
void ApplySettings();