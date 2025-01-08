#include "main.h"
#include "settings.h"

EncButton encoder(A1, A2, A3);

// Кастомные обработки пользовательских заметок ///////
const char messageP1[] PROGMEM = "Кажду минуту <ESC>";

void PressEscapeEveryMinute(){
    char bufferMessage[30];
    DrawMenuMessage(GetStringPgm(messageP1, bufferMessage));

    unsigned long timeoutCounter = millis() + (60 * 1000UL);

    // Ожидаем действия от пользователя
    while(true) {
        encoder.tick();
        
        // При нажатии выходим из работы заметки
        if (encoder.click())
            return;

        // Сбрасываем счётчик и выполняем рабочую нагрузку
        if (millis() >= timeoutCounter){
            timeoutCounter = millis() + (60 * 1000UL);

            Keyboard.press(KEY_ESC);
            Keyboard.release(KEY_ESC);
        }
    }
}

///////////////////////////////////////////////////////


// Настраиваем
void setup() {
    
    // Отключаем светодиоды
    pinMode(LED_BUILTIN_TX, INPUT);
    pinMode(LED_BUILTIN_RX, INPUT);

    // Настраиваем энкодер
    encoder.setEncType(EB_STEP2);

    // Инициализация клавиатуры
    Keyboard.begin();

    // Инциализация дисплея
    InitializeMenu();

    // Заставка
    #ifdef COMPILE_ENABLE_LOGO
        DrawMenuLogo();
        delayMillis(3000);
    #endif

    // При первом запуске задаём настройки по-умолчанию и сохраняем их
    #if defined(COMPILE_ENABLE_PIN) || defined(COMPILE_ENABLE_ANY_OPTIONS)
        if (IsFirstRun()) {
            #ifdef COMPILE_ENABLE_PIN
                SetSettingAttempt(0);
            #endif

            #ifdef COMPILE_ENABLE_ANY_OPTIONS
                SaveSettings();
            #endif
        }
    #endif

    // Загрузка настроек в глобальные переменные
    #ifdef COMPILE_ENABLE_ANY_OPTIONS
        LoadSettings();
    #endif

    // Применение настроек
    ApplySettings();

    // Проверить PIN
    #ifdef COMPILE_ENABLE_PIN
        GoAuthLoop();
    #endif

    // Включим подсветку
    #ifdef COMPILE_ENABLE_LIGHT
        LightOn();
    #endif

    // Перейти в корень
    GoFolderRoot();
}


