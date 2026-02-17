# matternyan

Matterなｱﾄﾑﾆｬﾝ の実装例 🐈

## 設定

### LED設定
`src/main.cpp` で以下を変更：
```cpp
constexpr uint8_t LED_PIN = 38;      // GPIO ピン番号
constexpr uint8_t LED_COUNT = 18;    // LED 個数
```

### WiFi設定
`src/main.cpp` でSSIDとパスワードを設定：
```cpp
constexpr char SSID[] = "your-ssid";
constexpr char PASSWORD[] = "your-password";
```

## ビルドとアップロード

### ビルド
```bash
platformio run
```

### アップロード
```bash
platformio run --target upload
```
