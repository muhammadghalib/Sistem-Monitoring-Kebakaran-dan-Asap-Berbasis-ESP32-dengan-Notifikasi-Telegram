#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define BOTtoken      "7219264499:AAEi3NSo-LZ6cbxPIskxrtShkc7YTyxyXyo"
#define CHAT_ID       "1267581166"
#define GAS_DO_PIN    34
#define FLAME_DO_PIN  35
#define LED_PIN       2

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

char SSID[] = "G";
char PASS[] = "1234567gg";

bool lastGasDetection         = HIGH;
bool lastFlameDetection       = HIGH;
bool gasConditionSafe         = true;
bool flameConditionSafe       = true;
bool lastSafeCondition        = false;
bool welcomeMessageDisplayed  = false;

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(" \nARCoS - Workshop FAIRCOS 2024");
    Serial.println(String("Connecting to ") + SSID);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(String("Connected to ") + SSID);
  } else {
      Serial.println(String("Failed to connect ") + SSID);
  }
}

void connectToTelegramServer() {
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void welcomeBannerAndText() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  if (numNewMessages > 0) {
    for (int i = 0; i < numNewMessages; i++) {
      String chat_id = String(bot.messages[i].chat_id);
      if (chat_id == CHAT_ID) {
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        
        if (text == "/start") {
          String imageUrl = "https://raw.githubusercontent.com/muhammadghalib/faircos-asset/refs/heads/main/banner.jpg";
          bot.sendPhoto(CHAT_ID, imageUrl, "");

          String welcome = "Halo, " + from_name + ".\n\n";
          welcome += "Selamat Datang di Workshop FAIRCOS 2024 🤖.\n";
          bot.sendMessage(chat_id, welcome, "");

          welcomeMessageDisplayed = true;
        }
      }
    }
  }
}

void gasDetectionChecking() {
  int gasState = digitalRead(GAS_DO_PIN);

  if (gasState == HIGH && lastGasDetection != HIGH) {
    lastGasDetection = HIGH;
    gasConditionSafe = true;
  } else if (gasState == LOW && lastGasDetection != LOW) {
      bot.sendMessage(CHAT_ID, "Gas terdeteksi! 💨⚠️", "");
      Serial.println("Kondisi = Gas terdeteksi");
      lastGasDetection = LOW;
      gasConditionSafe = false;
  }
}

void flameDetectionChecking() {
  int flameState = digitalRead(FLAME_DO_PIN);

  if (flameState == HIGH && lastFlameDetection != HIGH) {
    lastFlameDetection = HIGH;
    flameConditionSafe = true;
  } else if (flameState == LOW && lastFlameDetection != LOW) {
      bot.sendMessage(CHAT_ID, "Api terdeteksi! 🔥⚠️", "");
      Serial.println("Kondisi = Api terdeteksi");
      lastFlameDetection = LOW;
      flameConditionSafe = false;
  }
}

void safeConditionChecking() {
  if (flameConditionSafe == true && gasConditionSafe == true && !lastSafeCondition) {
    bot.sendMessage(CHAT_ID, "Kondisi aman! ✅", "");
    Serial.println("Kondisi = Aman");
    digitalWrite(LED_PIN, LOW);
    lastSafeCondition = true;
  } else if (flameConditionSafe == false || gasConditionSafe == false) {
      digitalWrite(LED_PIN, HIGH);
      lastSafeCondition = false;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(GAS_DO_PIN, INPUT);
  pinMode(FLAME_DO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  connectToWifi();
  connectToTelegramServer();
}

void loop() {
    if (welcomeMessageDisplayed == false) {
        welcomeBannerAndText();
    } else if (welcomeMessageDisplayed == true) {
        gasDetectionChecking();
        flameDetectionChecking();
        safeConditionChecking();
    }
}