#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP8266.h> 

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "KANGADY"
#define WIFI_PASSWORD "alinwa10"
#define API_KEY "AIzaSyD7TGX5RZ9w38Wz66Ufbza1EBYzagRPPpM"
#define DATABASE_URL "tcps-1d897-default-rtdb.asia-southeast1.firebasedatabase.app"

#define USER_EMAIL "tugascps@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;
const int mq2= A0;  
const int buzzer = D8;       

#define DHTPIN D4 
#define DHTTYPE DHT11
#define BUZZER_PIN 8
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // connect to wifi
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  config.api_key = API_KEY; // Assign the api key (required)
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(0, 1);
  lcd.print("Smoke: ");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // connect to wifi
  Serial.print("connecting");

  pinMode(buzzer, OUTPUT);  // Set buzzer pin as output
}

void loop() {
  float t = dht.readTemperature();
  int smokeValue = analogRead(mq2);

  // Print values to Serial Monitor
  Serial.print(F("Temp: "));
  Serial.print(t);
  Serial.println("°C");

  Serial.print(F("Smoke: "));
  Serial.print(smokeValue);
  Serial.println("PPM");

  // Display values on LCD
  lcd.setCursor(6, 0);
  lcd.print("     ");
  lcd.setCursor(6, 0);
  lcd.print(String(t) + "C");

  lcd.setCursor(7, 1);
  lcd.print("     ");
  lcd.setCursor(7, 1);
  lcd.print(String(smokeValue) + "PPM");

  // Check conditions and control the buzzer
  if (smokeValue > 700 && t > 45) {
    digitalWrite(buzzer, HIGH);  // Turn on the buzzer
    Serial.println("Buzzer ON");
  } else {
    digitalWrite(buzzer, LOW);   // Turn off the buzzer
    Serial.println("Buzzer OFF");
  }
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 ||sendDataPrevMillis == 0)){
  sendDataPrevMillis = millis();
  Firebase.setFloat(fbdo, "/temp", t);
  Firebase.setFloat(fbdo, "/smoke", smokeValue);
  }

  delay(2000);
}

//ini udah yg >45 sama >700