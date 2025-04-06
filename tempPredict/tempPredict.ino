#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_SHT4x.h>
#include <time.h>
#include <HTTPClient.h>


// WiFi Credentials
const char* ssid = "Your_ssid";
const char* password = "Your_password";

// NETPIE MQTT Credentials
const char* mqtt_server = "mqtt.netpie.io";
const char* clientID = "Your_clientID";
const char* username = "Your_username";
const char* passwordMqtt = "Your_passwordMqtt";

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor Objects
Adafruit_HTS221 hts221;
Adafruit_SHT4x sht4x;
bool useHTS221 = false; // Flag to track which sensor is available

String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "N/A";
  }
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void sendToGoogleSheet(float temp, float humid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("Your_http");
    http.addHeader("Content-Type", "application/json");

    String timestamp = getTimeString();
    String json = "{\"temp\":" + String(temp, 2) + 
                  ",\"humid\":" + String(humid, 2) +
                  ",\"time\":\"" + timestamp + "\"}";

    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      Serial.print("Google Sheets Response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Error sending to Google Sheets: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

// Data Queue (FIFO)
struct SensorData {
  float temp;
  float humid;
};
QueueHandle_t sensorQueue;

// Connect to WiFi
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

// MQTT Reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID, username, passwordMqtt)) {
      Serial.println("Connected to NETPIE!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again...");
      delay(5000);
    }
  }
}

// **Thread 1: Read Sensors and Store in Queue**
void sensorTask(void *pvParameters) {
  while (1) {
    SensorData data;
    
    if (useHTS221) {
      sensors_event_t temp, humid;
      hts221.getEvent(&humid, &temp);
      data.temp = temp.temperature;
      data.humid = humid.relative_humidity;
    } else {
      sensors_event_t humidity, temperature;
      sht4x.getEvent(&humidity, &temperature);
      data.temp = temperature.temperature;
      data.humid = humidity.relative_humidity;
    }

    // Add Data to Queue
    if (xQueueSend(sensorQueue, &data, portMAX_DELAY) == pdPASS) {
      Serial.println("Sensor data added to queue.");
    }

    vTaskDelay(pdMS_TO_TICKS(60000)); // Wait 1 minute
  }
}

// **Thread 2: Read from Queue and Publish to NETPIE**
void mqttTask(void *pvParameters) {
  while (1) {
    if (!client.connected()) {
      reconnect();
    }

    SensorData data;
    if (xQueueReceive(sensorQueue, &data, portMAX_DELAY) == pdPASS) {
      String payload = "{ \"data\": { \"temp\": " + String(data.temp) +
                       ", \"humid\": " + String(data.humid) + " }}";
      
      client.publish("@shadow/data/update", payload.c_str());
      Serial.println("Published data to NETPIE.");

      // เพิ่มบรรทัดนี้
      sendToGoogleSheet(data.temp, data.humid);
    }

    vTaskDelay(pdMS_TO_TICKS(10000)); // Publish every 10 sec
  }
}

// **Setup Function**
void setup() {
  Wire.begin(41,40);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // GMT+7 สำหรับไทย

  Wire.begin(8, 9); // Adjust SDA & SCL for ESP32-S2

  // Sensor Detection
  Wire.beginTransmission(0x5F);
  if (Wire.endTransmission() == 0) {
    Serial.println("HTS221 detected at 0x5F");
    if (hts221.begin_I2C()) {
      useHTS221 = true;
    }
  } else {
    Serial.println("HTS221 not found, checking for SHT4x...");
    if (sht4x.begin()) {
      Serial.println("SHT4x detected at 0x44");
      useHTS221 = false;
    } else {
      Serial.println("ERROR: No valid temp/humidity sensor found!");
    }
  }

  // Create Queue (FIFO Buffer)
  sensorQueue = xQueueCreate(10, sizeof(SensorData));

  // Create Threads
  xTaskCreate(sensorTask, "SensorTask", 10000, NULL, 1, NULL);
  xTaskCreate(mqttTask, "MQTTTask", 10000, NULL, 1, NULL);
}

// **Loop Function (Empty Because Tasks Handle Execution)**
void loop() {
  client.loop();
}
