#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <USB.h>

// Configurable matrix size
#ifndef NUM_ROWS
#define NUM_ROWS 3
#endif
#ifndef NUM_COLS
#define NUM_COLS 3
#endif

// GPIO pins for rows and cols (example pins)
const uint8_t rowPins[NUM_ROWS] = {2, 3, 4};
const uint8_t colPins[NUM_COLS] = {5, 6, 7};

// Burner pin (held LOW on boot to enter burner mode)
const uint8_t BURNER_PIN = 0;

Preferences prefs;

String keyMap[NUM_ROWS][NUM_COLS];

void saveMapping(const String &json) {
  prefs.begin("keypad", false);
  prefs.putString("mapping", json);
  prefs.end();
}

void loadMapping() {
  prefs.begin("keypad", true);
  String json = prefs.getString("mapping", "{}");
  prefs.end();
  if (json.length() == 0) return;

  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, json) == DeserializationError::Ok) {
    JsonArray keys = doc["keys"].as<JsonArray>();
    for (int r = 0; r < NUM_ROWS; r++) {
      for (int c = 0; c < NUM_COLS; c++) {
        keyMap[r][c] = keys[r][c].as<String>();
      }
    }
  }
}

void sendMacro(const String &macro) {
  if (macro.indexOf('+') != -1) {
    // Modifier combination
    String temp = macro;
    temp.replace("+", " ");
    // parse tokens
    std::vector<String> parts;
    char *token = strtok((char*)temp.c_str(), " ");
    while (token) {
      parts.push_back(String(token));
      token = strtok(NULL, " ");
    }
    for (size_t i = 0; i < parts.size(); i++) {
      Keyboard.press(parts[i][0]); // simplistic
    }
    Keyboard.releaseAll();
  } else {
    Keyboard.print(macro);
  }
}

void scanMatrix() {
  for (int r = 0; r < NUM_ROWS; r++) {
    digitalWrite(rowPins[r], LOW);
    for (int c = 0; c < NUM_COLS; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        sendMacro(keyMap[r][c]);
        delay(200);
      }
    }
    digitalWrite(rowPins[r], HIGH);
  }
}

void burnerMode() {
  Serial.println("Entering burner mode. Send JSON mapping...");
  String json = Serial.readStringUntil('\n');
  saveMapping(json);
  Serial.println("Mapping saved. Reboot device.");
  while (1) {
    delay(1000);
  }
}

void setup() {
  pinMode(BURNER_PIN, INPUT_PULLUP);
  if (digitalRead(BURNER_PIN) == LOW) {
    Serial.begin(115200);
    while (!Serial) {}
    burnerMode();
  }

  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  loadMapping();
  USB.begin();
}

void loop() {
  scanMatrix();
}
