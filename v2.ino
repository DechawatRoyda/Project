#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <time.h>

#define WIFI_SSID "SEKSAN_2.4Ghz"
#define WIFI_PASSWORD "0813808670"

#define FIREBASE_HOST "asixiot-default-rtdb.firebaseio.com"
#define FIREBASE_KEY "QsvJIcK5rgRVfJ1hWuaIXOumz5nUbB1Bg9c8w6Uv"

#define PUMP D0
#define LED D1
#define Device1 D2
#define Device2 D3

FirebaseData firebaseData;
int led, pump, temp, humidity, AUTO, device1, device2 , device1mode , device2mode, LEDMODE;

int timezone = 7 * 3600;
int dst = 0;
int x, y, H_on1, M_on1, H_off1, M_off1, H_on2, M_on2, H_off2, M_off2, H_on3, M_on3, H_off3, M_off3, H_on4, M_on4, H_off4, M_off4, Max1, Min1, Max2, Min2;
byte time_h, time_m;
float Temp;

void setup() {
  //------------------Wifi ------------------
  connectWifi();//void เรียกใช้ไวไฟเอา
  pinMode(LED, OUTPUT);
  pinMode(Device1, OUTPUT);
  pinMode(Device2, OUTPUT);
  //------------------firebase ------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);

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

  Firebase.setInt(firebaseData, "/STATUS/TIME/HOUR", time_h); //เซ็ทค่าเวลาในอินเทอร์เน็ต...เผื่ออยากดูเวลา
  Firebase.setInt(firebaseData, "/STATUS/TIME/MIN", time_m);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_on1", H_on1);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_on2", H_on2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_on3", H_on3);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_on4", H_on4);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_on1", M_on1);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_on2", M_on2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_on3", M_on3);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_on4", M_on4);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_off1", H_off1);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_off2", H_off2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_off3", H_off3);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_off4", H_off4);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_off1", M_off1);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_off2", M_off2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_off3", M_off3);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_off4", M_off4);

  Firebase.getInt(firebaseData, "/FARM/LED", led);
  Firebase.getInt(firebaseData, "/FARM/device1", device1);
  Firebase.getInt(firebaseData, "/FARM/device2", device2);

  Firebase.getInt(firebaseData, "/FARM/LEDMODE", LEDMODE);
  Firebase.getInt(firebaseData, "/FARM/device1mode", device1mode);
  Firebase.getInt(firebaseData, "/FARM/device2mode", device2mode);

  Firebase.getFloat(firebaseData, "/STATUS/DHT11(1)/Temp2", Temp);
  Firebase.getInt(firebaseData, "/STATUS/TEMPSET/MAX1", Max1);
  Firebase.getInt(firebaseData, "/STATUS/TEMPSET/MIN1", Min1);
  Firebase.getInt(firebaseData, "/STATUS/TEMPSET/MAX2", Max2);
  Firebase.getInt(firebaseData, "/STATUS/TEMPSET/MIN2", Min2);

  Serial.print("--------------");
  Serial.print("h : ");
  Serial.print(p_tm->tm_hour);
  Serial.print(" m : ");
  Serial.print(p_tm->tm_min);
  Serial.println("--------------");
  Serial.print("H_on1 : ");
  Serial.println(H_on1);
  Serial.print("H_on2 : ");
  Serial.println(H_on2);
  Serial.print("H_on3 : ");
  Serial.println(H_on3);
  Serial.print("H_on4 : ");
  Serial.println(H_on4);
  Serial.print("M_on1 : ");
  Serial.println(M_on1);
  Serial.print("M_on2 : ");
  Serial.println(M_on2);
  Serial.print("M_on3 : ");
  Serial.println(M_on3);
  Serial.print("M_on4 : ");
  Serial.println(M_on4);
  Serial.print("H_off1 : ");
  Serial.println(H_off1);
  Serial.print("H_off2 : ");
  Serial.println(H_off2);
  Serial.print("H_off3 : ");
  Serial.println(H_off3);
  Serial.print("H_off4 : ");
  Serial.println(H_off4);
  Serial.print("M_off1 : ");
  Serial.println(M_off1);
  Serial.print("M_off2 : ");
  Serial.println(M_off2);
  Serial.print("M_off3 : ");
  Serial.println(M_off3);
  Serial.print("M_off4 : ");
  Serial.println(M_off4);
  Serial.print("LED : ");
  Serial.println(led);
  Serial.print("DEVICE1 : ");
  Serial.println(device1);
  Serial.print("DEVICE2 : ");
  Serial.println(device2);
  Serial.print("LEDMODE : ");
  Serial.println(LEDMODE);
  Serial.print("device1mode : ");
  Serial.println(device1mode);
  Serial.print("device2mode : ");
  Serial.println(device2mode);
  Serial.print("MAX1 : ");
  Serial.println(Max1);
  Serial.print("MIN1 : ");
  Serial.println(Min1);
  Serial.print("MAX2 : ");
  Serial.println(Max2);
  Serial.print("MIN2 : ");
  Serial.println(Min2);
  
