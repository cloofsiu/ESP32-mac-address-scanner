#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BLEDevice.h>

// 創建LCD對象，地址0x27，16列2行
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin = 0; // BOOT按鈕

struct DeviceInfo {
  String name;
  String macAddress;
};

std::vector<DeviceInfo> devices;
int currentIndex = 0;
bool buttonPressed = false;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  pinMode(buttonPin, INPUT);

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); 
  BLEScanResults foundDevices = pBLEScan->start(5); 

  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    String macAddress = device.getAddress().toString().c_str();
    String name = device.getName().c_str();
    if (name.length() == 0) name = "Unknown"; // 如果沒有設備名稱，設置為“Unknown”
    devices.push_back({name, macAddress});
    Serial.println("Found Device: " + name + " (" + macAddress + ")");
  }

  if (devices.size() > 0) {
    updateDisplay();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("No devices found");
  }
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);

  // 分成兩行顯示名稱和MAC地址
  String name = devices[currentIndex].name;
  String macAddress = devices[currentIndex].macAddress;

  if (name.length() > 16) {
    name = name.substring(0, 16); // 截取名稱前16個字符
  }

  lcd.print(name);

  lcd.setCursor(0, 1);

  // 顯示MAC地址，去掉「:」符號
  for (int i = 0; i < macAddress.length(); i++) {
    if (macAddress.charAt(i) != ':') {
      lcd.print(macAddress.charAt(i));
    }
  }
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      delay(200); 
      currentIndex++;
      if (currentIndex >= devices.size()) {
        currentIndex = 0;
      }
      updateDisplay();
    }
  } else {
    buttonPressed = false;
  }
}
