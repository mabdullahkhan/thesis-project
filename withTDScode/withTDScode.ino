#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// ----- Pin Definitions -----
#define DHTPIN 22           // DHT22 data pin
#define DHTTYPE DHT22       // DHT 22 (AM2302)
#define TRIG_PIN 18         // HC-SR04 trigger pin
#define ECHO_PIN 19         // HC-SR04 echo pin
#define RAIN_SENSOR_PIN 21  // Rain sensor A0 analog output
#define TDS_PIN 15          // TDS sensor analog output

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("ESP32 Multi-Sensor Monitor (DHT22 + HC-SR04 + Rain + TDS)");
}

void loop() {
  // ----- DHT22: Temperature & Humidity -----
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // ----- HC-SR04: Water Level -----
  long duration;
  float distance_cm;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance_cm = duration * 0.0343 / 2;

  // ----- Rain Sensor -----
  int rainValue = analogRead(RAIN_SENSOR_PIN);

  // ----- TDS Sensor -----
  int tdsAnalog = analogRead(TDS_PIN);
  float tdsVoltage = tdsAnalog * (3.3 / 4095.0);  // ESP32 ADC range
  float tdsValue = (tdsVoltage / 3.3) * 1000;     // Simple estimation (needs calibration)

  // ----- Serial Output -----
  Serial.println("------ SENSOR READINGS ------");

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT22: Failed to read!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  Serial.print("Water Level Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);
  if (rainValue < 1000) {
    Serial.println("Rain Status: It's Raining!");
  } else {
    Serial.println("Rain Status: Dry");
  }

  Serial.print("TDS Raw Value: ");
  Serial.println(tdsAnalog);
  Serial.print("TDS Voltage: ");
  Serial.print(tdsVoltage, 2);
  Serial.println(" V");
  Serial.print("Estimated TDS: ");
  Serial.print(tdsValue, 1);
  Serial.println(" ppm");

  Serial.println("-----------------------------\n");

  delay(2000);  // Wait 2 seconds before next reading
}
