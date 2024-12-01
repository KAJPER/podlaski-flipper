#include <Arduino.h>
#include "modules/buttons.h"
#include "modules/display.h"
#include "modules/ble_spam.h"
#include "modules/wifi_attacks.h"

// Inicjalizacja wyświetlacza
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Zmienne menu
int currentMenuIndex = 0;
int currentSubMenuIndex = 0;
enum MenuType { MAIN_MENU, BLE_MENU, WIFI_MENU };
MenuType currentMenu = MAIN_MENU;

// Elementy menu głównego
const char* mainMenuItems[] = {
    "BLE Spam",
    "Deauth Attack",
    "Beacon Spam",
    "Packet Sniffer",
    "Exit"
};
const int mainMenuItemsCount = 5;

// Elementy podmenu BLE
const char* bleMenuItems[] = {
    "iOS Spam",
    "SwiftPair",
    "Samsung",
    "Android",
    "Spam All",
    "Back"
};
const int bleMenuItemsCount = 6;

void displayMenu() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    
    const char* title;
    const char** items;
    int itemCount;
    int currentIndex;
    
    switch(currentMenu) {
        case MAIN_MENU:
            title = "Main Menu";
            items = mainMenuItems;
            itemCount = mainMenuItemsCount;
            currentIndex = currentMenuIndex;
            break;
        case BLE_MENU:
            title = "BLE Spam";
            items = bleMenuItems;
            itemCount = bleMenuItemsCount;
            currentIndex = currentSubMenuIndex;
            break;
        default:
            return;
    }
    
    // Tytuł
    u8g2.drawStr(0, 10, title);
    u8g2.drawHLine(0, 12, 128);
    
    // Elementy menu
    int startY = 25;
    int itemHeight = 12;
    int maxItems = 4;
    
    int startItem = max(0, currentIndex - maxItems + 1);
    startItem = min(startItem, max(0, itemCount - maxItems));
    
    for(int i = 0; i < min(maxItems, itemCount); i++) {
        int itemIndex = startItem + i;
        if(itemIndex == currentIndex) {
            u8g2.drawBox(0, startY + i*itemHeight - 9, 128, 11);
            u8g2.setDrawColor(0);
        }
        u8g2.drawStr(2, startY + i*itemHeight, items[itemIndex]);
        u8g2.setDrawColor(1);
    }
    
    u8g2.sendBuffer();
}