// Выполняем
void loop() {

    // Считываем положение энкодера
    if (encoder.tick()){
        #ifdef COMPILE_ENABLE_LIGHT
            LightOn();
        #endif
    }

    // Выполняем функцию для регулировки подсветки дисплея
    #ifdef COMPILE_ENABLE_LIGHT
        LightTick();
    #endif

    // Выбираем действия по текущему представлению
    switch (ViewMode) {
        case MENU_TYPE_ROOT:
        case MENU_TYPE_OPTIONS:
        case MENU_TYPE_FOLDER:
            if (encoder.turn()){
                if (encoder.right())
                    GoCursorUp();
            
                if (encoder.left())
                    GoCursorDown();
                
                // Отрисовать папку с курсором-выделением
                MenuItem m = GetMenuItemById(SelectedMenuItemId);
                DrawMenuFolder(m);
            }

            if (encoder.hold())
                GoFolderRoot();

            if (encoder.click()) {
                
                // Если курсор на пункте меню открытой папки, значит мы на заголовке
                if (SelectedMenuItemId == ActiveMenuItemId) {

                    // Находим пункт меню по ID и ищем родителя
                    MenuItem m = GetMenuItemById(GetMenuItemById(ActiveMenuItemId).parentId);
                    ChangeViewMode(m.type, m.id);

                    // Переходим в папку "выше"
                    GoFolderTo(m);

                } else {

                    // Узнаём что выбрано под курсором и "открываем"
                    MenuItem m = GetMenuItemById(ActiveMenuItemId);

                    // Меняем представление, кроме заметок
                    if (m.type != MENU_TYPE_NOTE)
                        ChangeViewMode(m.type, m.id);

                    char menuItemTitle[MAX_TITLE];

                    switch (m.type) {
                        case MENU_TYPE_NOTE:
                            if (m.mode == SCENARIO_MODE_CUSTOM) {
                                GoCustomProcessor(m);

                            } else if (m.mode == SCENARIO_MODE_MANUAL) {
                                DrawMenuNote(m, 2);
                                GoAutoProcessor(m, 2); // Ожидание логина

                                DrawMenuNote(m, 3);
                                GoAutoProcessor(m, 3); // Ожидание пароля

                            } else {
                                DrawMenuNote(m, 1);
                                GoAutoProcessor(m, 1);
                            }

                            // Перерисовка папки в которой находится заметка
                            DrawMenuFolder(GetMenuItemById(m.parentId));

                            //MenuItem mp2 = GetMenuItemById(m.parentId); // NotWork!
                            //DrawMenuFolder(mp2);
                            break;
                        
                        case MENU_TYPE_OPTIONS:
                        case MENU_TYPE_FOLDER:
                            GoFolderTo(m);
                            break;

                        case MENU_TYPE_HELP:
                            #ifdef COMPILE_ENABLE_HELP
                                DrawMenuHelp();
                            #endif
                            break;

                        case MENU_TYPE_LOGO:
                            #ifdef COMPILE_ENABLE_LOGO
                                DrawMenuLogo();
                            #endif
                            break;

                        case MENU_TYPE_OPTIONS_CONTRAST:
                            #ifdef COMPILE_ENABLE_CONTRAST
                                DrawMenuInteger(GetStringPgm(menu_contrast_text_pgm, menuItemTitle), SettingDisplayContrast, true);
                            #endif
                            break;

                        case MENU_TYPE_OPTIONS_LIGHT:
                            #ifdef COMPILE_ENABLE_LIGHT
                                DrawMenuInteger(GetStringPgm(menu_light_text_pgm, menuItemTitle), SettingDisplayLight, false);
                            #endif
                            break;

                        case MENU_TYPE_OPTIONS_FLIP:
                            #ifdef COMPILE_ENABLE_FLIP
                                DrawMenuBoolean(GetStringPgm(menu_flip_text_pgm, menuItemTitle), SettingDisplayFlip);
                            #endif
                            break;

                        case MENU_TYPE_OPTIONS_INVERT:
                            #ifdef COMPILE_ENABLE_INVERT
                                DrawMenuBoolean(GetStringPgm(menu_invert_text_pgm, menuItemTitle), SettingDisplayInvert);
                            #endif
                            break;

                        case MENU_TYPE_OPTIONS_RESET:
                            #ifdef COMPILE_ENABLE_RESET
                                PLATFORM_RESET(); // Программмный сброс
                            #endif
                            break;
                    }
                }

            } // End Click
            break;

        case MENU_TYPE_LOGO:
            #ifdef COMPILE_ENABLE_LOGO
                if (encoder.click()) {
                    ApplySettings();
                    GoFolderRoot(); // Вернуться в корень
                }
            #endif
            break;

        case MENU_TYPE_HELP:
            #ifdef COMPILE_ENABLE_HELP
                if (encoder.turn()){
                    if (encoder.right())
                        GoCursorUp();
                    
                    if (encoder.left())
                        GoCursorDown();

                    DrawMenuHelp();
                }

                if (encoder.click())
                    GoFolderRoot(); // Вернуться в корень
            #endif
            break;

        case MENU_TYPE_OPTIONS_CONTRAST:
        case MENU_TYPE_OPTIONS_INVERT:
        case MENU_TYPE_OPTIONS_FLIP:
        case MENU_TYPE_OPTIONS_LIGHT:
            #ifdef COMPILE_ENABLE_ANY_OPTIONS
            if (encoder.turn()){

                // Перемещаем курсоры
                if (encoder.left())
                    GoCursorDown();
                
                if (encoder.right())
                    GoCursorUp();

                char menuItemTitle[MAX_TITLE];

                if (ViewMode == MENU_TYPE_OPTIONS_CONTRAST) {
                    #ifdef COMPILE_ENABLE_CONTRAST
                        SettingDisplayContrast = GlobalCursorY * 5;

                        ApplySettings(); // Применить настройки
                        DrawMenuInteger(GetStringPgm(menu_contrast_text_pgm, menuItemTitle), SettingDisplayContrast, true); // Выводим на экран
                    #endif
                } else if (ViewMode == MENU_TYPE_OPTIONS_LIGHT) {
                    #ifdef COMPILE_ENABLE_LIGHT
                        SettingDisplayLight = GlobalCursorY * 10;

                        ApplySettings(); // Применить настройки
                        DrawMenuInteger(GetStringPgm(menu_light_text_pgm, menuItemTitle), SettingDisplayLight, false); // Выводим на экран
                    #endif
                } else if (ViewMode == MENU_TYPE_OPTIONS_INVERT) {
                    #ifdef COMPILE_ENABLE_INVERT
                        SettingDisplayInvert = GlobalCursorY == 1;

                        ApplySettings(); // Применить настройки
                        DrawMenuBoolean(GetStringPgm(menu_invert_text_pgm, menuItemTitle), SettingDisplayInvert); // Выводим на экран
                    #endif
                } else if (ViewMode == MENU_TYPE_OPTIONS_FLIP) {
                    #ifdef COMPILE_ENABLE_FLIP
                        SettingDisplayFlip = GlobalCursorY == 1;

                        ApplySettings(); // Применить настройки
                        DrawMenuBoolean(GetStringPgm(menu_flip_text_pgm, menuItemTitle), SettingDisplayFlip); // Выводим на экран
                    #endif
                }
            }

            // Сохранение настроек и вверх в меню настроек
            if (encoder.click()) {
                SaveSettings();
                GoFolderSelectedItem();
            }
                
            // Загрузка и восстановление настроек и вверх в меню настроек
            if (encoder.hold()) {
                LoadSettings();
                ApplySettings();
                GoFolderSelectedItem();
            }
            #endif
            break;

        case MENU_TYPE_NOTE:
            return;
            break;

    } // End Switch

} // End Loop


