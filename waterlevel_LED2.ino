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

#define LED D0
#define PUMPOUT D1
#define PUMPIN D2
#define WATERLEVEL D3


int timezone = 7 * 3600;
int dst = 0;
byte time_h, time_m;
int buttonState;
int  pumpin, pumpout, AUTO , change , led , x , date , ledon_H, ledon_M, ledoff_H, ledoff_M ,preoid;

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
void setup() {

  connectWifi();//void เรียกใช้ไวไฟเอา
  pinMode(LED, OUTPUT);
  pinMode(PUMPIN, OUTPUT);
  pinMode(PUMPOUT, OUTPUT);
  pinMode(WATERLEVEL, INPUT);


  //------------------firebase ------------------
  config.api_key = API_KEY;
  config.database_url = FIREBASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
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
  buttonState = digitalRead(WATERLEVEL);
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  time_h = p_tm->tm_hour;
  time_m = p_tm->tm_min;

  Firebase.RTDB.setInt(&firebaseData, "/STATUSNODE/MAIN", time_m);

  Firebase.RTDB.getInt(&firebaseData,"/FISH/PUMPIN",&pumpin);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/PUMPOUT",&pumpout);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/CHANGE",&change);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/LED",&led);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/DLAY",&x);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/DATE",&date);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/LEDONH",&ledon_H);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/LEDONM",&ledon_M);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/LEDOFFH",&ledoff_H);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/LEDOFFM",&ledoff_M);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/STATUS",&AUTO);
  Firebase.RTDB.getInt(&firebaseData,"/FISH/PREOID",&preoid);

  Serial.print("--------------");
  Serial.print("h : ");
  Serial.print(p_tm->tm_hour);
  Serial.print(" m : ");
  Serial.print(p_tm->tm_min);
  Serial.println("--------------");
  Serial.print("PUMP_IN : ");
  Serial.println(pumpin);
  Serial.print("PUMP_OUT : ");
  Serial.println(pumpout);
  Serial.print("LED : ");
  Serial.println(led);
  Serial.print("/FISH/AUTO : ");
  Serial.println(AUTO);

  if (led == 0)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }
  //-------------------PUMP_IN------------------------------
  if (pumpin == 0)
  {
    digitalWrite(PUMPIN, HIGH);
  }
  else
  {
    digitalWrite(PUMPIN, LOW);
  }
  //-------------------PUMP_OUT------------------------------
  if (pumpout == 0)
  {
    digitalWrite(PUMPOUT, HIGH);
  }
  else
  {
    digitalWrite(PUMPOUT, LOW);
  }
  //-------------------PUMP------------------------------//เป็นระบบถ่ายน้ำ กะเวลาถ่ายน้ำออก แล้วเติมน้ำเข้าจนกว่าเซนเซอร์จะขึ้นว่า low แต่อุปกรณณ์จะ HIGH
  if (change == 1)   // ทำให้ set ค่าเวลาเปิดปิดกี่โมงได้ได้ และกดเปิดปิดเองได้ด้วย
  {
    Firebase.RTDB.setInt(&firebaseData, "/FISH/PUMPOUT", 1);
    digitalWrite(PUMPOUT, LOW);
     if (buttonState == LOW)
    {
      Serial.println("Status Water lavel : LOW");
      Firebase.RTDB.setInt(&firebaseData, "/FISH/PUMPOUT", 0);
      Firebase.RTDB.setInt(&firebaseData, "/FISH/CHANGE", 2);
    }
  }
  if (change == 2)
  {
    Firebase.RTDB.setInt(&firebaseData, "/FISH/PUMPIN", 1);
    digitalWrite(PUMPIN, LOW);
  }
  if (date == preoid)
  {
    Firebase.RTDB.setInt(&firebaseData, "/FISH/CHANGE", 1);
    Firebase.RTDB.setInt(&firebaseData, "/FISH/DATE", 0);
  }
  
  if(AUTO == 0){
  if (time_h == ledon_H && time_m == ledon_M) {
    Firebase.RTDB.setInt(&firebaseData, "/FISH/LED", 1);
    digitalWrite(LED, LOW);
  }
  if (time_h == ledoff_H && time_m == ledoff_M) {
    Firebase.RTDB.setInt(&firebaseData, "/FISH/LED", 0);
    digitalWrite(LED, HIGH);
  }
  }
        if (buttonState == LOW)
    {
      Serial.println("Status Water lavel : LOW");
      Firebase.RTDB.setInt(&firebaseData, "/FISH/PUMPOUT", 0);
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