void handleMenuSelection() {
    if(currentMenu == MAIN_MENU) {
        switch(currentMenuIndex) {
            case 0: // BLE Spam
                currentMenu = BLE_MENU;
                currentSubMenuIndex = 0;
                break;
                
            case 1: // Deauth Attack
                {
                    auto networks = scanNetworks();
                    int selectedNetwork = 0;
                    bool selecting = true;
                    
                    while(selecting) {
                        u8g2.clearBuffer();
                        u8g2.setFont(u8g2_font_6x10_tf);
                        
                        for(int i = 0; i < min(4, (int)networks.size()); i++) {
                            if(i == selectedNetwork) {
                                u8g2.drawBox(0, i*16, 128, 15);
                                u8g2.setDrawColor(0);
                            }
                            u8g2.drawStr(2, (i+1)*16-4, networks[i].ssid.c_str());
                            u8g2.setDrawColor(1);
                        }
                        u8g2.sendBuffer();
                        
                        if(digitalRead(buttonLeftPin) == LOW && selectedNetwork > 0) {
                            selectedNetwork--;
                            delay(200);
                        }
                        if(digitalRead(buttonRightPin) == LOW && selectedNetwork < networks.size()-1) {
                            selectedNetwork++;
                            delay(200);
                        }
                        if(digitalRead(buttonSelectPin) == LOW) {
                            startDeauthAttack(networks[selectedNetwork]);
                            selecting = false;
                            delay(200);
                        }
                        if(digitalRead(buttonBackPin) == LOW) {
                            selecting = false;
                            delay(200);
                        }
                    }
                }
                break;
                
            case 2: // Beacon Spam
                {
                    int numNetworks = 10;
                    bool configuring = true;
                    
                    while(configuring) {
                        u8g2.clearBuffer();
                        u8g2.setFont(u8g2_font_6x10_tf);
                        u8g2.drawStr(2, 12, ("Networks: " + String(numNetworks)).c_str());
                        u8g2.sendBuffer();
                        
                        if(digitalRead(buttonLeftPin) == LOW && numNetworks > 1) {
                            numNetworks--;
                            delay(200);
                        }
                        if(digitalRead(buttonRightPin) == LOW && numNetworks < 100) {
                            numNetworks++;
                            delay(200);
                        }
                        if(digitalRead(buttonSelectPin) == LOW) {
                            startBeaconSpam("KINDZIUK", numNetworks);
                            configuring = false;
                            delay(200);
                        }
                        if(digitalRead(buttonBackPin) == LOW) {
                            configuring = false;
                            delay(200);
                        }
                    }
                }
                break;
                
            case 3: // Packet Sniffer
                {
                    auto networks = scanNetworks();
                    int selectedNetwork = 0;
                    bool selecting = true;
                    
                    while(selecting) {
                        u8g2.clearBuffer();
                        u8g2.setFont(u8g2_font_6x10_tf);
                        
                        for(int i = 0; i < min(4, (int)networks.size()); i++) {
                            if(i == selectedNetwork) {
                                u8g2.drawBox(0, i*16, 128, 15);
                                u8g2.setDrawColor(0);
                            }
                            u8g2.drawStr(2, (i+1)*16-4, networks[i].ssid.c_str());
                            u8g2.setDrawColor(1);
                        }
                        u8g2.sendBuffer();
                        
                        if(digitalRead(buttonLeftPin) == LOW && selectedNetwork > 0) {
                            selectedNetwork--;
                            delay(200);
                        }
                        if(digitalRead(buttonRightPin) == LOW && selectedNetwork < networks.size()-1) {
                            selectedNetwork++;
                            delay(200);
                        }
                        if(digitalRead(buttonSelectPin) == LOW) {
                            startPacketSniffing(networks[selectedNetwork]);
                            selecting = false;
                            delay(200);
                        }
                        if(digitalRead(buttonBackPin) == LOW) {
                            selecting = false;
                            delay(200);
                        }
                    }
                }
                break;
                
            case 4: // Exit
                ESP.restart();
                break;
        }
    } else if(currentMenu == BLE_MENU) {
        if(currentSubMenuIndex == bleMenuItemsCount - 1) { // Back
            currentMenu = MAIN_MENU;
            currentMenuIndex = 0;
        } else {
            aj_adv(currentSubMenuIndex);
        }
    }
}

void setup() {
    // Inicjalizacja wyświetlacza
    u8g2.begin();
    
    // Inicjalizacja przycisków
    pinMode(buttonLeftPin, INPUT_PULLUP);
    pinMode(buttonRightPin, INPUT_PULLUP);
    pinMode(buttonSelectPin, INPUT_PULLUP);
    pinMode(buttonBackPin, INPUT_PULLUP);
    
    // Inicjalizacja WiFi
    initWiFiAttacks();
    
    displayMenu();
}

void loop() {
    if(digitalRead(buttonLeftPin) == LOW) {
        if(currentMenu == MAIN_MENU && currentMenuIndex > 0) {
            currentMenuIndex--;
        } else if(currentMenu == BLE_MENU && currentSubMenuIndex > 0) {
            currentSubMenuIndex--;
        }
        displayMenu();
        delay(200);
    }
    
    if(digitalRead(buttonRightPin) == LOW) {
        if(currentMenu == MAIN_MENU && currentMenuIndex < mainMenuItemsCount - 1) {
            currentMenuIndex++;
        } else if(currentMenu == BLE_MENU && currentSubMenuIndex < bleMenuItemsCount - 1) {
            currentSubMenuIndex++;
        }
        displayMenu();
        delay(200);
    }
    
    if(digitalRead(buttonSelectPin) == LOW) {
        handleMenuSelection();
        displayMenu();
        delay(200);
    }
    
    if(digitalRead(buttonBackPin) == LOW) {
        if(currentMenu != MAIN_MENU) {
            currentMenu = MAIN_MENU;
            currentMenuIndex = 0;
            displayMenu();
        }
        delay(200);
    }
}