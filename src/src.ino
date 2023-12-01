#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <Wire.h> 
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <semphr.h>
#include <DHT11.h> 
#include <FreeRTOSConfig.h>
#define DHTPIN 2 // DHT 数据引脚

DHT11 dht(DHTPIN);

String strRead = "";

int state_led[3] = {0,0,0};
int led_pin[] = {3, 4, 5};

const int LDR_PIN = 14;

int alert_led = 9;
int buzzer_pin = 10;
bool alert = false;
int ale_state = 0;//设备未处于警报状态

Adafruit_BMP085 bmp;

SemaphoreHandle_t Mutex_Alert;

TaskHandle_t start_handler;
TaskHandle_t datareceive_handler;
TaskHandle_t datasend_handler;
TaskHandle_t blink_handler;

//任务函数
void TaskStart(void* pvParameters);
void TaskDataReceive(void* pvParameters);
void TaskDataSend(void* pvParameters);
void TaskBlink(void* pvParameters);

//功能函数
void Alert_Led(bool ale);
void OpLight();

void setup() {
  Serial.begin(9600);//串口初始化
  bmp.begin();
  Mutex_Alert = xSemaphoreCreateMutex();

  xSemaphoreGive(Mutex_Alert);

  pinMode(alert_led, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  for(int i = 0; i < 3; i++){
    pinMode(led_pin[i], OUTPUT);
    digitalWrite(led_pin[i], LOW);
  }
  digitalWrite(alert_led, LOW);
  analogWrite(buzzer_pin, 0);//引脚初始化

  xTaskCreate(TaskStart,
              "Start",
              128,
              NULL, 
              0,
              &start_handler);
  vTaskStartScheduler();    // 开启任务调度
}

void loop() {}

//任务函数
void TaskStart(void *pvParameters)
{
  
  taskENTER_CRITICAL();   // 进入临界区
  xTaskCreate(TaskDataReceive,
                "DataReceive",
                128,
                NULL, 
                configMAX_PRIORITIES-1,
                &datareceive_handler);

  xTaskCreate(TaskDataSend,
              "DataSend",
              128,
              NULL, 
              0,
              &datasend_handler);

  xTaskCreate(TaskBlink,
              "Blink",
              128,
              NULL, 
              1,
              &blink_handler);
  vTaskDelete(start_handler);  //删除start_task
  taskEXIT_CRITICAL();    // 退出临界区
}

void TaskDataReceive(void *pvParameters)
{
  for (;;)
  {
    while (Serial.available() > 0){
      xSemaphoreTake(Mutex_Alert, portMAX_DELAY);
      strRead += char(Serial.read());
      if(strRead == "0")  state_led[0] = 0, state_led[1] = 0, state_led[2] = 0;
      else if(strRead == "1") state_led[0] = 1, state_led[1] = 0, state_led[2] = 0;
      else if(strRead == "2") state_led[0] = 0, state_led[1] = 1, state_led[2] = 0;
      else if(strRead == "3") state_led[0] = 1, state_led[1] = 1, state_led[2] = 0;
      else if(strRead == "4") state_led[0] = 0, state_led[1] = 0, state_led[2] = 1;
      else if(strRead == "5") state_led[0] = 1, state_led[1] = 0, state_led[2] = 1;
      else if(strRead == "6") state_led[0] = 0, state_led[1] = 1, state_led[2] = 1;
      else if(strRead == "7") state_led[0] = 1, state_led[1] = 1, state_led[2] = 1;
      OpLight();
      strRead="";
      xSemaphoreGive(Mutex_Alert);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void TaskDataSend(void *pvParameters)
{
  float temp, hum, lux, pre=0;
  pre = bmp.readPressure()/100; 
  for (;;)
  {
    hum = dht.readHumidity();
    Serial.print(F("Hum:"));
    Serial.println(hum);//湿度数据
    temp = dht.readTemperature();
    Serial.print(F("Tem:"));
    Serial.println(temp);//温度数据
    lux = analogRead(LDR_PIN);
    Serial.print(F("Lux:"));
    Serial.println(lux);//光强数据
    Serial.print(F("Pre:"));
    Serial.println(pre);//气压数据

    //报警判断
    if(temp > 45) alert = true;
    else  alert = false;

    Alert_Led(alert);
    Serial.print(F("Ale:"));
    Serial.println(alert);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void TaskBlink(void *pvParameters)
{
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay( 250 / portTICK_PERIOD_MS );
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay( 250 / portTICK_PERIOD_MS );
  }
}

//功能函数
void Alert_Led(bool ale){
  if(ale && !ale_state){
    xSemaphoreTake(Mutex_Alert, portMAX_DELAY);
    digitalWrite(alert_led, HIGH);
    analogWrite(buzzer_pin, 128);
    ale_state = 1;
    {
      digitalWrite(led_pin[0], LOW);
      digitalWrite(led_pin[1], LOW);
      digitalWrite(led_pin[2], LOW);
    }
  }
  if(!ale && ale_state){
    digitalWrite(alert_led, LOW);
    analogWrite(buzzer_pin, 0);
    ale_state = 0;
    OpLight();
    xSemaphoreGive(Mutex_Alert);
  }
  return;
}

void OpLight()
{
  for(int i = 0; i < 3; i ++)
  {
    if(state_led[i] == 1){
      digitalWrite(led_pin[i], HIGH);
    }
    else{
      digitalWrite(led_pin[i], LOW);
    }
  }
  return 0;
}
