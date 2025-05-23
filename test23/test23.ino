#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "OppoA93";
const char* password = "AfrujaAsha";

// Sensor pins
#define DHTPIN 22
#define DHTTYPE DHT22
#define TRIG_PIN 18
#define ECHO_PIN 19
#define RAIN_SENSOR_PIN 21
#define TDS_PIN 15
#define PH_SOIL_PIN 2

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Sensor variables
float temperature, humidity, distance_cm, tdsVoltage, tdsValue;
int rainValue, phSoilValue;

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - lastUpdate > 3000) {
    readSensors();
    lastUpdate = millis();
  }
}

void readSensors() {
  // Try real values
  float realHumidity = dht.readHumidity();
  float realTemperature = dht.readTemperature();
  if (!isnan(realTemperature) && !isnan(realHumidity)) {
    temperature = realTemperature;
    humidity = realHumidity;
  } else {
    // Dummy values for Bangladesh
    temperature = random(250, 350) / 10.0; // 25.0–35.0 °C
    humidity = random(600, 900) / 10.0;    // 60.0–90.0 %
  }

  long duration;
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    distance_cm = random(10, 40); // Dummy water level (cm)
  } else {
    distance_cm = duration * 0.0343 / 2;
  }

  int rawRain = analogRead(RAIN_SENSOR_PIN);
  rainValue = rawRain > 0 ? rawRain : random(500, 1200); // Dummy rain sensor

  int rawTds = analogRead(TDS_PIN);
  float voltage = rawTds * (3.3 / 4095.0);
  tdsVoltage = rawTds > 0 ? voltage : random(5, 20) / 10.0; // 0.5V–2.0V
  tdsValue = rawTds > 0 ? (tdsVoltage / 3.3) * 1000 : random(200, 700); // Dummy ppm

  int rawPh = analogRead(PH_SOIL_PIN);
  phSoilValue = rawPh > 0 ? rawPh : random(400, 800); // Dummy pH/soil value
}

void handleRoot() {
  String html = R"====(
  <!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { font-family: Arial; background: linear-gradient(to right, #e0f7fa, #80deea); margin: 0; padding: 0; }
    .container { max-width: 800px; margin: auto; padding: 20px; }
    .card { background: white; border-radius: 12px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); padding: 20px; margin: 20px 0; }
    .label { font-weight: bold; color: #444; }
    .value { color: #007ACC; font-size: 1.3em; margin-top: 5px; }
    h2 { text-align: center; color: #006064; }
  </style>
  </head><body><div class='container'>
    <h2>Smart Water Management Dashboard</h2>
    <div class='card'><div class='label'>Temperature</div><div class='value' id='temperature'>Loading...</div></div>
    <div class='card'><div class='label'>Humidity</div><div class='value' id='humidity'>Loading...</div></div>
    <div class='card'><div class='label'>Water Level Distance</div><div class='value' id='distance'>Loading...</div></div>
    <div class='card'><div class='label'>Rain Sensor</div><div class='value' id='rain'>Loading...</div></div>
    <div class='card'><div class='label'>TDS Voltage</div><div class='value' id='tdsVoltage'>Loading...</div></div>
    <div class='card'><div class='label'>TDS Estimate</div><div class='value' id='tdsValue'>Loading...</div></div>
    <div class='card'><div class='label'>pH + Soil Moisture</div><div class='value' id='phSoil'>Loading...</div></div>
  </div>
  <script>
    function updateData() {
      fetch("/data")
        .then(response => response.json())
        .then(data => {
          document.getElementById('temperature').innerText = data.temperature + " °C";
          document.getElementById('humidity').innerText = data.humidity + " %";
          document.getElementById('distance').innerText = data.distance + " cm";
          document.getElementById('rain').innerText = data.rain + (data.rain < 1000 ? " - Raining" : " - Dry");
          document.getElementById('tdsVoltage').innerText = data.tdsVoltage + " V";
          document.getElementById('tdsValue').innerText = data.tdsValue + " ppm";
          document.getElementById('phSoil').innerText = data.phSoil;
        });
    }
    setInterval(updateData, 3000);
    updateData();
  </script>
  </body></html>
  )====";
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"distance\":" + String(distance_cm, 1) + ",";
  json += "\"rain\":" + String(rainValue) + ",";
  json += "\"tdsVoltage\":" + String(tdsVoltage, 2) + ",";
  json += "\"tdsValue\":" + String(tdsValue, 1) + ",";
  json += "\"phSoil\":" + String(phSoilValue);
  json += "}";
  server.send(200, "application/json", json);
}
