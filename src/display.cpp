#include "main.h"
#include "display.h"
#include "settings.h"
#include "notes.h"
#include "MemoryFree.h"

GyverOLED<SSD1306_128x32, OLED_BUFFER, OLED_I2C> display;

// Для регулировки подсветкой дисплея
unsigned long timerLightOff;
bool flagLightOff;

int GlobalCursorMin;  // Минимальное кол-во пунктов всего
int GlobalCursorMax;  // Максимальное кол-во пунктов всего (или у папки)
int GlobalCursorY;    // Глобальная позиция курсора для навигации по пунктам меню

int LocalCursorMin;  // Минимальное кол-во пунктов в окне просмотра
int LocalCursorMax;  // Максимальное кол-во пунктов в окне просмотра
int LocalCursorY;    // Позиция курсора для отображаемых в текущий момент на экране пунктов.
                     //      т.е. вместо уменьшения глобальной позиции сначала уменьшает локальную до 0, а только потом глобальную.
                     //           вместо увеличения глобальной позиции сначала увеличиваем локальную до N, а потом увеличиваем глобальную, но локальный счётчик не трогаем после.

MenuType ViewMode;                       // Текущий режим отображения
UINTX SelectedMenuItemId = MENU_ROOT_ID; // Выбранный текущий пункт меню
UINTX ActiveMenuItemId = MENU_ROOT_ID;   // Активный пункт меню под курсором


// Найти количество всех элементов в массиве
UINTX GetMenuLengthAll() {
    return sizeof(MenuArray) / sizeof(MenuItemPGM);
}

// Возвратить элемент меню из глобального массива
MenuItem GetMenuItem(UINTX index) {
    MenuItem temp;

    // Начальные значения, иначе в следующий элемент могут попасть предыдущие
    temp.type = MENU_TYPE_NOTE;
    temp.mode = SCENARIO_MODE_PASSWORD;
    temp.id = temp.parentId = 0;
    temp.title[0] = temp.login[0] = temp.password[0] = '\0';

    // Числа читаем через функции pgm
    if (MENU_ROOT_ID == UINT8_MAX) {
        temp.id = (UINTX) pgm_read_byte(&(MenuArray[index].id));
        temp.parentId = (UINTX) pgm_read_byte(&(MenuArray[index].parentId));
    } else {
        temp.id = (UINTX) pgm_read_word(&(MenuArray[index].id));
        temp.parentId = (UINTX) pgm_read_word(&(MenuArray[index].parentId));
    }
    
    // Перечисления копируем так
    memcpy_P(&(temp.type), &(MenuArray[index].type), sizeof(MenuType));
    
    // Строки читаем так
    strcpy_P(temp.title, (PGM_P) pgm_read_word(&(MenuArray[index].title)));

    // Для заметок, копируем ещё данные
    if (temp.type == MENU_TYPE_NOTE ){
        strcpy_P(temp.login, (PGM_P) pgm_read_word(&(MenuArray[index].login)));
        strcpy_P(temp.password, (PGM_P) pgm_read_word(&(MenuArray[index].password)));

        memcpy_P(&(temp.mode), &(MenuArray[index].mode), sizeof(ScenarioMode));
    }

    return temp;
}

// Найти пункт меню по его ID
MenuItem GetMenuItemById(UINTX menuId) {
    MenuItem m;

    // Если запросили Корень, тогда вернём виртуальный элемент
    if (menuId == MENU_ROOT_ID){
        m.type = MENU_TYPE_ROOT;
        m.id = m.parentId = MENU_ROOT_ID;
        return m;
    }
    
    // Перебираем все пункты меню
    for (UINTX i = 0; i < GetMenuLengthAll(); i++) {
        m = GetMenuItem(i);

        if (GetMenuItem(i).id == menuId)
            return m;
    }
}

// Найти количество дочерних пунктов родительскому ID
UINTX GetMenuLengthById(UINTX parentId) {
    UINTX count = 0;

    for (UINTX i = 0; i < GetMenuLengthAll(); i++) {
        if (parentId == GetMenuItem(i).parentId)
            count++;
    }

    return count;
}

// Вернуть индекс значка по типу меню
uint8_t GetIconByMenuType(MenuType type){
    switch (type) {
        case MENU_TYPE_FOLDER:
            return BMP_ICONS_FOLDER;
        case MENU_TYPE_NOTE:
            return BMP_ICONS_FILE;
        case MENU_TYPE_OPTIONS:
        case MENU_TYPE_OPTIONS_CONTRAST:
        case MENU_TYPE_OPTIONS_FLIP:
        case MENU_TYPE_OPTIONS_INVERT:
        case MENU_TYPE_OPTIONS_LIGHT:
            return BMP_ICONS_TOOL;
        case MENU_TYPE_ROOT:
        case MENU_TYPE_HELP:
            return BMP_ICONS_HELP;
        case MENU_TYPE_OPTIONS_RESET:
            return BMP_ICONS_WARNING;
        default:
            return BMP_ICONS_CAT;
    }
}

