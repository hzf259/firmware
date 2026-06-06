#include <Arduino.h>
#include <Wire.h>

#include "app_config.h"

namespace {

HardwareSerial &DEBUG_SERIAL = Serial1;
HardwareSerial &ESP_SERIAL = Serial2;

constexpr uint8_t SHT31_MEAS_HIGHREP_STRETCH_DISABLED_MSB = 0x24;
constexpr uint8_t SHT31_MEAS_HIGHREP_STRETCH_DISABLED_LSB = 0x00;
constexpr uint8_t SHT31_SOFT_RESET_MSB = 0x30;
constexpr uint8_t SHT31_SOFT_RESET_LSB = 0xA2;

struct SensorReading {
  float temperature = NAN;
  float humidity = NAN;
  bool valid = false;
};

struct ButtonState {
  bool lastPhysicalPressed = false;
  bool stablePressed = false;
  uint32_t lastDebounceAt = 0;
  uint32_t pressedAt = 0;
  bool longPressHandled = false;
  bool shortPressEvent = false;
};

enum class PublishReason {
  Periodic,
  Alarm,
  Manual,
};

String mqttTelemetryTopic() {
  return "coldchain/" DEVICE_ID "/telemetry";
}

String mqttCmdTopic() {
  return "coldchain/" DEVICE_ID "/cmd";
}

bool crc8Matches(const uint8_t *data, uint8_t expected) {
  uint8_t crc = 0xFF;
  for (uint8_t i = 0; i < 2; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      if (crc & 0x80) {
        crc = static_cast<uint8_t>((crc << 1) ^ 0x31);
      } else {
        crc <<= 1;
      }
    }
  }
  return crc == expected;
}

void setBuzzer(bool on) {
  const uint8_t level = (on == BUZZER_ACTIVE_HIGH) ? HIGH : LOW;
  digitalWrite(BUZZER_PIN, level);
}

bool isButtonPressedRaw(uint8_t pin) {
  const int raw = digitalRead(pin);
  return BUTTON_ACTIVE_HIGH ? (raw == HIGH) : (raw == LOW);
}

String formatFloat2(float value) {
  char buf[24];
  dtostrf(value, 0, 2, buf);
  return String(buf);
}

String escapeAtString(const String &input) {
  String out;
  out.reserve(input.length() + 8);
  for (size_t i = 0; i < input.length(); ++i) {
    const char c = input[i];
    if (c == '\\' || c == '"' || c == ',') {
      out += '\\';
    }
    out += c;
  }
  return out;
}

class SHT31Sensor {
 public:
  bool begin() {
    Wire.begin();
    delay(20);
    return writeCommand(SHT31_SOFT_RESET_MSB, SHT31_SOFT_RESET_LSB);
  }

  SensorReading read() {
    SensorReading reading;
    if (!writeCommand(SHT31_MEAS_HIGHREP_STRETCH_DISABLED_MSB,
                      SHT31_MEAS_HIGHREP_STRETCH_DISABLED_LSB)) {
      return reading;
    }

    delay(20);
    Wire.requestFrom(static_cast<uint8_t>(SHT31_I2C_ADDR), static_cast<uint8_t>(6));
    if (Wire.available() != 6) {
      return reading;
    }

    uint8_t raw[6];
    for (int i = 0; i < 6; ++i) {
      raw[i] = Wire.read();
    }

    if (!crc8Matches(raw, raw[2]) || !crc8Matches(raw + 3, raw[5])) {
      return reading;
    }

    const uint16_t rawTemp = (static_cast<uint16_t>(raw[0]) << 8) | raw[1];
    const uint16_t rawHum = (static_cast<uint16_t>(raw[3]) << 8) | raw[4];

    reading.temperature = -45.0f + 175.0f * (static_cast<float>(rawTemp) / 65535.0f);
    reading.humidity = 100.0f * (static_cast<float>(rawHum) / 65535.0f);
    reading.valid = true;
    return reading;
  }

 private:
  bool writeCommand(uint8_t msb, uint8_t lsb) {
    Wire.beginTransmission(SHT31_I2C_ADDR);
    Wire.write(msb);
    Wire.write(lsb);
    return Wire.endTransmission() == 0;
  }
};

class EspAtClient {
 public:
  void begin() {
    ESP_SERIAL.begin(ESP_AT_BAUDRATE);
    delay(200);
  }

  void tick() {
    while (ESP_SERIAL.available()) {
      char c = static_cast<char>(ESP_SERIAL.read());
      if (c == '\r') {
        continue;
      }
      if (c == '\n') {
        if (!lineBuffer_.isEmpty()) {
          handleLine(lineBuffer_);
          responseBuffer_ += lineBuffer_;
          responseBuffer_ += '\n';
          lineBuffer_ = "";
        }
        continue;
      }
      lineBuffer_ += c;
    }
  }

