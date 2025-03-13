// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_MPU6050.h>
// #include <Wire.h>

// #define SERVICE_UUID        "89ecd0e9-5c79-4563-a636-42cf27682a77"
// #define CHARACTERISTIC_UUID "3a695770-6e26-4fa7-aadc-0c5387dde9df"


// BLEServer* pServer = NULL;
// BLECharacteristic* pCharacteristic = NULL;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;

// Adafruit_MPU6050 mpu;
// float velocityX = 0.0, velocityY = 0.0, velocityZ = 0.0;
// float accX_offset = 0.0, accY_offset = 0.0, accZ_offset = 0.0;
// unsigned long lastUpdate = 0;

// class MyServerCallbacks : public BLEServerCallbacks {
//     void onConnect(BLEServer* pServer) {
//         deviceConnected = true;
//     };

//     void onDisconnect(BLEServer* pServer) {
//         deviceConnected = false;
//     }
// };

// void calibrateMPU6050() {
//     Serial.println("Calibrating MPU6050...");
//     float sumX = 0, sumY = 0, sumZ = 0;
//     int numSamples = 100;

//     for (int i = 0; i < numSamples; i++) {
//         sensors_event_t a, g, temp;
//         mpu.getEvent(&a, &g, &temp);
//         sumX += a.acceleration.x;
//         sumY += a.acceleration.y;
//         sumZ += a.acceleration.z;  
//         delay(10);
//     }
    
//     accX_offset = sumX / numSamples;
//     accY_offset = sumY / numSamples;
//     accZ_offset = sumZ / numSamples;
    
//     Serial.println("Calibration complete!");
//     Serial.print("Offsets: X: "); Serial.print(accX_offset);
//     Serial.print(", Y: "); Serial.print(accY_offset);
//     Serial.print(", Z: "); Serial.println(accZ_offset);
// }

// void setup() {
//     Serial.begin(115200);
//     Serial.println("Starting BLE work!");

//     BLEDevice::init("XIAO_ESP32S3");
//     pServer = BLEDevice::createServer();
//     pServer->setCallbacks(new MyServerCallbacks());
//     BLEService *pService = pServer->createService(SERVICE_UUID);
//     pCharacteristic = pService->createCharacteristic(
//         CHARACTERISTIC_UUID,
//         BLECharacteristic::PROPERTY_READ |
//         BLECharacteristic::PROPERTY_NOTIFY
//     );
//     pCharacteristic->addDescriptor(new BLE2902());
//     pService->start();
//     BLEDevice::startAdvertising();

//     Serial.println("Adafruit MPU6050 test!");
//     if (!mpu.begin()) {
//         Serial.println("Failed to find MPU6050 chip");
//         while (1) {
//             delay(10);
//         }
//     }
//     Serial.println("MPU6050 Found!");

//     mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//     mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//     mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//     Serial.println("");
//     delay(100);

//     calibrateMPU6050();
// }

// void loop() {
//     sensors_event_t a, g, temp;
//     mpu.getEvent(&a, &g, &temp);
//     unsigned long currentTime = millis();
//     float deltaTime = (currentTime - lastUpdate) / 1000.0;
//     lastUpdate = currentTime;

//     // 应用偏移修正
//     float correctedAccX = a.acceleration.x - accX_offset;
//     float correctedAccY = a.acceleration.y - accY_offset;
//     float correctedAccZ = a.acceleration.z - accZ_offset;

//     velocityX += correctedAccX * deltaTime;
//     velocityY += correctedAccY * deltaTime;
//     velocityZ += correctedAccZ * deltaTime;

//     // 误差修正：如果陀螺仪检测到几乎没有旋转，速度衰减
//     if (abs(g.gyro.x) < 0.02 && abs(g.gyro.y) < 0.02 && abs(g.gyro.z) < 0.02) {
//         velocityX *= 0.95;
//         velocityY *= 0.95;
//         velocityZ *= 0.95;
//     }

//     Serial.print("Acceleration X: "); Serial.print(correctedAccX);
//     Serial.print(", Y: "); Serial.print(correctedAccY);
//     Serial.print(", Z: "); Serial.print(correctedAccZ);
//     Serial.println(" m/s^2");

//     Serial.print("Velocity X: "); Serial.print(velocityX);
//     Serial.print(" m/s, Y: "); Serial.print(velocityY);
//     Serial.print(" m/s, Z: "); Serial.print(velocityZ);
//     Serial.println(" m/s");

//     if (deviceConnected) {
//         String velocityData = String(correctedAccX) + "," + String(correctedAccY) + "," + String(correctedAccZ);
//         pCharacteristic->setValue(velocityData.c_str());
//         pCharacteristic->notify();
//     }
//     delay(100);
// }


#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
#define LED_PIN D10
#define PRESSURE_PIN D0 
#define SWITCH_PIN D1   
#define VIBRATE_PIN D2

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  pinMode(LED_PIN, OUTPUT);
  pinMode(PRESSURE_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

//   Serial.println("");
//   delay(500);
//   digitalWrite(LED_PIN, HIGH);
//   delay(500);
//   digitalWrite(LED_PIN, LOW);

//   int pressureValue = analogRead(PRESSURE_PIN);
//   Serial.print("fsr: ");
//   Serial.println(pressureValue);
//   delay(500);



//   int switchState = analogRead(SWITCH_PIN);
//   Serial.println(switchState);
    

}