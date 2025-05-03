#include <WiFi.h>

const char* ssid = "Khan Network";
const char* password = "Sohel@437";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP address:");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        client.readStringUntil('\r');
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println(buildHTML());
        break;
      }
    }
    client.stop();
  }
}

String buildHTML() {
  return R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>IoT Smart Water Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: 'Segoe UI', sans-serif;
        background: #e0f2f1;
        color: #004d40;
        margin: 0;
        padding: 20px;
      }
      h1 {
        text-align: center;
        color: #00695c;
        margin-bottom: 5px;
      }
      p.sub {
        text-align: center;
        margin: 0;
        color: #00796b;
        font-style: italic;
      }
      .clock {
        text-align: center;
        margin-top: 10px;
        font-size: 18px;
        color: #004d40;
        font-weight: bold;
      }
      .grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
        gap: 20px;
        max-width: 1000px;
        margin: 30px auto;
      }
      .card {
        background: #ffffff;
        padding: 20px;
        border-radius: 12px;
        box-shadow: 0 4px 10px rgba(0, 0, 0, 0.15);
        text-align: center;
      }
      .title {
        font-size: 16px;
        margin-bottom: 8px;
        color: #00796b;
      }
      .value {
        font-size: 24px;
        font-weight: bold;
        color: #004d40;
      }
    </style>
  </head>
  <body>
    <h1>IoT-Enabled Smart Water Management System</h1>
    <p class="sub">for Efficient Urban Resource Usage</p>
    <div class="clock" id="clock">Loading current time...</div>
    <div class="grid">
      <div class="card">
        <div class="title">Temperature</div>
        <div class="value">28 °C</div>
      </div>
      <div class="card">
        <div class="title">Humidity</div>
        <div class="value">17 %</div>
      </div>
      <div class="card">
        <div class="title">Water Level</div>
        <div class="value">10 cm</div>
      </div>
      <div class="card">
        <div class="title">Rain Sensor</div>
        <div class="value">It's Raining</div>
      </div>
      <div class="card">
        <div class="title">TDS Level</div>
        <div class="value">100 ppm</div>
      </div>
      <div class="card">
        <div class="title">pH Level</div>
        <div class="value">5.6</div>
      </div>
    </div>

    <script>
      function updateClock() {
        const now = new Date();
        const options = {
          weekday: 'long', year: 'numeric', month: 'long',
          day: 'numeric', hour: '2-digit', minute: '2-digit', second: '2-digit'
        };
        const formatted = now.toLocaleString('en-US', options);
        document.getElementById('clock').textContent = formatted;
      }

      setInterval(updateClock, 1000);
      updateClock(); // initial call
    </script>
  </body>
  </html>
  )rawliteral";
}