// Метод вызывается для инициализации дисплея
void InitializeMenu() {
    display.init();
}


// Настройка курсоров и режима отображения. Опционально передаём ID меню, требуется для папок
void ChangeViewMode(MenuType type, UINTX id = MENU_ROOT_ID) {
    ViewMode = type; // Выставляем режим

    // Ограничители для курсоров
    switch (ViewMode) {
        case MENU_TYPE_NOTE:            
            GlobalCursorMin = GlobalCursorMax = GlobalCursorY = 0;
            LocalCursorMin = LocalCursorMax = LocalCursorY = 0;

        case MENU_TYPE_AUTH:
            #ifdef COMPILE_ENABLE_PIN
                GlobalCursorMin = GlobalCursorY = 0;
                GlobalCursorMax = 9;
                LocalCursorMin = LocalCursorMax = LocalCursorY = 0;
            #endif
            break;

        case MENU_TYPE_ROOT:
            GlobalCursorMin = 0;
            GlobalCursorMax = max(GetMenuLengthById(id) - 1, 0);

            LocalCursorMin = 0;
            LocalCursorMax = 3;
            
            // Сбрасываем текущий курсор по-умолчанию
            GlobalCursorY = 0;
            LocalCursorY = 0;
            break;

        case MENU_TYPE_FOLDER:
        case MENU_TYPE_OPTIONS:
            GlobalCursorMin = -2;
            GlobalCursorMax = max(GetMenuLengthById(id) - 1, 0);

            LocalCursorMin = 0;
            LocalCursorMax = 1;

            // Сбрасываем текущий курсор по-умолчанию
            GlobalCursorY = -2;
            LocalCursorY = -2;
            break;

        case MENU_TYPE_HELP:
            #ifdef COMPILE_ENABLE_HELP
                GlobalCursorMin = GlobalCursorY = 0;
                GlobalCursorMax = max(strlen_P(data_help_text_pgm) / (DISPLAY_COLS * 2) - 1, 0);

                LocalCursorMin = LocalCursorMax = LocalCursorY = 0;
            #endif
            break;
            
        case MENU_TYPE_OPTIONS_CONTRAST:
            #ifdef COMPILE_ENABLE_HELP
                GlobalCursorMin = 0;
                GlobalCursorMax = 20; // шаг 5% от 0% до 100% 
                GlobalCursorY = SettingDisplayContrast / 5;

                LocalCursorMin = LocalCursorMax = LocalCursorY = 0;
            #endif
            break;

        case MENU_TYPE_OPTIONS_LIGHT:
            #ifdef COMPILE_ENABLE_LIGHT
                GlobalCursorMin = 0;
                GlobalCursorMax = 18; // шаг 10 сек от 0 до 180 сек. 0 - не отключать экран 
                GlobalCursorY = SettingDisplayLight / 10;

                LocalCursorMin = LocalCursorMax = LocalCursorY = 0;
            #endif
            break;

        case MENU_TYPE_OPTIONS_INVERT:
        case MENU_TYPE_OPTIONS_FLIP:
            #if defined(COMPILE_ENABLE_INVERT) || defined(COMPILE_ENABLE_FLIP)
                GlobalCursorMin = 0;
                GlobalCursorMax = 1;
                LocalCursorMin = LocalCursorMax = LocalCursorY = 0;

                #ifdef COMPILE_ENABLE_INVERT
                    if (ViewMode == MENU_TYPE_OPTIONS_INVERT)
                        GlobalCursorY = SettingDisplayInvert ? 1 : 0;
                #endif
                
                #ifdef COMPILE_ENABLE_FLIP
                    if (ViewMode == MENU_TYPE_OPTIONS_FLIP)
                        GlobalCursorY = SettingDisplayFlip ? 1 : 0;
                #endif
            #endif
            break;
    }
}

// Отрисовать активную (выбранный) пункт меню
void DrawMenuActiveText(const MenuItem m) {
    int8_t rowDrawY;
    char menuTitleBuffer[MAX_TITLE];

    if (ViewMode == MENU_TYPE_ROOT) {
        rowDrawY = LocalCursorY;
    } else {
        rowDrawY = LocalCursorY + 2;
    }
    
    display.setCursor(OFFSET_X_MENU_ITEM_TEXT - 2, rowDrawY);
    display.invertText(true);
    display.print(GetStringPgm(data_space_text_pgm, menuTitleBuffer));
    display.invertText(false);

    display.setCursor(OFFSET_X_MENU_ITEM_TEXT, rowDrawY);
    display.invertText(true);
    display.print(m.title);
    display.invertText(false);
}