  bool initModule() {
    if (!runCommand("AT", 2000)) {
      return false;
    }
    runCommand("ATE0", 1000);
    runCommand("AT+CWMODE=1", 2000);
    runCommand("AT+SYSSTORE=0", 1000);
    moduleReady_ = true;
    return true;
  }

  bool ensureWifi() {
    if (!moduleReady_ && !initModule()) {
      return false;
    }
    if (String(WIFI_SSID).startsWith("YOUR_") || String(WIFI_PASSWORD).startsWith("YOUR_")) {
      DEBUG_SERIAL.println("[ESP] please set WIFI_SSID/WIFI_PASSWORD in app_config.h");
      return false;
    }
    if (wifiReady_) {
      return true;
    }
    if (millis() - lastWifiAttemptAt_ < WIFI_RECONNECT_INTERVAL_MS) {
      return false;
    }
    lastWifiAttemptAt_ = millis();

    DEBUG_SERIAL.println("[ESP] connecting Wi-Fi...");
    const String cmd = "AT+CWJAP=\"" + escapeAtString(WIFI_SSID) + "\",\"" +
                       escapeAtString(WIFI_PASSWORD) + "\"";
    wifiReady_ = runCommand(cmd, 30000);
    if (wifiReady_) {
      DEBUG_SERIAL.println("[ESP] Wi-Fi connected");
    } else {
      DEBUG_SERIAL.println("[ESP] Wi-Fi connect failed");
    }
    return wifiReady_;
  }

  bool ensureMqtt() {
    if (!ensureWifi()) {
      return false;
    }
    if (mqttReady_) {
      return true;
    }
    if (millis() - lastMqttAttemptAt_ < MQTT_RECONNECT_INTERVAL_MS) {
      return false;
    }
    lastMqttAttemptAt_ = millis();

    runCommand("AT+MQTTCLEAN=0", 2000);

    String cfg = "AT+MQTTUSERCFG=0,1,\"" + escapeAtString(String(MQTT_CLIENT_ID)) +
                 "\",\"" + escapeAtString(String(MQTT_USERNAME)) + "\",\"" +
                 escapeAtString(String(MQTT_PASSWORD)) + "\",0,0,\"\"";
    if (!runCommand(cfg, 5000)) {
      DEBUG_SERIAL.println("[ESP] MQTTUSERCFG failed");
      return false;
    }

    String conn = "AT+MQTTCONN=0,\"" + escapeAtString(String(MQTT_HOST)) + "\"," +
                  String(MQTT_PORT) + ",0";
    if (!runCommand(conn, 10000)) {
      DEBUG_SERIAL.println("[ESP] MQTTCONN failed");
      return false;
    }

    mqttReady_ = true;
    DEBUG_SERIAL.println("[ESP] MQTT connected");

    const String sub = "AT+MQTTSUB=0,\"" + escapeAtString(mqttCmdTopic()) + "\",1";
    if (!runCommand(sub, 5000)) {
      DEBUG_SERIAL.println("[ESP] cmd subscribe failed");
    } else {
      DEBUG_SERIAL.println("[ESP] cmd topic subscribed");
    }
    return true;
  }

  bool publishTelemetry(const String &payload) {
    if (!ensureMqtt()) {
      return false;
    }
    const String topic = escapeAtString(mqttTelemetryTopic());
    const String body = escapeAtString(payload);
    const String cmd = "AT+MQTTPUB=0,\"" + topic + "\",\"" + body + "\",1,0";
    const bool ok = runCommand(cmd, 8000);
    if (!ok) {
      mqttReady_ = false;
    }
    return ok;
  }

  bool remoteAlarmOn() const {
    return remoteAlarmOn_;
  }

  bool mqttReady() const {
    return mqttReady_;
  }

  void forceReconnect() {
    wifiReady_ = false;
    mqttReady_ = false;
    moduleReady_ = false;
  }

 private:
  bool runCommand(const String &command, uint32_t timeoutMs) {
    responseBuffer_ = "";
    lineBuffer_ = "";

    while (ESP_SERIAL.available()) {
      ESP_SERIAL.read();
    }

    DEBUG_SERIAL.print("[ESP] >> ");
    DEBUG_SERIAL.println(command);
    ESP_SERIAL.print(command);
    ESP_SERIAL.print("\r\n");

    const uint32_t start = millis();
    while (millis() - start < timeoutMs) {
      tick();
      if (responseBuffer_.indexOf("\nOK\n") >= 0 || responseBuffer_.endsWith("OK")) {
        DEBUG_SERIAL.println("[ESP] << OK");
        return true;
      }
      if (responseBuffer_.indexOf("ERROR") >= 0 || responseBuffer_.indexOf("FAIL") >= 0) {
        DEBUG_SERIAL.println("[ESP] << ERROR");
        DEBUG_SERIAL.println(responseBuffer_);
        return false;
      }
      delay(5);
    }

    DEBUG_SERIAL.println("[ESP] << TIMEOUT");
    DEBUG_SERIAL.println(responseBuffer_);
    return false;
  }

