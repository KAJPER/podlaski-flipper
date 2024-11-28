#ifndef WIFI_ATTACKS_H
#define WIFI_ATTACKS_H

#include <Arduino.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <vector>

typedef struct {
    uint8_t frame_ctrl[2];
    uint8_t duration[2];
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint8_t seq_ctrl[2];
} wifi_ieee80211_mac_hdr_t;

typedef struct {
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0];
} wifi_ieee80211_packet_t;

typedef struct {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
} wifi_pkt_rx_ctrl_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t payload[0];
    uint16_t len;
    uint16_t ampdu_info[128];
} wifi_promiscuous_pkt_t;

struct NetworkInfo {
    String ssid;
    uint8_t bssid[6];
    int channel;
    int rssi;
};

// Główne funkcje
void initWiFiAttacks();
void startDeauthAttack(NetworkInfo target, int packets = 100);
void startBeaconSpam(String baseSSID = "KINDZIUK", int numNetworks = 10);
void startPacketSniffing(NetworkInfo target);
std::vector<NetworkInfo> scanNetworks();
void stopCurrentAttack();

#endif 