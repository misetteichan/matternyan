// Copyright (c) 2026 misetteichan
// Licensed under the MIT License. See LICENSE file for details.

#pragma once

#include <Matter.h>
#include <Avatar.h>

class MatterAvatar : public MatterEnhancedColorLight {
  m5avatar::Avatar avatar = {};
  m5avatar::ColorPalette colorPalette = {}; 
  bool state = false;
  espHsvColor_t color = {};
  uint8_t brightness = 0;
  uint16_t temperature = 0;
public:
  virtual bool begin(bool initialState = false, espHsvColor_t colorHSV = {21, 216, 25}, uint8_t newBrightness = 25, uint16_t colorTemperature = 454) override {
    if (!MatterEnhancedColorLight::begin(initialState, colorHSV, newBrightness, colorTemperature)) {
      return false;
    }
    state = initialState;
    color = colorHSV;
    brightness = newBrightness;
    temperature = colorTemperature;

    avatar.setScale(0.55f);
    avatar.setPosition(-60, -95);
    avatar.init(1);
    update();

    onChangeOnOff([this](bool o) {
      M5_LOGD("onChangeOnOff(%d)", o);
      state = o;    
      update();
      return true;
    });
    onChangeBrightness([this](uint8_t b) {
      M5_LOGD("onChangeBrightness(%d)", b);
      brightness = b;
      update();
      return true;
    });
    onChangeColorHSV([this](espHsvColor_t hsv) {
      M5_LOGD("onChangeColorHSV(%d, %d, %d)", hsv.h, hsv.s, hsv.v);
      color = hsv;
      update();
      return true;
    });
    onChangeColorTemperature([this](uint16_t t){
      M5_LOGD("onChangeColorTemperature(%d)", t);
      temperature = t;
      update();
      return true;
    });
    return true;
  }

private:
  static uint8_t scale(uint8_t v, uint8_t s) {
    return static_cast<uint8_t>((uint16_t(v) * uint16_t(s) + 127) / 255);
  }

  static void scale(espRgbColor_t& v, const espRgbColor_t& s) {
    v.r = scale(v.r, s.r);
    v.g = scale(v.g, s.g);
    v.b = scale(v.b, s.b);
  }

  static void scale(espRgbColor_t& v, uint8_t s) {
    scale(v, espRgbColor_t{s,s,s,});
  }

  static uint8_t clamp(int x) {
    if (x < 0) return 0;
    if (x > 255) return 255;
    return static_cast<uint8_t>(x);
  }

  static uint16_t toKelvin(uint16_t mired, uint16_t mired_min = 140, uint16_t mired_max = 500) {
    if (mired < mired_min) mired = mired_min;
    if (mired > mired_max) mired = mired_max;

    uint32_t k = (1000000u + (mired / 2)) / mired;
    if (k < 1000) k = 1000;
    if (k > 40000) k = 40000;
    return static_cast<uint16_t>(k);
  }

  static espRgbColor_t toWhitePoint(uint16_t kelvin) {
    const auto t = kelvin / 100.0f;
    float r, g, b;

    // Red
    if (t <= 66.0f) {
      r = 255.0f;
    } else {
      r = 329.698727446f * powf(t - 60.0f, -0.1332047592f);
    }

    // Green
    if (t <= 66.0f) {
      g = 99.4708025861f * logf(t) - 161.1195681661f;
    } else {
      g = 288.1221695283f * powf(t - 60.0f, -0.0755148492f);
    }

    // Blue
    if (t >= 66.0f) {
      b = 255.0f;
    } else if (t <= 19.0f) {
      b = 0.0f;
    } else {
      b = 138.5177312231f * logf(t - 10.0f) - 305.0447927307f;
    }
    
    return {
      clamp((int)lroundf(r)),
      clamp((int)lroundf(g)),
      clamp((int)lroundf(b))
    };
  }

  static espRgbColor_t complementRgb(const espRgbColor_t& c) {
    return {
      static_cast<uint8_t>(255u - c.r),
      static_cast<uint8_t>(255u - c.g),
      static_cast<uint8_t>(255u - c.b)
    };
  }

  void update() {
    // HSV -> RGB
    auto rgb = state ? espHsvColorToRgbColor(color) : espRgbColor_t{0,0,0};
    // HomeKit色温度を白色点として掛ける（ホワイトバランス）
    const auto k = toKelvin(temperature);
    const auto w = toWhitePoint(k);
    scale(rgb, w);
    // 明るさ適用（最後にまとめて）
    scale(rgb, brightness);

    colorPalette.set(COLOR_BACKGROUND, M5.Display.color565(rgb.r, rgb.g, rgb.b));

    const auto comp = complementRgb(rgb);
    colorPalette.set(COLOR_PRIMARY, M5.Display.color565(comp.r, comp.g, comp.b));
     
    avatar.setColorPalette(colorPalette);
    avatar.setMouthOpenRatio(brightness / 255.0f);
  }
};
