#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Konstan untuk WiFi credentials
const char *WIFI_SSID = "Redmi Note 12 Pro";
const char *WIFI_PASS = "66666666";

// API server address
const char *serverName = "http://192.168.75.25:3001/api/data"; // Ganti <alamat_ip_komputer> dengan alamat IP lokal komputer Anda

// Pin assignments
int sensor_input = 34;
#define DHTPIN 4  // DHT22 sensor pin
#define DHTTYPE DHT11  // DHT sensor type
const int ledPin = 32;  // LED pin

// Global objects
DHT dht(DHTPIN, DHTTYPE);

// Fungsi untuk mendapatkan data suhu
float get_temperature_data() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Error reading temperature!"));
    return 0.0;  // Return default value on error
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.println(F("°C"));
    return t;
  }
}

// Fungsi untuk mendapatkan data kelembaban
float get_humidity_data() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println(F("Error reading humidity!"));
    return 0.0;  // Return default value on error
  } else {
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%"));
    return h;
  }
}

// Fungsi untuk membaca data analog dari air quality sensor
int get_air_quality_data() {
  int analog_data = analogRead(sensor_input);
  Serial.print("Air Quality: ");
  Serial.println(analog_data);
  delay(300);  // Allow sensor to stabilize
  return analog_data;
}

// Fungsi untuk mengirim data ke API server
void send_data_to_server(float temperature, float humidity, int air_quality) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"temperature\":" + String(temperature) + 
                      ",\"humidity\":" + String(humidity) + 
                      ",\"air_quality\":" + String(air_quality) + "}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

// Fungsi untuk mengontrol lampu berdasarkan kondisi lingkungan
void control_lamp(float temperature, float humidity, int air_quality) {
  bool is_hot = temperature > 32.0; // Kondisi suhu terlalu panas
  bool is_dry = humidity < 32.0; // Kondisi kelembapan terlalu kering
  bool is_bad_air_quality = air_quality > 1000; // Kondisi kualitas udara jelek

  if (is_hot || is_dry || is_bad_air_quality) {
    digitalWrite(ledPin, HIGH); // Nyalakan lampu
  } else {
    digitalWrite(ledPin, LOW); // Matikan lampu
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  dht.begin();
  pinMode(ledPin, OUTPUT); // Set LED pin as output
}

void loop() {
  delay(1000);
  float temperature = get_temperature_data();
  float humidity = get_humidity_data();
  int air_quality = get_air_quality_data();
  
  send_data_to_server(temperature, humidity, air_quality);
  control_lamp(temperature, humidity, air_quality);
  
  delay(5000);
}
