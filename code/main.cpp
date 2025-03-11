#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define MOTOR_PIN 4  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool motorState = false; 
int displayNumber = 0;   

#define SERVICE_UUID        "a1ad2f66-376c-478c-b073-f8e7d1ebd65e"
#define MOTOR_CHARACTERISTIC_UUID "420fa05b-393d-41f4-915c-6f7f68ac1f7a"
#define DISPLAY_CHARACTERISTIC_UUID "9991c9b8-4a86-4397-9f51-275fa6a3c6e9"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            if (pCharacteristic->getUUID().toString() == MOTOR_CHARACTERISTIC_UUID) {
                if (value == "ON") {
                    digitalWrite(MOTOR_PIN, HIGH);
                    motorState = true;
                } else if (value == "OFF") {
                    digitalWrite(MOTOR_PIN, LOW);
                    motorState = false;
                }
            } 
            else if (pCharacteristic->getUUID().toString() == DISPLAY_CHARACTERISTIC_UUID) {
                displayNumber = atoi(value.c_str());
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED 初始化失败");
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);


    BLEDevice::init("ESP32C3_BLE");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *motorCharacteristic = pService->createCharacteristic(
        MOTOR_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    motorCharacteristic->setCallbacks(new MyCallbacks());

    BLECharacteristic *displayCharacteristic = pService->createCharacteristic(
        DISPLAY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    displayCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    Serial.println("蓝牙已启动，等待连接...");
}

void loop() {
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 10000) {
        lastUpdateTime = millis();
        display.clearDisplay();
        display.setCursor(20, 20);
        display.print("Number:");
        display.setCursor(50, 40);
        display.print(displayNumber);
        display.display();
    }
}
