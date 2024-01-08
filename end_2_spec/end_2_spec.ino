#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define REPORTING_PERIOD_MS 1000  // 感測器間隔
#define MAX_DATA_SIZE 10  // 資料陣列大小
#define SECRET_SSID "3715"
#define SECRET_PASS "12345678"

struct SensorData {
  int heartRate;
  int spo2;
};

PulseOximeter pox;
uint32_t tsLastReport = 0;
WiFiClient client;
SensorData dataBuffer[MAX_DATA_SIZE];  // 用來儲存資料的陣列
int dataCount = 0;  // 目前陣列中的資料數量
String URL = "http://passing.sfs.tw:20001/receive-data";

int flag = true;  // 程式只跑一遍

void onBeatDetected()
{
  Serial.println("Beat!");
}

// 發送數據
void sendData(SensorData *data, int count) {
  HTTPClient http;

  // 啟動HTTP客戶端
  http.begin(client, URL);
  http.addHeader("Content-Type", "application/json");

  // 創建JSON格式的數據
  String jsonData = "[";
  for(int i = 0; i < count; i++) {
    jsonData += "{\"heartRate\":" + String(data[i].heartRate) + ",\"spo2\":" + String(data[i].spo2) + "}";
    if(i < count - 1) jsonData += ",";
  }
  jsonData += "]";

  // 發送POST請求
  int httpResponseCode = http.POST(jsonData);

  if(httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  // 結束HTTP連接
  http.end();
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); 
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SECRET_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    Serial.print(".");
    delay(5000);
  }
  Serial.println("\nConnected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Initializing pulse oximeter..");
  if(!pox.begin()) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
  pox.update();

  if(millis() - tsLastReport > REPORTING_PERIOD_MS && flag) {
    if(pox.getHeartRate()>0 && pox.getSpO2()>0) {
      // 有數值時進行輸出
      Serial.print(dataCount+1);
      Serial.print(": Heart rate:");
      Serial.print(pox.getHeartRate());
      Serial.print("bpm / SpO2:");
      Serial.print(pox.getSpO2());
      Serial.println("%");

      // 將資料加入陣列
      dataBuffer[dataCount].heartRate = pox.getHeartRate();  // 儲存心率
      dataBuffer[dataCount].spo2 = pox.getSpO2();  // 儲存血氧
      dataCount = dataCount + 1;

      if(dataCount >= MAX_DATA_SIZE) {
        dataCount = 0;
        sendData(dataBuffer, MAX_DATA_SIZE);
        flag = false;
        Serial.println("Program termination !");
      }
    } else {
      // 沒有數值時重新初始化陣列
      dataCount = 0;
      memset(dataBuffer, 0, sizeof(dataBuffer));
    }
    tsLastReport = millis();
  }
}