  void handleLine(const String &line) {
    if (line.indexOf("WIFI GOT IP") >= 0) {
      wifiReady_ = true;
      return;
    }
    if (line.indexOf("WIFI DISCONNECT") >= 0 || line.indexOf("NO AP") >= 0) {
      wifiReady_ = false;
      mqttReady_ = false;
      return;
    }
    if (line.indexOf("+MQTTCONNECTED:") >= 0) {
      mqttReady_ = true;
      return;
    }
    if (line.indexOf("+MQTTDISCONNECTED:") >= 0) {
      mqttReady_ = false;
      return;
    }
    if (line.startsWith("+MQTTSUBRECV:")) {
      handleMqttSubRecv(line);
      return;
    }
  }

  void handleMqttSubRecv(const String &line) {
    const int firstComma = line.indexOf(',');
    if (firstComma < 0) return;

    const int topicStart = line.indexOf('"', firstComma);
    if (topicStart < 0) return;
    const int topicEnd = line.indexOf('"', topicStart + 1);
    if (topicEnd < 0) return;

    const int lenComma = line.indexOf(',', topicEnd + 1);
    if (lenComma < 0) return;
    const int payloadComma = line.indexOf(',', lenComma + 1);
    if (payloadComma < 0) return;

    const String payload = line.substring(payloadComma + 1);
    if (payload.indexOf("\"state\":\"ON\"") >= 0) {
      remoteAlarmOn_ = true;
      DEBUG_SERIAL.println("[ESP] remote alarm ON");
    } else if (payload.indexOf("\"state\":\"OFF\"") >= 0) {
      remoteAlarmOn_ = false;
      DEBUG_SERIAL.println("[ESP] remote alarm OFF");
    }
  }

  bool moduleReady_ = false;
  bool wifiReady_ = false;
  bool mqttReady_ = false;
  bool remoteAlarmOn_ = false;
  uint32_t lastWifiAttemptAt_ = 0;
  uint32_t lastMqttAttemptAt_ = 0;
  String lineBuffer_;
  String responseBuffer_;
};

SHT31Sensor sensor;
EspAtClient esp;

SensorReading latestReading;
bool localAlarmOn = false;
bool manualPublishRequested = false;
bool buzzerSilenced = false;
uint32_t buzzerSilencedUntil = 0;
uint32_t lastSampleAt = 0;
uint32_t lastTelemetryAt = 0;

ButtonState silenceButton;
ButtonState actionButton;

void updateButton(ButtonState &state, uint8_t pin) {
  const bool physicalPressed = isButtonPressedRaw(pin);
  const uint32_t now = millis();

  if (physicalPressed != state.lastPhysicalPressed) {
    state.lastDebounceAt = now;
    state.lastPhysicalPressed = physicalPressed;
  }

  if (now - state.lastDebounceAt < 30) {
    return;
  }

  if (state.stablePressed != physicalPressed) {
    state.stablePressed = physicalPressed;
    if (state.stablePressed) {
      state.pressedAt = now;
      state.longPressHandled = false;
    } else {
      if (state.pressedAt != 0 && !state.longPressHandled) {
        state.shortPressEvent = true;
      }
      state.pressedAt = 0;
      state.longPressHandled = false;
    }
  }
}

bool isOutOfRange(const SensorReading &reading) {
  if (!reading.valid) {
    return false;
  }
  return reading.temperature < TEMP_MIN_C || reading.temperature > TEMP_MAX_C ||
         reading.humidity < HUM_MIN_RH || reading.humidity > HUM_MAX_RH;
}

String buildTelemetryJson(const SensorReading &reading, PublishReason reason) {
  String payload = "{";
  payload += "\"device_id\":\"" DEVICE_ID "\",";
  payload += "\"shipment_code\":\"" SHIPMENT_CODE "\",";
  payload += "\"temperature\":" + formatFloat2(reading.temperature) + ",";
  payload += "\"humidity\":" + formatFloat2(reading.humidity) + ",";
  payload += "\"alarm_local\":" + String(localAlarmOn ? "true" : "false") + ",";
  payload += "\"alarm_remote\":" + String(esp.remoteAlarmOn() ? "true" : "false") + ",";
  payload += "\"publish_reason\":\"";
  switch (reason) {
    case PublishReason::Periodic:
      payload += "periodic";
      break;
    case PublishReason::Alarm:
      payload += "alarm";
      break;
    case PublishReason::Manual:
      payload += "manual";
      break;
  }
  payload += "\"";
  payload += "}";
  return payload;
}

