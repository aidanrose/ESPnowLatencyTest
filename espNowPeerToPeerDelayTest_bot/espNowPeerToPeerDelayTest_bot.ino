/*
Simulated bot
Sends response to ground station following response request,
*/

//Required libraries
#include <WiFi.h>
#include <esp_now.h>

char receivedRequest[40];

// Ground station MAC address
uint8_t broadcastAddress[] = {0xF4, 0x12, 0xFA, 0x59, 0x60, 0x88};

esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void onResponseSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

}

// Response received callback function
void onReceive(const uint8_t * mac, const uint8_t *incomingRequest, int len) {
  memcpy(&receivedRequest, incomingRequest, sizeof(receivedRequest));

  // Send requested response via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &receivedRequest, sizeof(receivedRequest));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  delay(100);  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register sending callback function
  esp_now_register_send_cb(onResponseSend);

  // Register receiving callback function
  esp_now_register_recv_cb(esp_now_recv_cb_t(onReceive));

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}

void loop() {
  
}