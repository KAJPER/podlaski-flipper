#pragma once
#include <Arduino.h>
#include <vector>
#include "esp_wifi.h"
#include "esp_wifi_types.h"

struct NetworkInfo {
    String ssid;
    uint8_t bssid[6];
    int channel;
    int rssi;
};

// Deklaracje funkcji
void initWiFiAttacks();
std::vector<NetworkInfo> scanNetworks();
void startDeauthAttack(NetworkInfo target, int packets = 10);
void startBeaconSpam(String ssid, int num);
void startPacketSniffing(NetworkInfo target);
void stopCurrentAttack(); 