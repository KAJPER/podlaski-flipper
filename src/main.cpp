#include <Arduino.h>
#include "modules/ble_spam.h"
#include "modules/buttons.h"
#include "modules/display.h"

// Inicjalizacja wyświetlacza
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Zmienne menu
int currentMenuIndex = 0; // Indeks aktualnego elementu menu
int currentMenuLevel = 0; // 0: Główne menu, 1: Podmenu `ble_spam`
const int mainMenuItemsCount = 1; // Liczba opcji w głównym menu
const int bleSpamMenuItemsCount = 5; // Liczba opcji w menu `ble_spam`

// Elementy głównego menu
const char* mainMenuItems[] = {
    "BLE Spam"
};

// Elementy podmenu `ble_spam`
const char* bleSpamMenuItems[] = {
    "iOS Spam",
    "SwiftPair",
    "Samsung",
    "Android",
    "Spam All"
};

// Wyświetlanie menu
void displayMenu() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);

    if (currentMenuLevel == 0) {
        // Główne menu
        int16_t width = u8g2.getStrWidth(mainMenuItems[currentMenuIndex]);
        int16_t x = (128 - width) / 2; // Wycentrowanie tekstu
        u8g2.drawStr(x, 35, mainMenuItems[currentMenuIndex]);
    } else if (currentMenuLevel == 1) {
        // Podmenu `ble_spam`
        int16_t width = u8g2.getStrWidth(bleSpamMenuItems[currentMenuIndex]);
        int16_t x = (128 - width) / 2; // Wycentrowanie tekstu
        u8g2.drawStr(x, 35, bleSpamMenuItems[currentMenuIndex]);
    }

    // Strzałki nawigacyjne
    if (currentMenuIndex > 0) {
        u8g2.drawStr(5, 35, "<");
    }
    if ((currentMenuLevel == 0 && currentMenuIndex < mainMenuItemsCount - 1) ||
        (currentMenuLevel == 1 && currentMenuIndex < bleSpamMenuItemsCount - 1)) {
        u8g2.drawStr(120, 35, ">");
    }

    u8g2.sendBuffer();
}

// Obsługa wyboru w menu
void handleMenuSelection() {
    if (currentMenuLevel == 0) {
        // Główne menu
        if (currentMenuIndex == 0) {
            // Wejście do podmenu `ble_spam`
            currentMenuLevel = 1;
            currentMenuIndex = 0;
        }
    } else if (currentMenuLevel == 1) {
        // Podmenu `ble_spam`
        aj_adv(currentMenuIndex); // Wywołanie funkcji BLE
    }
}

// Konfiguracja początkowa
void setup() {
    // Inicjalizacja wyświetlacza
    u8g2.begin();

    // Konfiguracja pinów przycisków
    pinMode(buttonLeftPin, INPUT_PULLUP);
    pinMode(buttonRightPin, INPUT_PULLUP);
    pinMode(buttonSelectPin, INPUT_PULLUP);
    pinMode(buttonBackPin, INPUT_PULLUP);

    // Wyświetlenie menu początkowego
    displayMenu();
}

void loop() {
    // Nawigacja w menu
    if (digitalRead(buttonLeftPin) == LOW) {
        if (currentMenuIndex > 0) {
            currentMenuIndex--;
            displayMenu();
        }
        delay(200); // Debounce
    }
    if (digitalRead(buttonRightPin) == LOW) {
        if ((currentMenuLevel == 0 && currentMenuIndex < mainMenuItemsCount - 1) ||
            (currentMenuLevel == 1 && currentMenuIndex < bleSpamMenuItemsCount - 1)) {
            currentMenuIndex++;
            displayMenu();
        }
        delay(200); // Debounce
    }
    if (digitalRead(buttonSelectPin) == LOW) {
        handleMenuSelection();
        displayMenu(); // Aktualizacja menu po wyborze
        delay(200); // Debounce
    }
    if (digitalRead(buttonBackPin) == LOW) {
        if (currentMenuLevel > 0) {
            // Powrót do poprzedniego poziomu menu
            currentMenuLevel--;
            currentMenuIndex = 0;
            displayMenu();
        }
        delay(200); // Debounce
    }
}
