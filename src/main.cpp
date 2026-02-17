// Copyright (c) 2026 misetteichan
// Licensed under the MIT License. See LICENSE file for details.

#include <Avatar.h>
#include <WiFi.h>
#include <Matter.h>

#include "matteravatar.hpp"
#include "matterfastled.hpp"

constexpr char SSID[] = "";
constexpr char PASSWORD[] = "";
constexpr uint8_t LED_PIN = 38;
constexpr uint8_t LED_COUNT = 18;

MatterAvatar avatar;
MatterFastLED<LED_PIN, LED_COUNT> light;
MatterGenericSwitch genericSwitch;

bool connectWiFi() {
  M5.Display.printf("Connect");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  uint8_t count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    M5.Display.print(".");
    if (++count > 20) {
      M5.Display.printf("failed");
      return false;
    }
    delay(500);
  }
  return true;
}

void setup() {
  M5.begin();

  if (!connectWiFi()) {
    delay(1000);
    ESP.restart();
  }

  avatar.begin();
  light.begin(false, 0);
  genericSwitch.begin();
  Matter.begin();
}

void loop() {
  if (!Matter.isDeviceCommissioned()) {
    printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());

    while (!Matter.isDeviceCommissioned()) {
      printf(".");
      delay(500);
    }
    return;
  }

  M5.update();
  if (M5.BtnA.wasClicked()) {
    genericSwitch.click();
  }
}
