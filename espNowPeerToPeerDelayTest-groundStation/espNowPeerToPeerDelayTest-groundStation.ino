/*
Simulated ground station
Sends response request to simulated bot,
then prints round-trip response time to serial monitor
*/

//Required libraries
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>

long timeStart;
long timeEnd;
long responseTime;
char request[40];
const int NUM_PIXELS = 1; //Board has a single onboard NeoPIxel

Adafruit_NeoPixel pixels_(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);


// Bot MAC address
uint8_t broadcastAddress[] = {0xF4, 0x12, 0xFA, 0x59, 0x5E, 0x24};

esp_now_peer_info_t peerInfo;


// Response received callback function
void onReceive(const uint8_t * mac, const uint8_t *incomingResponse, int len) {
  timeEnd = micros();
  timeStart = strtol((char*)incomingResponse, NULL, 10);
  responseTime = timeEnd - timeStart;


  //Print response time to serial monitor
  Serial.println(responseTime);

}

void terminalErrorBlink() {
  while (true){
  pixels_.fill(pixels_.Color(255, 0, 0));
  pixels_.show();
  delay (100);
  pixels_.fill(pixels_.Color(0, 0, 0));
  pixels_.show();
  delay (100);
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 
  if (status != ESP_NOW_SEND_SUCCESS){
    Serial.println("nodata");
  }
}

void setup() {
  #if defined(NEOPIXEL_POWER)
    // If this board has a power control pin, we must set it to output and high
    // in order to enable the NeoPixels_-> We put this in an #if defined so it can
    // be reused for other boards without compilation errors
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

    pixels_.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
    pixels_.setBrightness(100); // not so bright
  
  // Set up Serial Monitor
  pixels_.fill(pixels_.Color(0, 255, 0));
  pixels_.show();
  Serial.begin(115200);
  while (!Serial);  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    terminalErrorBlink();
  }
  
  // Register receiving callback function
  esp_now_register_recv_cb(esp_now_recv_cb_t(onReceive));
  // Register sending callback function
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    terminalErrorBlink();
  }
  delay(100); //100ms delay to allow serial monitor to start

}

void loop() {
  //Send response request in the form of the start time as a string
  itoa(micros(),request,10);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &request, sizeof(request));
   if (result != ESP_OK) {
    Serial.println("nodata");
  }
  //delay to space out response requests
  delay(10);
}