// Метод отрисовывает пункт меню для открытой папки и возвращает выделенный пункт
void DrawMenuFolder(const MenuItem mp) {
    int8_t menuItemNumber = 0;
    int menuOffset = 0;

    // Чистим экран
    display.clear();
    display.home();
    display.autoPrintln(false);
    display.setScale(1);

    // В режиме "Папка" отображаем отдельно область заголовка
    switch (mp.type) {
        case MENU_TYPE_ROOT:
            break;

        case MENU_TYPE_OPTIONS:
        case MENU_TYPE_FOLDER:
            display.drawBitmap(OFFSET_X_MENU_ITEM_ICON, OFFSET_X_MENU_ITEM_ICON, bmp_icons_pgm[BMP_ICONS_ARROW_LEFT], BMP_SIZE, BMP_SIZE); // Значок в заголовке

            // Рисуем курсор-выделение, если курсор в заголовке
            if (GlobalCursorY < 0 && LocalCursorY < 0) {
                ActiveMenuItemId = mp.id;
                DrawMenuActiveText(mp);
 
            } else {
                // Текст заголовка
                display.setCursor(OFFSET_X_MENU_ITEM_TEXT, 0);
                display.print(mp.title);
            }

            // Горизонтальная линия
            display.fastLineH(OFFSET_X_MENU_ITEM_TEXT - 1, 0, DISPLAY_WIDTH - 1);
            break;
    }


    // По всем массиву меню
    for (UINTX i = 0; i < GetMenuLengthAll(); i++) {
        MenuItem m = GetMenuItem(i);

        // Нашли дочерние пункты для текущей папки, при чём эта папка сама не является корнем
        if (m.parentId == mp.id && m.id != mp.id) {
            menuOffset++;

            // Если пропустили достаточное количество меню (проскролили)
            if (menuOffset > GlobalCursorY) {

                // Если это активный пункт меню
                if (menuOffset == GlobalCursorY + LocalCursorY + 1) {
                    
                    // Рисуем курсор-выделение на элементе
                    ActiveMenuItemId = m.id;
                    DrawMenuActiveText(m);

                } else  {

                    // Текст дочернего элемента
                    display.setCursor(OFFSET_X_MENU_ITEM_TEXT, menuItemNumber + (mp.type == MENU_TYPE_ROOT ? 0 : 2));
                    display.print(m.title);
                }

                // Значок
                display.drawBitmap(OFFSET_X_MENU_ITEM_ICON, menuItemNumber * 8 + (mp.type == MENU_TYPE_ROOT ? 0 : 2 * 8) + OFFSET_X_MENU_ITEM_ICON, bmp_icons_pgm[GetIconByMenuType(m.type)], BMP_SIZE, BMP_SIZE);
                menuItemNumber++;
            }
        }
    
        // Рисуем не более N-пунктов
        if (menuItemNumber > LocalCursorMax)
            break;
    }

    // Рисуем
    display.update();
}

// Метод отрисовывает пункт меню "Справка"
void DrawMenuHelp() {
    int len_max_screen = DISPLAY_COLS * DISPLAY_ROWS * 2; // 21 строка * 4 строки * 2 байта (UTF-8)
    int len_max_row = DISPLAY_COLS * 2;

    char buffer[len_max_screen];
    for (int i = 0; i < len_max_screen; i++) {
        buffer[i] = pgm_read_byte_near(data_help_text_pgm + (GlobalCursorY * len_max_row) + i);
    }

    display.clear();
    display.home();
    display.setScale(1);
    display.autoPrintln(true);
    display.print(buffer);
    display.update();
}

// Метод отрисовывает пункт меню "О приложении"
void DrawMenuLogo() {
    display.clear();
    display.invertDisplay(true);
    display.setCursorXY(4, 4);

    char buffer[MAX_TITLE];
    display.print(GetStringPgm(data_about_text_pgm, buffer));

    display.setCursorXY(OFFSET_X_MENU_ITEM_TEXT, 18);
    display.print(GetStringPgm(data_about_version_pgm, buffer));

    display.drawBitmap(DISPLAY_WIDTH - 48, DISPLAY_HEIGHT - 21, bmp_logo_pgm, 46, 19);
    display.update();
}

