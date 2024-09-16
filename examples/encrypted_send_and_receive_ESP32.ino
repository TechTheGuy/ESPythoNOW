#include <esp_now.h>
#include "esp_private/wifi.h"
#include <WiFi.h>

static const char* PMK_KEY = "0u4hgz7pgct3gnv8"; // Do not leave. Example PMK
static const char* LMK_KEY = "a3o4csuv2bpvr0wu"; // Do not leave. Example LMK

typedef struct __attribute__ ((packed)) python_message {
  int a = 567;
  bool b = false;
  uint8_t c[22];
} python_message;
python_message pm;

esp_now_peer_info_t peer_info;
uint8_t python_peer[] = {0xE0,0x5A,0x1B,0x11,0x22,0x33};
uint8_t local_mac[] = {0xE0,0x5A,0x1B,0x33,0x22,0x11}; // Override local MAC

void esp_now_rx_callback(const esp_now_recv_info_t * info, const uint8_t *data, int len) {
  memcpy(&pm, data, sizeof(pm));
  Serial.println(pm.a);
  Serial.println(pm.b);
  Serial.write(pm.c, 22);
  Serial.println();
 }

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(8, WIFI_SECOND_CHAN_NONE);
  esp_now_init();
  esp_wifi_set_mac(WIFI_IF_STA, local_mac);
  esp_now_set_pmk((uint8_t *)PMK_KEY);
  memcpy(peer_info.lmk, LMK_KEY, 16);
  memcpy(peer_info.peer_addr, python_peer, 6);
  peer_info.channel = 0; // 0 defaults to existing channel setting
  peer_info.encrypt = true;
  esp_now_add_peer(&peer_info);
  esp_now_register_recv_cb(esp_now_recv_cb_t(esp_now_rx_callback));
}

void loop() {
  pm.a = 567;
  pm.b = false;
  memcpy(pm.c, "Hello from ESP device!", 22);
  esp_now_send(python_peer, (uint8_t *) &pm, sizeof(pm));
  delay(3000);
}
