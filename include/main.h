#pragma once

#define PLATFORM_RESET() asm("JMP 0")
#define EB_NO_FOR           // отключить поддержку pressFor/holdFor/stepFor и счётчик степов (экономит 2 байта оперативки)
#define EB_NO_CALLBACK      // отключить обработчик событий attach (экономит 2 байта оперативки)
#define EB_NO_COUNTER       // отключить счётчик энкодера (экономит 4 байта оперативки)
//#define EB_NO_BUFFER        // отключить буферизацию энкодера (экономит 1 байт оперативки)

//#define MORE_MENU_ITEMS

#include <Arduino.h>
#include <Keyboard.h>
#include <EncButton.h>
#include "display.h"
//#include "MemoryFree.h"

// Управление курсором
void GoCursorUp();
void GoCursorDown();

// Навигация по папкам
void GoFolderRoot();
void GoFolderSelectedItem();
void GoFolderTo(const MenuItem m);

// Форма запроса авторизации и цикл обработки ошибок
bool GoCheckPassword(const char masterPassword[5]);
void GoAuthLoop();

// Обработка заметок
void GoAutoProcessor(const MenuItem m, uint8_t step = 1);
void GoCustomProcessor(const MenuItem m);

// Вспомогательные функции
void delayMillis(uint32_t ns);