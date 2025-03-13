#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#define LED_PIN D10

// UUIDs for the service and characteristic
static BLEUUID serviceUUID("89ecd0e9-5c79-4563-a636-42cf27682a77");
static BLEUUID charUUID("3a695770-6e26-4fa7-aadc-0c5387dde9df");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

// Variables to track received distance data
float currentDistance = 0.0;
float maxDistance = -INFINITY;
float minDistance = INFINITY;
int dataCount = 0;

// Function to handle received BLE notifications
static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{

  // Convert received data from byte array to string
  String receivedString = "";
  for (int i = 0; i < length; i++)
  {
    receivedString += (char)pData[i];
  }

  // Convert string to float
  int commaIndex = receivedString.indexOf(',');  // Find the position of the first comma
  String numberStr = receivedString.substring(0, commaIndex);  // Extract the substring before the comma
  float receivedDistance = numberStr.toFloat();

  // Ignore invalid readings
  if (receivedDistance <= 0)
  {
    Serial.println("Received invalid data, skipping...");
    return;
  }

  // Update tracking variables
  currentDistance = receivedDistance;
  if (receivedDistance > maxDistance)
    maxDistance = receivedDistance;
  if (receivedDistance < minDistance)
    minDistance = receivedDistance;

  // Increment received data count
  dataCount++;

  // Print the received, max, and min distances
  Serial.print("Current acclx: ");
  Serial.print(currentDistance);
  if (currentDistance > 2)
  {
    digitalWrite(LED_PIN, HIGH); // Turn on LED
    delay(200);            // Wait for the mapped delay time
    digitalWrite(LED_PIN, LOW);  // Turn off LED
  }
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient)
  {
    connected = false;
    Serial.println("Disconnected");
  }
};

// Function to connect to the BLE Server
bool connectToServer()
{
  Serial.print("Connecting to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  if (pClient->connect(myDevice))
  {
    Serial.println("Connected to server");
  }
  else
  {
    Serial.println("Failed to connect to server");
    return false;
  }

  // Get the service from the server
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.println("Failed to find service UUID");
    pClient->disconnect();
    return false;
  }
  Serial.println("Found service");

  // Get the characteristic from the service
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.println("Failed to find characteristic UUID");
    pClient->disconnect();
    return false;
  }
  Serial.println("Found characteristic");

  // Subscribe to notifications
  if (pRemoteCharacteristic->canNotify())
  {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  return true;
}

// BLE Scanning callback
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("Found device: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client...");
  pinMode(LED_PIN, OUTPUT);
  BLEDevice::init("");

  // Start BLE scanning
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop()
{
  if (doConnect)
  {
    if (connectToServer())
    {
      Serial.println("Connected to BLE Server.");
    }
    else
    {
      Serial.println("Failed to connect to server.");
    }
    doConnect = false;
  }

  if (connected)
  {
    delay(1000); // Avoid overwhelming the BLE connection
  }
  else if (doScan)
  {
    BLEDevice::getScan()->start(0);
  }
}

// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>

// #define LED_PIN D10

// static BLEUUID serviceUUID("89ecd0e9-5c79-4563-a636-42cf27682a77");
// static BLEUUID charUUID("3a695770-6e26-4fa7-aadc-0c5387dde9df");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic *pRemoteCharacteristic;
// static BLEAdvertisedDevice *myDevice;

// // Function to handle received BLE notifications
// static void notifyCallback(
//     BLERemoteCharacteristic *pBLERemoteCharacteristic,
//     uint8_t *pData,
//     size_t length,
//     bool isNotify)
// {
//     // Convert received data from byte array to string
//     String receivedString = "";
//     for (int i = 0; i < length; i++)
//     {
//         receivedString += (char)pData[i];
//     }

//     // Split the received data into X, Y, Z acceleration components
//     float accX = 0.0, accY = 0.0, accZ = 0.0;
//     int commaIndex1 = receivedString.indexOf(',');
//     int commaIndex2 = receivedString.indexOf(',', commaIndex1 + 1);

