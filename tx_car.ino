
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <MPU6050.h>
 int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int right,left,backward,forward,center;
  MPU6050 mpu;
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x9C, 0x9C, 0x1F, 0x47, 0x38, 0xF3};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int right,left,forward,backward,center;

} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  
  mpu.initialize();
  
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);//ESP_NOW_ROLE_CONTROLLER, ESP_NOW_ROLE_SLAVE, ESP_NOW_ROLE_COMBO, ESP_NOW_ROLE_MAX.
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer for pairing another nodemcu
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // Set values to send
  
    //center
   if((az>17000 and az<21000) ){
       center=1;
   }
   else{
    center=0;
   }

   //right 
   if((az>-10000 and az<1000)  ){
    right=1;
   }
   else{
    right=0;
   }
    // forward
   if((az>-2000 and az<8000) ){
    forward=1;
   }
   else {
    forward=0;
   }
  // left
  if((az>-1000 and az<10000) and  (ay>-2000 and ay<2000) and (ax>-17000 and ax<-14000)){
    left=1;
    right=0;
    forward=0;
   }
   else {
    left=0;
   }
    //backward
   if((az>-2000 and az<5000) and (ay>-17000 and ay<-13000) and (ax>-5000 and ax<0)){
    backward=1;
    forward=0;
   }
   else {
    backward=0;
   }
   //for sending to rx
   if(center==1){
     myData.center=1;
   }
   else{
    myData.center=0;
   }
    if(right==1){
     myData.right=1;
   }
   else{
    myData.right=0;
   }
    if(left==1){
     myData.left=1;
   }
   else{
    myData.left=0;
   }
    if(backward==1){
     myData.backward=1;
   }
   else{
    myData.backward=0;
   }
    if(forward==1){
     myData.forward=1;
   }
   else{
    myData.forward=0;
   }
   
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
Serial.print("center");
Serial.println(center);
Serial.print("right");
Serial.println(right);
Serial.print("left");
Serial.println(left);
Serial.print("backward");
Serial.println(backward);
Serial.print("forward");
Serial.println(forward);
delay(500);
    // Print data to Serial Monitor
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(ax);
  Serial.print(" | Y = "); Serial.print(ay);
  Serial.print(" | Z = "); Serial.println(az);
  
}
