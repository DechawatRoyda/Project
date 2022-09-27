#include "esp_camera.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <TridentTD_LineNotify.h>
#include "esp_system.h"
hw_timer_t *timer = NULL;
void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    esp_restart();
}


//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

const char* ssid = "NOPADON_2.4GHz";
const char* password = "12345678";
#define FIREBASE_HOST "asixiot-default-rtdb.firebaseio.com"
#define FIREBASE_KEY "QsvJIcK5rgRVfJ1hWuaIXOumz5nUbB1Bg9c8w6Uv"
#define LINE_TOKEN  "bS6fEmyt3dbUPn4a6zZ4TXVn2zLluqcYUSTrCMNAAfn"   

void startCameraServer();
FirebaseData firebaseData;
const int Led_Flash = 4;
const int Led_run = 13;
int PIR_Sensor = 12;
boolean startTimer = false;
unsigned long time_now=0;
int time_capture=0;
byte camera;

void setup() {
  Serial.begin(115200);
     while (!Serial) {  ;  }
  pinMode(Led_Flash, OUTPUT);
  pinMode(Led_run, OUTPUT);
  Serial.setDebugOutput(true);
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY); 
  LINE.setToken(LINE_TOKEN);

    timer = timerBegin(0, 80, true); //timer 0, div 80Mhz 
    timerAttachInterrupt(timer, &resetModule, true);
    timerAlarmWrite(timer, 20000000, false); //set time in us 15s
    timerAlarmEnable(timer); //enable interrupt

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  

    
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void Camera_capture() {
  digitalWrite(Led_Flash, HIGH);
  delay(100); 
  digitalWrite(Led_Flash, LOW);
  delay(100);
  digitalWrite(Led_Flash, HIGH);
  camera_fb_t * fb = NULL;
  delay(200); 
  // Take Picture with Camera
  fb = esp_camera_fb_get(); 
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
   digitalWrite(Led_Flash, LOW);
   Send_line(fb->buf,fb->len);
   esp_camera_fb_return(fb); 
  // Serial.println("Going to sleep now");
  // esp_deep_sleep_start();
  // Serial.println("This will never be printed");

}

void Send_line(uint8_t *image_data,size_t   image_size){
   LINE.notifyPicture("ถ่ายภาพ",image_data, image_size);
  }
  
void loop() {timerWrite(timer, 0); //reset timer (feed watchdog)
    long tme = millis();

    if (Firebase.getInt(firebaseData,"/STATUS/CAMERA")) {
    if (firebaseData.dataType() == "int") {
      Serial.println(firebaseData.intData());
      camera = firebaseData.intData();
    }
  }
  
    if(camera == 1 && startTimer != true){    
           Camera_capture();
           Serial.println("OK");
           Serial.print(camera);
           startTimer = true; 
           delay(1000);
           Firebase.setInt(firebaseData, "/STATUS/CAMERA",0);
           delay(1000); 
      }else {
        Serial.print(camera);
        startTimer = false;   
        time_capture=0;
        delay(3000);  
      }

     if(millis() > time_now + 1000) {
           time_now = millis();
           digitalWrite(Led_run, HIGH);
            delay(20);  
           digitalWrite(Led_run, LOW); 
          }
         tme = millis() - tme;
      if(camera == 1){
         if(++time_capture > 60){
            time_capture=0;
            Camera_capture();
            Serial.println("Over Time");
          }
  delay(10000);
}
}