//     if (commaIndex1 != -1 && commaIndex2 != -1)
//     {
//         accX = receivedString.substring(0, commaIndex1).toFloat();
//         accY = receivedString.substring(commaIndex1 + 1, commaIndex2).toFloat();
//         accZ = receivedString.substring(commaIndex2 + 1).toFloat();
//     }

//     // Calculate the total acceleration magnitude (optional)
//     float accelerationMagnitude = sqrt(accX * accX + accY * accY + accZ * accZ);

//     // Control the LED delay based on the magnitude of the acceleration
//     // We will map the acceleration magnitude to a delay range (100 ms to 1000 ms)
//     int delayTime = map(accelerationMagnitude * 100, 0, 25, 100, 1000); // Adjust the scaling factor as necessary

//     // Print the calculated delay
//     Serial.print("Calculated Delay: ");
//     Serial.println(delayTime);

//     // Control the LED delay based on the acceleration magnitude
//     digitalWrite(LED_PIN, HIGH); // Turn on LED
//     delay(delayTime);            // Wait for the mapped delay time
//     digitalWrite(LED_PIN, LOW);  // Turn off LED
//     delay(delayTime);            // Wait for the mapped delay time
// }

// class MyClientCallback : public BLEClientCallbacks
// {
//     void onConnect(BLEClient *pclient) {}

//     void onDisconnect(BLEClient *pclient)
//     {
//         connected = false;
//         Serial.println("Disconnected");
//     }
// };

// // Function to connect to the BLE Server
// bool connectToServer()
// {
//     Serial.print("Connecting to ");
//     Serial.println(myDevice->getAddress().toString().c_str());

//     BLEClient *pClient = BLEDevice::createClient();
//     pClient->setClientCallbacks(new MyClientCallback());

//     if (pClient->connect(myDevice))
//     {
//         Serial.println("Connected to server");
//     }
//     else
//     {
//         Serial.println("Failed to connect to server");
//         return false;
//     }

//     // Get the service from the server
//     BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
//     if (pRemoteService == nullptr)
//     {
//         Serial.println("Failed to find service UUID");
//         pClient->disconnect();
//         return false;
//     }
//     Serial.println("Found service");

//     // Get the characteristic from the service
//     pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//     if (pRemoteCharacteristic == nullptr)
//     {
//         Serial.println("Failed to find characteristic UUID");
//         pClient->disconnect();
//         return false;
//     }
//     Serial.println("Found characteristic");

//     // Subscribe to notifications
//     if (pRemoteCharacteristic->canNotify())
//     {
//         pRemoteCharacteristic->registerForNotify(notifyCallback);
//     }

//     connected = true;
//     return true;
// }

// // BLE Scanning callback
// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
// {
//     void onResult(BLEAdvertisedDevice advertisedDevice)
//     {
//         Serial.print("Found device: ");
//         Serial.println(advertisedDevice.toString().c_str());

//         if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
//         {
//             BLEDevice::getScan()->stop();
//             myDevice = new BLEAdvertisedDevice(advertisedDevice);
//             doConnect = true;
//             doScan = true;
//         }
//     }
// };

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(LED_PIN, OUTPUT);  // Set LED_PIN as output

//     Serial.println("Starting BLE Client...");

//     BLEDevice::init("");

//     // Start BLE scanning
//     BLEScan *pBLEScan = BLEDevice::getScan();
//     pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//     pBLEScan->setInterval(1349);
//     pBLEScan->setWindow(449);
//     pBLEScan->setActiveScan(true);
//     pBLEScan->start(5, false);
// }

// void loop()
// {
//     if (doConnect)
//     {
//         if (connectToServer())
//         {
//             Serial.println("Connected to BLE Server.");
//         }
//         else
//         {
//             Serial.println("Failed to connect to server.");
//         }
//         doConnect = false;
//     }

//     if (connected)
//     {
//         delay(1000); // Avoid overwhelming the BLE connection
//     }
//     else if (doScan)
//     {
//         BLEDevice::getScan()->start(0);
//     }
// }
