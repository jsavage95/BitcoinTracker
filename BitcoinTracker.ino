//ARDUINO HTTPS
#include <WiFiS3.h>           // For Arduino UNO R4 WiFi
#include <WiFiSSLClient.h>    // For HTTPS connections
#include <ArduinoJson.h>
#include <LiquidCrystal.h>    // For LCD shield

#include "secrets.h"
#include "helpers.h"

// LCD pin configuration for Duinotech 2x16 LCD Keypad Shield
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

WiFiSSLClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Init LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  IPAddress ip;
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED || (ip = WiFi.localIP()) == INADDR_NONE) && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED && ip != INADDR_NONE) {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print(ip);
    Serial.println();
    Serial.print("IP Address: ");
    Serial.println(ip);
    delay(1500);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    Serial.println("WiFi connection failed.");
    return;
  }
}



void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecting to CoinGecko...");
    if (client.connect("api.coingecko.com", 443)) {
      client.println("GET /api/v3/simple/price?ids=bitcoin&vs_currencies=usd HTTP/1.1");
      client.println("Host: api.coingecko.com");
      client.println("Connection: close");
      client.println();

      // Skip headers
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
      }

      String response = "";
      bool jsonStarted = false;

      while (client.available()) {
        String line = client.readStringUntil('\n');
        line.trim();

        // Start capturing once we hit the JSON (usually after chunk length)
        if (line.startsWith("{") && line.endsWith("}")) {
          response = line;
          break;
        }
      }

      Serial.println("Raw response:");
      Serial.println(response);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        int price = doc["bitcoin"]["usd"];
        String formattedPrice = formatPriceWithComma(price);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BTC/USD");
        lcd.setCursor(0, 1);
        lcd.print("$");
        lcd.print(formattedPrice);
      } else {
        Serial.print("JSON Parse Error: ");
        Serial.println(error.c_str());
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Parse Error");
      }

      client.stop();
    } else {
      Serial.println("HTTPS Connection Failed");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connect Failed");
    }
  } else {
    Serial.println("WiFi Disconnected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Lost");
  }

  delay(120000);  // Refresh every 2 minutes
}