//---------------------LED--------------------------
  if (LEDMODE == 1)
  { if (led == 0)
    {
      digitalWrite(LED, HIGH);
    }
    else if (led == 1)
    {
      digitalWrite(LED, LOW);
    }
  }
  if (LEDMODE == 0)
  {
    if (p_tm->tm_hour == H_on1 && p_tm->tm_min == M_on1) {
      Firebase.setInt(firebaseData, "/FARM/LED", 1);
      digitalWrite(LED, LOW);
    }
    if (p_tm->tm_hour == H_off1 && p_tm->tm_min == M_off1) {
      Firebase.setInt(firebaseData, "/FARM/LED", 0);
      digitalWrite(LED, HIGH);
    }
  }
  
//---------------------device1--------------------------
  if (device1mode == 1)
  {
    if ( device1 == 1) {
      Firebase.setInt(firebaseData, "/FARM/device1", 1);
      digitalWrite(Device1, LOW);
    }
    if ( device1 == 0) {
      Firebase.setInt(firebaseData, "/FARM/device1", 0);
      digitalWrite(Device1, HIGH);
    }
  }
  if (device1mode == 0)
  {
    if (p_tm->tm_hour == H_on3 && p_tm->tm_min == M_off3)
    {
      Firebase.setInt(firebaseData, "/FARM/device1", 1);
      digitalWrite(Device1, LOW);
    }
    if (p_tm->tm_hour == H_off3 && p_tm->tm_min == M_off3)
    {
        Firebase.setInt(firebaseData, "/FARM/device1", 0);
        digitalWrite(Device1, HIGH);
    }
  }
  if (device1mode == 2) {
    if (Temp <= Max1) {
      Firebase.setInt(firebaseData, "/FARM/device1", 1);
      digitalWrite(Device1, LOW);
    }
    if (Temp > Min1) {
      Firebase.setInt(firebaseData, "/FARM/device1", 0);
      digitalWrite(Device1, HIGH);
    }
  }
  
//---------------------device2--------------------------
  if (device2mode == 1)
  {
    if ( device2 == 1) {
      Firebase.setInt(firebaseData, "/FARM/device2", 1);
      digitalWrite(Device2, LOW);
    }
    if ( device2 == 0) {
      Firebase.setInt(firebaseData, "/FARM/device2", 0);
      digitalWrite(Device2, HIGH);
    }
  }
  if (device2mode == 0)
  {
    if (p_tm->tm_hour == H_on4 && p_tm->tm_min == M_off4)
    {
      Firebase.setInt(firebaseData, "/FARM/device2", 1);
      digitalWrite(Device2, LOW);
    }
    if (p_tm->tm_hour == H_off4 && p_tm->tm_min == M_off4)
    {
        Firebase.setInt(firebaseData, "/FARM/device2", 0);
        digitalWrite(Device2, HIGH);
    }
  }
  if (device2mode == 2) {
    if (Temp <= Max1) {
      Firebase.setInt(firebaseData, "/FARM/device2", 1);
      digitalWrite(Device2, LOW);
    }
    if (Temp > Min1) {
      Firebase.setInt(firebaseData, "/FARM/device2", 0);
      digitalWrite(Device2, HIGH);
    }
  }
  delay(1000);
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