// Метод отрисовывает меню для числовых и процентных значений
void DrawMenuInteger(const char title[MAX_TITLE], int16_t value, bool isPercent) {
    display.clear();
    
    // Заголовок
    display.setScale(1);
    display.setCursor(2, 0);
    display.print(title);

    // Позиционируем значение по центру
    uint8_t coordX = (value <= 9 ? 50 : (value >= 99 ? 30 : 40));
    display.setCursorXY(coordX, 14);
    display.setScale(2);

    // Показываем значение и знак % если требуется
    display.print(value);
    if (isPercent)
        display.print("%");

    // Маркеры вверх и вниз
    display.drawBitmap(90, 12, bmp_icons_pgm[BMP_ICONS_ARROW_UP], BMP_SIZE, BMP_SIZE);
    display.drawBitmap(90, 24, bmp_icons_pgm[BMP_ICONS_ARROW_DOWN], BMP_SIZE, BMP_SIZE);
    display.update();
}

// Метод отрисовывает меню для булевых значений
void DrawMenuBoolean(const char title[MAX_TITLE], bool value) {
    display.clear();
    
    // Заголовок
    display.setScale(1);
    display.setCursor(2, 0);
    display.print(title);

    // Позиционируем чекбокс по центру
    display.drawBitmap(DISPLAY_WIDTH / 2 - 8, 14, bmp_checkbox_pgm[value ? 1 : 0], 16, 16);
    display.update();
}

// Метод отрисовывает окно с подтверждением отправки логина и пароля
void DrawMenuNote(const MenuItem m, int8_t buttonMode = 0) {
    uint8_t xPos = 30;
    uint8_t yPos = 20;

    display.clear();
    
    // Заголовок
    display.setScale(1);
    display.autoPrintln(false);
    display.setCursor(2, 0);
    display.print(m.title);

    // Отображаем текст кнопки
    char buffer[MAX_TITLE];

    if (buttonMode > 1)
        xPos = xPos - 10;

    display.setCursorXY(xPos, yPos);

    switch (buttonMode) {
        case 1:
            display.print(GetStringPgm(data_print_accept_text_pgm, buffer));
            break;
        case 2:
            display.print(GetStringPgm(data_print_login_text_pgm, buffer));
            break;
        case 3:
            display.print(GetStringPgm(data_print_password_text_pgm, buffer));
            break;
    }
    
    display.rect(12, 16, 116, 31, OLED_STROKE);
    display.update();
}

// Метод отрисовывает меню для авторизации
void DrawMenuAuth(const char password[5], int8_t indexActive, bool masked) {

    // Заголовок
    display.clear();
    display.setScale(1);
    display.setCursor(2, 0);
    
    char menuItemTitle[MAX_TITLE];
    display.print(GetStringPgm(data_auth_text_pgm, menuItemTitle));

    // Рисуем знако-места
    uint8_t xPos;
    const uint8_t yPos = 14;
    const uint8_t w = 12;
    const uint8_t h = 14;

    for (uint8_t x = 0; x < 5 - 1; x++) {
        xPos = 14 + (x * w) * 2;
        
        if (x == indexActive) {
            display.rect(xPos, yPos, xPos + w, yPos + h, OLED_FILL);
            display.textMode(BUF_SUBTRACT);
        } else {
            display.rect(xPos, yPos, xPos + w, yPos + h, OLED_STROKE);
            display.textMode(BUF_ADD);
        }

        display.setCursorXY(xPos + 4, yPos + 4);
        
        // Если символ в массиве не нулл, выводим его. Если необходимо маскировать, то выполняем для раннее введёных символов
        if (x <= indexActive) {
            display.print(masked && x < indexActive ? '*' : password[x]);
        }
    }
    
    display.textMode(BUF_REPLACE);
    display.update();
}

// Отобразить сообщение
void DrawMenuMessage(const char message[25]) {
    display.clear();
    display.setScale(1);
    display.autoPrintln(true);
    display.setCursor(OFFSET_X_MENU_ITEM_TEXT, 1);
    display.print(message);
    display.update();
}


// Применить настройки
void ApplySettings() {
    display.setContrast(map(SettingDisplayContrast, 0, 100, 0, 255));
    display.flipH(SettingDisplayFlip);
    display.flipV(SettingDisplayFlip);
    display.invertDisplay(SettingDisplayInvert);
}

// Функцию для регулировки подсветки дисплея
void LightTick() {

    // Если задана настройка и экран сейчас не выключен
    if (SettingDisplayLight > 0 && !flagLightOff) {

        // Если время после последней активности больше чем задано настройкой
        if (millis() >= timerLightOff + (SettingDisplayLight * 1000UL)) {
            flagLightOff = true;
            display.setPower(false);
        }
    }
}

// Функция активации подсветки дисплея
void LightOn() {

    // Если экран выключен, включим его
    if (flagLightOff){
        flagLightOff = false;
        display.setPower(true);
    }
    
    // Сбрасываем счётчик
    timerLightOff = millis();
}