void sampleSensorIfNeeded() {
  const uint32_t now = millis();
  if (now - lastSampleAt < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleAt = now;

  SensorReading reading = sensor.read();
  if (!reading.valid) {
    DEBUG_SERIAL.println("[SHT31] read failed");
    return;
  }

  latestReading = reading;
  DEBUG_SERIAL.print("[SHT31] T=");
  DEBUG_SERIAL.print(reading.temperature, 2);
  DEBUG_SERIAL.print("C H=");
  DEBUG_SERIAL.print(reading.humidity, 2);
  DEBUG_SERIAL.println("%");

  localAlarmOn = isOutOfRange(reading);
  if (!localAlarmOn) {
    buzzerSilenced = false;
  }
}

void handleButtons() {
  updateButton(silenceButton, SILENCE_BUTTON_PIN);
  updateButton(actionButton, ACTION_BUTTON_PIN);

  const uint32_t now = millis();

  if (silenceButton.stablePressed && !silenceButton.longPressHandled &&
      now - silenceButton.pressedAt > 80) {
    silenceButton.longPressHandled = true;
    buzzerSilenced = true;
    buzzerSilencedUntil = now + SILENCE_DURATION_MS;
    DEBUG_SERIAL.println("[BTN] silence buzzer");
  }

  if (actionButton.stablePressed && !actionButton.longPressHandled &&
      now - actionButton.pressedAt >= BUTTON_LONG_PRESS_MS) {
    actionButton.longPressHandled = true;
    esp.forceReconnect();
    DEBUG_SERIAL.println("[BTN] force reconnect");
  }

  if (actionButton.shortPressEvent) {
    actionButton.shortPressEvent = false;
    manualPublishRequested = true;
    DEBUG_SERIAL.println("[BTN] manual publish");
  }

  if (buzzerSilenced && now >= buzzerSilencedUntil) {
    buzzerSilenced = false;
  }
}

void updateBuzzer() {
  const bool shouldBuzz = (localAlarmOn || esp.remoteAlarmOn()) &&
                          !(buzzerSilenced && localAlarmOn);
  setBuzzer(shouldBuzz);
}

void publishTelemetryIfNeeded() {
  if (!latestReading.valid) {
    return;
  }

  const uint32_t now = millis();
  PublishReason reason = PublishReason::Periodic;
  bool shouldPublish = false;

  if (manualPublishRequested) {
    shouldPublish = true;
    reason = PublishReason::Manual;
    manualPublishRequested = false;
  } else if (localAlarmOn && now - lastTelemetryAt >= 3000) {
    shouldPublish = true;
    reason = PublishReason::Alarm;
  } else if (now - lastTelemetryAt >= TELEMETRY_INTERVAL_MS) {
    shouldPublish = true;
    reason = PublishReason::Periodic;
  }

  if (!shouldPublish) {
    return;
  }

  String payload = buildTelemetryJson(latestReading, reason);
  if (esp.publishTelemetry(payload)) {
    lastTelemetryAt = now;
    DEBUG_SERIAL.println("[MQTT] telemetry published");
  } else {
    DEBUG_SERIAL.println("[MQTT] publish failed");
  }
}

void ensureEspReady() {
  esp.tick();
  esp.ensureMqtt();
}

}  // namespace

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(false);

  pinMode(SILENCE_BUTTON_PIN, BUTTON_ACTIVE_HIGH ? INPUT : INPUT_PULLUP);
  pinMode(ACTION_BUTTON_PIN, BUTTON_ACTIVE_HIGH ? INPUT : INPUT_PULLUP);

  DEBUG_SERIAL.begin(115200);
  delay(300);
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("==== ColdChain STM32 Device Boot ====");
  DEBUG_SERIAL.println("Board: STM32F103C8T6 (Blue Pill)");
  DEBUG_SERIAL.println("Sensor: SHT31");
  DEBUG_SERIAL.println("Wi-Fi: ESP8266 ESP-AT");

  sensor.begin();
  esp.begin();

  delay(SENSOR_WARMUP_MS);
}

void loop() {
  ensureEspReady();
  handleButtons();
  sampleSensorIfNeeded();
  updateBuzzer();
  publishTelemetryIfNeeded();
  delay(5);
}
