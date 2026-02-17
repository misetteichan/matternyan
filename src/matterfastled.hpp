// Copyright (c) 2026 misetteichan
// Licensed under the MIT License. See LICENSE file for details.

#pragma once

#include <Matter.h>
#include <FastLED.h>

template<size_t GPIO, size_t LEDS>
class MatterFastLED : public MatterDimmableLight {
  CRGB leds[LEDS];
  bool state;
  uint8_t brightness;
public:
  virtual bool begin(bool initialState = false, uint8_t initialBrightness = 64) override {
    if (!MatterDimmableLight::begin(initialState, initialBrightness)) {
      return false;
    }
    pinMode(GPIO, OUTPUT);
    digitalWrite(GPIO, LOW);
    FastLED.addLeds<SK6812, GPIO, GRB>(leds, LEDS);

    state = initialState;
    brightness = initialState;
    update();

    onChangeOnOff([this](bool s) {
      M5_LOGD("onChangeOnOff(%d)", s);
      state = s;
      update();
      return true;
    });
    onChangeBrightness([this](uint8_t b) {
      M5_LOGD("onChangeBrightness(%d)", b);
      brightness = b;
      update();
      return true;
    });

    return true;
  }
private:
  void update() {
    if (state) {
      for (auto i = 0; i < LEDS; ++i) {
        leds[i] = CRGB::White;
      }
      FastLED.setBrightness(static_cast<uint8_t>(map(brightness, 0, 255, 0, 20)));
    } else {
      FastLED.clear();
    }
    FastLED.show();
  }
};
