#include <LiquidCrystal.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const int rs = 16, en = 14, d4 = 12, d5 = 13, d6 = 5, d7 = 0; //Display Settings
const char* MQTT_BROKER = ""; // MQTT Broker IP
boolean display_status = true; // Setting Display backlight on/off
const int transistor = 4; // Transitor for Backlight

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);
    lcd.begin(16, 2);
    pinMode (transistor, OUTPUT);
}
 
void setup_wifi() {
 
    WiFiManager wifiManager;

    wifiManager.autoConnect();

}

void callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    String top = topic;
    for (byte i = 0; i < length; i++) {
        char tmp = char(payload[i]);
        msg += tmp;
    }

    Serial.println(msg);

    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, payload, length);
    
    // Test if parsing succeeds.
    if (error) {
      if (top == "KVB_status/display_status")
      {
          if (msg == "ON")
          {
            display_status = true;
          }
          else
          {
            display_status = false;
          }
      }
      Serial.println(display_status);
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    
    if (display_status)
    {
        digitalWrite (transistor, HIGH);
    }

    String Linie = doc["Linie"];
    String message = doc["message"];
    Serial.println(Linie);
    Serial.println(message);
    
    lcd.clear();

    //TODO: bottom Text to long
    
    lcd.setCursor(0, 0);
    lcd.print(Linie);
    lcd.setCursor(0, 1);
    lcd.print(message);
    delay(5000);
    digitalWrite (transistor, LOW);


    
    
    
}

void loop() {
    if (!client.connected()) {
        while (!client.connected()) {
            client.connect("ESP8266Client");
            client.subscribe("KVB_status/#");
            delay(100);
        }
    }
    client.loop();
}
