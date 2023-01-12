#include <SPI.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <EasyBuzzer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Port
#define RELAY_PIN 15
#define BUZZZER_PIN  19 // ESP32 pin GIOP18 connected to piezo buzzer

#define timeSeconds 5

// Insert Firebase project API Key
#define API_KEY "AIzaSyCddbm3Qwj1vQWEGyXEOjFK_CfqZx07Rvs"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://pengusir-burung-pmld-default-rtdb.asia-southeast1.firebasedatabase.app/"

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// WIFI Config
const char* ssid = "ZeroDev";
const char* password = "123456789";
const char* hostname = "ESP-1";

// Motion Sensor
const int motionSensor = 18;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

// Timer: Auxiliary trigger
unsigned long nowTrigger = millis();
unsigned long lastTimeTrigger = 0;
boolean startTimerTrigger = false;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String dateTime;

String idBoard = "Tower1";

//Offline Mode
int timeNow = 0;
int timeOut = 20;
boolean offlineMode = false;

FirebaseJson jsonSend;

unsigned char wifi1_icon4[] =
{
  0b00000000, 0b00000000, //                 
  0b00000111, 0b11100000, //      ######     
  0b00011111, 0b11111000, //    ##########   
  0b00111111, 0b11111100, //   ############  
  0b01110000, 0b00001110, //  ###        ### 
  0b01100111, 0b11100110, //  ##  ######  ## 
  0b00001111, 0b11110000, //     ########    
  0b00011000, 0b00011000, //    ##      ##   
  0b00000011, 0b11000000, //       ####      
  0b00000111, 0b11100000, //      ######     
  0b00000100, 0b00100000, //      #    #     
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
};

 unsigned char wifi1_icon3[] =
{
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //    
  0b00000000, 0b00000000, //    
  0b00000000, 0b00000000, //    
  0b00000000, 0b00000000, //  
  0b00000111, 0b11100000, //      ######  
  0b00001111, 0b11110000, //     ########    
  0b00011000, 0b00011000, //    ##      ##   
  0b00000011, 0b11000000, //       ####      
  0b00000111, 0b11100000, //      ######     
  0b00000100, 0b00100000, //      #    #     
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
};


 unsigned char wifi1_icon2[] =
{
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //          
  0b00000000, 0b00000000, //        
  0b00000000, 0b00000000, //      
  0b00000011, 0b11000000, //       ####      
  0b00000111, 0b11100000, //      ######     
  0b00000100, 0b00100000, //      #    #     
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
};

 unsigned char wifi1_icon1[] =
{
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //          
  0b00000000, 0b00000000, //        
  0b00000000, 0b00000000, //      
  0b00000000, 0b00000000, //             
  0b00000000, 0b00000000, //           
  0b00000000, 0b00000000, //          
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
};

 unsigned char wifi1_icon0[] =
{
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00000000, //                 
  0b00111000, 0b00001110, //   ###       ### 
  0b00111100, 0b00011110, //   ####     #### 
  0b00111110, 0b00111110, //   #####   ##### 
  0b00011111, 0b01111100, //    ##### #####  
  0b00001111, 0b11111000, //     #########   
  0b00000111, 0b11110000, //      #######    
  0b00000011, 0b11100000, //       #####     
  0b00000111, 0b11110000, //      #######    
  0b00001111, 0b11111000, //     #########   
  0b00011111, 0b01111100, //    ##### #####  
  0b00111110, 0b00111110, //   #####   ##### 
  0b00111100, 0b00011110, //   ####     #### 
  0b00111000, 0b00001110, //   ###       ### 
  0b00000000, 0b00000000, //                 
};

// Buzzer
int pinBuzzer = 19;
// End Buzzer

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  digitalWrite(RELAY_PIN, HIGH);
  display.clearDisplay();
  
  initWiFi();
  initMotion();
  EasyBuzzer.setPin(pinBuzzer);
}

void loop() {
    Serial.print("Loop\n");
    delay(100);
    display.clearDisplay();
    EasyBuzzer.update();
    showBarWifi();
    showMotionDetected();
    showClock();
    getTrigger();
    setStatusWifi();
    display.display();
}

