#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

#define WIFI_SSID "SEKSAN_2.4Ghz"
#define WIFI_PASSWORD "0813808670"

#define FIREBASE_HOST "asixiot-default-rtdb.firebaseio.com"
#define FIREBASE_KEY "QsvJIcK5rgRVfJ1hWuaIXOumz5nUbB1Bg9c8w6Uv"

#define DHTPIN1 D1
#define DHTPIN2 D2
#define DHTPIN3 D3
#define DHTPIN4 D4

#define DHTTYPE DHT22

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);
DHT dht4(DHTPIN4, DHTTYPE);

#define LDR  A0


FirebaseData firebaseData;

void setup() {
  pinMode(DHTPIN1,INPUT);
  pinMode(DHTPIN2,INPUT);
  pinMode(DHTPIN3,INPUT);
  pinMode(DHTPIN4,INPUT);
  pinMode(LDR,INPUT);
  connectWifi();
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);

  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();
}
 
void loop() {
  delay(2000);
  float h1 = dht1.readHumidity();                                 // Read Humidity
  float t1 = dht1.readTemperature();                              // Read temperature

  float h2 = dht2.readHumidity();                                 // Read Humidity
  float t2 = dht2.readTemperature();                              // Read temperature

  float h3 = dht3.readHumidity();                                 // Read Humidity
  float t3 = dht3.readTemperature();                              // Read temperature

  float h4 = dht4.readHumidity();                                 // Read Humidity
  float t4 = dht4.readTemperature();                              // Read temperature
  
  
  int  ldr = analogRead(LDR);

  Serial.print("Humidity[1]: ");
  Serial.print(h1);
  Serial.print("%  Temperature[1]: ");
  Serial.print(t1);
  Serial.println("°C ");

  Serial.print("Humidity[2]: ");
  Serial.print(h2);
  Serial.print("%  Temperature[2]: ");
  Serial.print(t2);
  Serial.println("°C ");

  Serial.print("Humidity[3]: ");
  Serial.print(h3);
  Serial.print("%  Temperature[3]: ");
  Serial.print(t3);
  Serial.println("°C ");

  Serial.print("Humidity[4]: ");
  Serial.print(h4);
  Serial.print("%  Temperature[4]: ");
  Serial.print(t4);
  Serial.println("°C ");


  Serial.print("Light sensor value: ");
  Serial.println(ldr);

 
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Temp(1)", t1);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Humidity(1)", h1);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Temp(2)", t2);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Humidity(2)", h2);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Temp(3)", t3);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Humidity(3)", h3);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Temp(4)", t4);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/Humidity(4)", h4);
Firebase.setFloat(firebaseData, "/STATUS/DHT11(1)/TempAVG", Tall);


Firebase.setFloat(firebaseData, "/STATUS/Ldr", ldr);
delay(1000);
}

void connectWifi() {
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
