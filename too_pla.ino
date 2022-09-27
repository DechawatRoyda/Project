#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <Servo.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "iPhone acer"
#define WIFI_PASSWORD "G12345678"

#define API_KEY "AIzaSyAcl9_kUjG2XPN3klFL1BSnzJ3lhgYHWD4"
#define FIREBASE_URL "https://fishcare1-20855-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_KEY "nsEO9G9MHYq9acLhSBrVpVvAdQm6QOgtBhKimC0t"

Servo servo;
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
int pump_in , pump_out ,led ,serVo ,AUTO;
int buttonState;
int H_on1, M_on1, H_on2, M_on2, H_on3, M_on3,H_off3, M_off3;
int timezone = 7 * 3600;
int dst = 0;
byte time_h, time_m;

void setup() {
  //------------------Wifi ------------------
  connectWifi();//void เรียกใช้ไวไฟเอา
 
  servo.attach(5); //D1 จองแล้ว
  servo.write(0);
  
  //------------------firebase ------------------
  config.api_key = API_KEY;
   config.database_url = FIREBASE_URL;
   if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
   config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
 Firebase.begin(&config, &auth);
 Firebase.reconnectWiFi(true);
  //------------------เวลา--------------------------
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for Internet time");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTime response....OK");

}

void loop() {
  delay(1000);
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  time_h = p_tm->tm_hour;
  time_m = p_tm->tm_min;
 

  
  Firebase.RTDB.setInt(&firebaseData, "/STATUSNODE/FOOD", time_m);
  Firebase.RTDB.getInt(&firebaseData, "/FISH/SERVO",&serVo);


   //--------------------SERVO------------
  Serial.print("SERVO : ");
  Serial.println(serVo);

 
    if(serVo == 1)
    {
      servo.write(0);
      delay(1000);
      servo.write(180);
      delay(1000);
      Firebase.RTDB.setInt(&firebaseData, "/FISH/SERVO", 0);
    }
}
void connectWifi() { //ไวไฟโค้ด
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}
