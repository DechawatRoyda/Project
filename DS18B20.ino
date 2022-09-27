
#include <ESP8266WiFi.h>
#include <time.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


#define WIFI_SSID "iPhone acer"
#define WIFI_PASSWORD "G12345678"

#define API_KEY "AIzaSyAcl9_kUjG2XPN3klFL1BSnzJ3lhgYHWD4"
#define FIREBASE_URL "https://fishcare1-20855-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_KEY "nsEO9G9MHYq9acLhSBrVpVvAdQm6QOgtBhKimC0t"

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D2 //กำหนดขาที่จะเชื่อมต่อ Sensor
#define WATERLEVEL D0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int timezone = 7 * 3600;
int dst = 0;
float Celsius;

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

byte time_h, time_m;
int buttonState;
int pumpin;
bool signupOK = false;
void setup() {
 connectWifi();//void เรียกใช้ไวไฟเอา
  pinMode(WATERLEVEL, INPUT);
  //Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library");
  sensors.begin();

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

void loop(void) {
  delay(1000);
  buttonState = digitalRead(WATERLEVEL);
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
    
  time_h = p_tm->tm_hour;
  time_m = p_tm->tm_min;
  
  Firebase.RTDB.setFloat(&firebaseData, "/FISH/TEMP_WATER", Celsius); //แสดงค่าอุณหภูมิ
  Firebase.RTDB.setFloat(&firebaseData, "/STATUSNODE/TEMP", time_m); //แสดงค่าอุณหภูมิ
  
  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures(); //อ่านข้อมูลจาก library
  Celsius = sensors.getTempCByIndex(0);
  Serial.print("Temperature is: ");
  Serial.print(Celsius); // แสดงค่า อูณหภูมิ
  Serial.println(" *C");
  Serial.println(pumpin);
  Serial.println(time_m);

      if (buttonState == HIGH)
    {
      Serial.println("Status Water lavel : HIGH");
      Firebase.RTDB.setInt(&firebaseData, "/FISH/PUMPIN", 0);
      Firebase.RTDB.setInt(&firebaseData, "/FISH/CHANGE", 0);
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
