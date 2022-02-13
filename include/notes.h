#pragma once
#define COMPILE_VERSION "2022-02-13"

#define COMPILE_ENABLE_PIN
#define PIN_MASTER_PASSWORD "0000"
#define PIN_MAX_ATTEMPT 3
#define PIN_WRONG_DELAY 120UL

#define COMPILE_ENABLE_CONTRAST
#define COMPILE_ENABLE_INVERT
#define COMPILE_ENABLE_FLIP
#define COMPILE_ENABLE_LIGHT
#define COMPILE_ENABLE_HELP
#define COMPILE_ENABLE_LOGO
#define COMPILE_ENABLE_RESET
#define COMPILE_ENABLE_CONFIRMATION

const char str_item_1_title_pgm[] PROGMEM = "Folder";
const char str_item_2_title_pgm[] PROGMEM = "SubFolder";
const char str_item_3_title_pgm[] PROGMEM = "Folder #2";

const char str_item_4_title_pgm[] PROGMEM = "Account #1";
const char str_item_4_login_pgm[] PROGMEM = "User1";
const char str_item_4_password_pgm[] PROGMEM = "12345ABCD1";
const char str_item_5_title_pgm[] PROGMEM = "Account #2";
const char str_item_5_login_pgm[] PROGMEM = "User2";
const char str_item_5_password_pgm[] PROGMEM = "12345ABCD2";
const char str_item_6_title_pgm[] PROGMEM = "Account in root";
const char str_item_6_login_pgm[] PROGMEM = "User2@domain.com";
const char str_item_6_password_pgm[] PROGMEM = "12345ABCD3";

const MenuItemPGM MenuArray[] PROGMEM = {
	{MENU_TYPE_FOLDER, 1, MENU_ROOT_ID, str_item_1_title_pgm},
	{MENU_TYPE_FOLDER, 2, 1, str_item_2_title_pgm},
	{MENU_TYPE_FOLDER, 3, MENU_ROOT_ID, str_item_3_title_pgm},

	{MENU_TYPE_NOTE, 4, 1, str_item_4_title_pgm, str_item_4_login_pgm, str_item_4_password_pgm, SCENARIO_MODE_LOGIN_TAB_PASSWORD_ENTER}, 
	{MENU_TYPE_NOTE, 5, 2, str_item_5_title_pgm, str_item_5_login_pgm, str_item_5_password_pgm, SCENARIO_MODE_LOGIN_TAB_PASSWORD_ENTER}, 
	{MENU_TYPE_NOTE, 6, MENU_ROOT_ID, str_item_6_title_pgm, str_item_6_login_pgm, str_item_6_password_pgm, SCENARIO_MODE_MANUAL}, 

	{MENU_TYPE_OPTIONS, MENU_OPTIONS_ID, MENU_ROOT_ID, menu_options_text_pgm},
		{MENU_TYPE_OPTIONS_CONTRAST, MENU_OPTIONS_CONTRAST_ID, MENU_OPTIONS_ID, menu_contrast_text_pgm},
		{MENU_TYPE_OPTIONS_INVERT, MENU_OPTIONS_INVERT_ID, MENU_OPTIONS_ID, menu_invert_text_pgm},
		{MENU_TYPE_OPTIONS_FLIP, MENU_OPTIONS_FLIP_ID, MENU_OPTIONS_ID, menu_flip_text_pgm},
		{MENU_TYPE_OPTIONS_LIGHT, MENU_OPTIONS_LIGHT_ID, MENU_OPTIONS_ID, menu_light_text_pgm},
		{MENU_TYPE_OPTIONS_RESET, MENU_OPTIONS_RESET_ID, MENU_OPTIONS_ID, menu_reset_text_pgm},
	{MENU_TYPE_HELP, MENU_HELP_ID, MENU_ROOT_ID, menu_help_text_pgm},
	{MENU_TYPE_LOGO, MENU_LOGO_ID, MENU_ROOT_ID, menu_about_text_pgm}
};