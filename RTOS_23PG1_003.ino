//Including Libraries 
#include <DHT.h>     
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "freertos/semphr.h"

//ESP32 Cor configaration
#if CONFIG_FREERTOS_UNICORE
#define ESP_RUNING_CORE 0
#else
#define ESP_RUNING_CORE 1
#endif

//PIN defining 
#define DHTPIN 18
#define DHTTYPE DHT11
#define MOTION_SENSOR_PIN 23
#define I2C_SDA 21
#define I2C_SCL 22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; // Use I2C interface

SemaphoreHandle_t motionSemaphore;

// Task handlers
static TaskHandle_t taskHandleDHT = NULL;
static TaskHandle_t taskHandleLED = NULL;
static TaskHandle_t taskHandleBMP = NULL;
static TaskHandle_t taskHandleMotion = NULL;

// Function prototypes
void readDHTTask(void *parameter);
void ledControlTask(void *parameter);
void readBMPTask(void *parameter);
void handleMotionTask(void *parameter);
void motionDetectedISR();

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring! Trying again..."));
    // Attempt to initialize BMP280 again
    for(int retries = 0; retries < 5; retries++) {
      if(bmp.begin(0x76)) {
        Serial.println("BMP280 initialization successful.");
        break;
      }
      vTaskDelay(500/portTICK_PERIOD_MS); // Wait half a second between retries
    }
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTION_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR_PIN), motionDetectedISR, RISING);

  // Initialize semaphore
  motionSemaphore = xSemaphoreCreateBinary();

  // Create tasks
  xTaskCreatePinnedToCore(readDHTTask, "Read_DHT11", 2048, NULL, 1, &taskHandleDHT, ESP_RUNING_CORE);
  xTaskCreatePinnedToCore(ledControlTask, "Control_LED", 2048, NULL, 1, &taskHandleLED, ESP_RUNING_CORE);
  xTaskCreatePinnedToCore(readBMPTask, "Read_BMP280", 2048, NULL, 1, &taskHandleBMP, ESP_RUNING_CORE);
  xTaskCreatePinnedToCore(handleMotionTask, "Handle_Motion", 2048, NULL, 2, &taskHandleMotion, ESP_RUNING_CORE); // Higher priority for motion handling

}

void loop() {
  // Empty, tasks are running
  for(int i;i<5;i++){
    vTaskSuspend(taskHandleDHT);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(taskHandleDHT);
    vTaskDelay(2000/portTICK_PERIOD_MS);  
  }

  //lowest pririty task 
    if(taskHandleLED !=NULL){
      vTaskDelete(taskHandleLED);
      taskHandleLED =NULL;
    }
}

void readDHTTask(void *parameter) {
  for (;;) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor! Retrying...");
      // Simple retry logic, if necessary
      vTaskDelay(5000/portTICK_PERIOD_MS);  // Wait longer before trying again
    } else {
      Serial.print("DHT11 - Humidity = ");
      Serial.print(humidity);
      Serial.print("% Temperature = ");
      Serial.print(temperature);
      Serial.println("C");
    }
    vTaskDelay(2000/portTICK_PERIOD_MS);;
  }
}

void ledControlTask(void *parameter) {
  for (;;) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    vTaskDelay(500/portTICK_PERIOD_MS);;
  }
}

void readBMPTask(void *parameter) {
  for (;;) {
    if (!bmp.begin(0x76)) {
      Serial.println("BMP280 failed to initialize. Retrying...");
      // Attempt to reinitialize BMP280
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay significantly before retrying
      continue; // Skip this loop iteration
    }
    float temp = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
    float altitude = (bmp.readAltitude(1013.25));
    Serial.print("BMP280 - Temperature = ");
    Serial.print(temp);
    Serial.print(" *C  Pressure = ");
    Serial.print(pressure);
    Serial.print(" hPa");
    Serial.print(" Approx altitude = ");
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void handleMotionTask(void *parameter) {
  for (;;) {
    if (xSemaphoreTake(motionSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("Motion detected! Handling event...");
      digitalWrite(LED_BUILTIN, HIGH); // Example action: LED ON for 10 seconds
      vTaskDelay(1000/portTICK_PERIOD_MS);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void motionDetectedISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(motionSemaphore, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