// Setup WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,20);             
  display.println("Connecting to WiFi ..");
  display.display();
  offlineMode = false;
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    timeNow += 1;
    if (timeNow == timeOut) {
      break;
    }
    delay(1000);
  }
      if (WiFi.status() != WL_CONNECTED){
    offlineMode == true;
  }else {
    offlineMode == false;
    initFirebase();
  }
}

void setStatusWifi(){
  if (WiFi.status() != WL_CONNECTED){
    offlineMode == true;
  }else {
    offlineMode == false;
  }
}

// Setup Firebase
void initFirebase() {
  if(offlineMode == false) {
    /* Assign the api key (required) */
    config.api_key = API_KEY;
    
    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;
  
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0,20);             
    display.println("Connecting to Firebase ..");
    display.display();
  
    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")){
      Serial.println("ok");
      signupOK = true;
      Firebase.RTDB.setString(&fbdo, idBoard + "/name", idBoard);
      initNTP();
    }
    else{
      Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
  
    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    
    Firebase.begin(&config, &auth);
  }

}


// Show Bar Wifi
void showBarWifi() {
  if (WiFi.status() == WL_CONNECTED){
      if (WiFi.RSSI() >= -50){
      display.drawBitmap(0, 0, wifi1_icon4, 16, 16, 1);
      }else if ((WiFi.RSSI() < -50) &&  (WiFi.RSSI() > -70)){
        display.drawBitmap(0, 0, wifi1_icon3, 16, 16, 1);
      }else if ((WiFi.RSSI() < -70) &&  (WiFi.RSSI() > -80)){
        display.drawBitmap(0, 0, wifi1_icon2, 16, 16, 1);
      }else {
        display.drawBitmap(0, 0, wifi1_icon1, 16, 16, 1);
      }
      display.setCursor(18,4);             
      display.println(WiFi.localIP());
    }else{
      offlineMode = true;
      display.setCursor(4,4);             
      display.println("Offline Mode");
    }
}

boolean sendReq = false;

void showMotionDetected() {
  now = millis();
  
  display.setCursor(4,18);
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    sendReq = true;
    startTimer = false;
  }
  
  if(startTimer){
    display.println("Motion Detected!!");
    EasyBuzzer.beep(1200);
    if(sendReq){
      writeDetectionDB();  
    }
    sendReq = false;
  }else{
    display.println("Motion Stopped!!");
    EasyBuzzer.beep(0);
  }
}

// Motion
void IRAM_ATTR detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  startTimer = true;
  lastTrigger = millis();
}

// Setup Motion Sensor
void initMotion() {
    // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

// Play Buzzer
void playBuzzer() {
  EasyBuzzer.beep(1200);
}

void initNTP() {
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0,20);             
    display.println("Connecting to NTP ..");
    display.display();
  
    // Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +7
    timeClient.setTimeOffset(25200);
}

void showClock() {
  if (offlineMode == false){
       while(!timeClient.update()) {
        timeClient.forceUpdate();
      }
      // The formattedDate comes with the following format:
      int splitT = formattedDate.indexOf("T");
      formattedDate = timeClient.getFormattedDate();
      dayStamp = formattedDate.substring(0, splitT);
      timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
      dateTime = dayStamp + " " + timeStamp;
      display.setCursor(4,54);
      display.println(dayStamp + " " + timeStamp);
      
  }
}

// Write Realtime DB
void writeDetectionDB() {
  if (offlineMode == false){
    jsonSend.add("detected", "true");
    jsonSend.add("datetime", dateTime);
    if (Firebase.RTDB.pushJSON(&fbdo, idBoard + "/detected/", &jsonSend)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

// Get Trigger From Firebase
void getTrigger() {
  if (offlineMode == false){
    nowTrigger = millis();
    if(nowTrigger - lastTimeTrigger > (timeSeconds*1000)) {
        if (Firebase.RTDB.getBool(&fbdo, idBoard + "/trigger")) {
          if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_boolean) {
            if(fbdo.to<bool>() == true){
              Firebase.RTDB.setBool(&fbdo, idBoard + "/trigger", false);
              startTimer = true;
              lastTrigger = millis();
            }
          }
        } else {
          Serial.println(fbdo.errorReason());
        }
        lastTimeTrigger = millis();
    }
  }
  
}