// Общая функция для работы локального и глобальных курсоров
void GoCursorUp() {

    // Обычные режимы
    if (LocalCursorY > LocalCursorMin) {
        LocalCursorY--;

    } else {
        if (GlobalCursorY > GlobalCursorMin) {
            GlobalCursorY--;
        }

        // Делаем прыжок между разделителем папки и содержимого. Для режима папки
        if (GlobalCursorY == -1) {
            GlobalCursorY = -2;
            LocalCursorY = -2;
        }
    }
}

// Общая функция для работы локального и глобальных курсоров
void GoCursorDown() {

    // Делаем прыжок между разделителем папки и содержимого. Для режима папки
    if (GlobalCursorY == -2) {
        GlobalCursorY = 0;
        LocalCursorY = 0;

    } else {
        // Обычные режимы
        if (LocalCursorY < min(LocalCursorMax, GlobalCursorMax)) {
            LocalCursorY++;

        } else {
            if (GlobalCursorY < GlobalCursorMax - LocalCursorMax) {
                GlobalCursorY++;
            }
        }
    }
}

// Перейти в корневую папку
void GoFolderRoot(){
    MenuItem m = GetMenuItemById(MENU_ROOT_ID);
    GoFolderTo(m);
}

// Перейти в выбранный элемент
void GoFolderSelectedItem(){
    MenuItem m = GetMenuItemById(SelectedMenuItemId);
    GoFolderTo(m);
}

// Перейти в папку
void GoFolderTo(const MenuItem m) {
    SelectedMenuItemId = m.id;
    ActiveMenuItemId = 0;

    ChangeViewMode(m.type, m.id);
    DrawMenuFolder(m);
}

