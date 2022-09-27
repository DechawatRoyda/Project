#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <time.h>

#define WIFI_SSID "SEKSAN_2.4Ghz"
#define WIFI_PASSWORD "0813808670"

#define FIREBASE_HOST "asixiot-default-rtdb.firebaseio.com"
#define FIREBASE_KEY "QsvJIcK5rgRVfJ1hWuaIXOumz5nUbB1Bg9c8w6Uv"

#define PUMP D0

FirebaseData firebaseData;
int pump, Sh1, Eh1, x, y, PUMPMODE;
int timezone = 7 * 3600;
int dst = 0;
byte time_h, time_m;
void setup() {
  //------------------Wifi ------------------
  connectWifi();//voidเรียกใช้ไวไฟเอา
  pinMode(PUMP, OUTPUT);
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
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  time_h = p_tm->tm_hour;
  time_m = p_tm->tm_min;

  Firebase.getInt(firebaseData, "/FARM/PUMPMODE", PUMPMODE);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_on2", H_on2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_on2", M_on2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/H_off2", H_off2);
  Firebase.getInt(firebaseData, "/STATUS/TIMESET/M_off2", M_off2);
  Firebase.getInt(firebaseData, "/FARM/PUMP", pump);
  Firebase.getInt(firebaseData, "/SETPUMP/MINon", x);
  Firebase.getInt(firebaseData, "/SETPUMP/MINoff", y);

  Serial.print("--------------");
  Serial.print("h : ");
  Serial.print(p_tm->tm_hour);
  Serial.print(" m : ");
  Serial.print(p_tm->tm_min);
  Serial.println("--------------");
  Serial.print("Sh1 : ");
  Serial.println(Sh1);
  Serial.print("Eh1 : ");
  Serial.println(Eh1);
  Serial.print("PUMP : ");
  Serial.println(pump);
  Serial.print("PUMP MIN on : ");
  Serial.println(x);
  Serial.print("PUMP MIN off : ");
  Serial.println(y);

  if (PUMPMODE == 1)
  {
    if (pump == 0) {
      digitalWrite(PUMP, HIGH);
    }
    if (pump == 1)
    {
      digitalWrite(PUMP, LOW);
    }
  }
 if (PUMPMODE == 0)
 {
  if (p_tm->tm_hour == H_on2 && p_tm->tm_min == M_on2) {
    Firebase.setInt(firebaseData, "/FARM/PUMP", 1);
    digitalWrite(PUMP, LOW);
  }
  if (p_tm->tm_hour == H_off2 && p_tm->tm_min == M_off2) {
    Firebase.setInt(firebaseData, "/FARM/PUMP", 0);
    digitalWrite(PUMP, HIGH);
  }
   if (pump == 0) {
      digitalWrite(PUMP, HIGH);
      delay(y * 1000 * 60);
      Firebase.setInt(firebaseData, "/FARM/PUMP", 1);
    }
    if (pump == 1)
    {
      digitalWrite(PUMP, LOW);
      delay(x * 1000 * 60);
      Firebase.setInt(firebaseData, "/FARM/PUMP", 0);
    }
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
