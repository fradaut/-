#include <Ultrasonic.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 //OLED顯示寬度pixels
#define SCREEN_HEIGHT 64 //OLED顯示高度pixels
#define OLED_RESET  -1 //共用板子上的reset pin
#define SCREEN_ADDRESS 0x3C //0x3D for 128x64, 0x3C for 128x32

//設定超音波感測器連接的Pin腳
const int trigPin = 5; 
const int echoPin = 18;
//設定LED燈泡連接的Pin腳
const int ledPin = 17;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //初始化display物件並設定螢幕長寬
Ultrasonic ultrasonic(trigPin, echoPin); //初始化ultrasonic物件並設定Pin腳

void setup() {
  //設定輸出的Pin腳
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //螢幕初始化失敗
    Serial.println(F("SSD1306 allocation failed")); //在console顯示
    for(;;); //迴圈阻塞程式
  }
  display.clearDisplay(); //清除螢幕上緩衝的資料
}

void loop() {
  int distance; //宣告distance變數
  distance = ultrasonic.read(); //將感測器讀到的值丟到變數

  bool ledStatus= false; //定義LED的狀態為false
  if(distance <= 15) ledStatus = true; //如果感測器測到的值小於或等於15，將LED狀態設為true
  else ledStatus = false; //否則仍維持false

  display.clearDisplay();
  display.setTextSize(0); //設定螢幕文字大小
  display.setTextColor(WHITE); //設定螢幕的字體顏色為白色
  display.setCursor(0,20); //從第0行，第20列開始顯示
  if(ledStatus) { //如果LED狀態為true執行以下{}
    digitalWrite(LED_BUILTIN, HIGH); //開啟板子內建的LED
    digitalWrite(ledPin, HIGH); //開啟外部LED
    display.println("LED status: On"); //螢幕顯示LED狀態
    display.print(distance); //顯示distance的值
    display.print(" cm");
  }else { //否則，執行以下{}
    digitalWrite(LED_BUILTIN, LOW); //關閉板子內建的LED
    digitalWrite(ledPin, LOW); //關閉外部LED
    display.println("LED status: Off");
    display.print(distance);
    display.print(" cm");
  }

  display.display();
}