// Обработка заметок в автоматическом и пользовательском режимах
void GoAutoProcessor(const MenuItem m, uint8_t step = 1) {

    // Ждём действия от пользователя
    #ifdef COMPILE_ENABLE_CONFIRMATION
    while(true) {
        encoder.tick();
        
        // Выполняем, идём дальше
        if (encoder.click())
            break;        

        // Отмена
        if (encoder.hold())
            return;
    }
    #endif

    // Для ручного режима (ввод логина)
    if (step == 2) {
        Keyboard.print(m.login);
        return;
    }

    // Для ручного режима (ввод пароля)
    if (step == 3) {
        Keyboard.print(m.password);
        return;
    }

    // Для автоматических режимов
    if (step == 1) {
        
        if (m.mode == SCENARIO_MODE_PASSWORD || m.mode == SCENARIO_MODE_PASSWORD_ENTER) {
            Keyboard.print(m.password);
        }
        
        if (m.mode == SCENARIO_MODE_LOGIN_TAB_PASSWORD || m.mode == SCENARIO_MODE_LOGIN_TAB_PASSWORD_ENTER) {
            Keyboard.print(m.login);
            Keyboard.press(KEY_TAB);
            Keyboard.release(KEY_TAB);
            Keyboard.print(m.password);
        }

        if (m.mode == SCENARIO_MODE_PASSWORD_ENTER || m.mode == SCENARIO_MODE_LOGIN_TAB_PASSWORD_ENTER){
            Keyboard.press(KEY_RETURN);
            Keyboard.release(KEY_RETURN);
        }
    }
}

// Обработка пользовательских заметок >> Здесь <<
void GoCustomProcessor(const MenuItem m) {
    if (strcasecmp(m.login, "P1") == 0) {
        PressEscapeEveryMinute();
    }
}


// Отобразить форму авторизации и выполнить проверку на количество попыток
#ifdef COMPILE_ENABLE_PIN
void GoAuthLoop() {
    uint8_t failedAttempt, lastAttempt;
    bool passed = false;

    char buffer[5];
    GetStringPgm(master_password_pgm, buffer);
    
    do {
        // Если меньше N-попыток
        GetSettingAttempt(failedAttempt);
        
        // Проверка авторизации, если менее N-попыток
        if (failedAttempt < PIN_MAX_ATTEMPT) {
            passed = GoCheckPassword(buffer);

            // Если неуспешная попытка
            if (!passed) {
                failedAttempt++;
            } else {
                failedAttempt = 0;
            }

            // Сохраняем использованные попытки. Если ввели верно сразу и прошлые попытки не использованы, ничего не сохраняем
            GetSettingAttempt(lastAttempt);

            if (failedAttempt > 0 || (failedAttempt == 0 && lastAttempt > 0))
                SetSettingAttempt(failedAttempt);

            // Выходим, если прошли проверку
            if (passed)
                break;

        } else {

            char bufferTitle[MAX_TITLE];
            DrawMenuMessage(GetStringPgm(data_auth_error_pgm, bufferTitle));
            
            // Блокируем на N секунд
            delayMillis(PIN_WRONG_DELAY * 1000);

            // Восстанавливаем попытки
            SetSettingAttempt(0);
        }
        
    } while (true);
}


// Перейти в цикл авторизации
bool GoCheckPassword(const char masterPassword[5]) {
    
    // Выставляем курсоры для перебора цифр
    ChangeViewMode(MENU_TYPE_AUTH);

    uint8_t numberPosition = 0;
    const char zero = '0';

    char userPassword[5];
    userPassword[0] = userPassword[1] = userPassword[2] = userPassword[3] = zero;
    userPassword[4] = '\0';

    DrawMenuAuth(userPassword, numberPosition, true);
    
    while (true) {
        if (encoder.tick() != 0) {
            
            if (encoder.turn()) {
                if (encoder.right())
                    GoCursorUp();
                
                if (encoder.left())
                    GoCursorDown();

                if (numberPosition < 4)
                    userPassword[numberPosition] = GlobalCursorY + zero;
            }

            // На следующее число
            if (encoder.click()){
                if (numberPosition < 4) numberPosition++;

                GlobalCursorY = userPassword[numberPosition] - zero;
            }
            
            // Возврат на предыдущее число
            if (encoder.hold()){
                if (numberPosition > 0) numberPosition--;

                GlobalCursorY = userPassword[numberPosition] - zero;
            }

            // Курсор позиций ушёл за последний элемент [рабочие индексы 0, 1, 2, 3], индекс 4 для символа нуля, но мы на нём проверяем правильность ввода пароля
            if (numberPosition >= 4)
                return (strcmp(userPassword, masterPassword) == 0);

            DrawMenuAuth(userPassword, numberPosition, true); // Рисуем
        }
    }
}
#endif

// Метод ожидания
void delayMillis(uint32_t ns) {
    uint32_t tmr = millis();
    while (millis() - tmr < ns);
}


