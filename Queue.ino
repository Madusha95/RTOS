#if CONFIG_FREERTOS_UNICORE
#define ESP_RUNING_CORE 0
#else
#define ESP_RUNING_CORE 1
#endif

//Queue length
static const u_int8_t QueueLength = 5;
//Global varable 
static QueueHandle_t Queuemessage;

void printMessage(void *pvParameters){
  (void) pvParameters;
  //local variable 
  int item;

  while(1){

    if(xQueueReceive(Queuemessage,(void *)&item,0) == pdTRUE){
      Serial.print(item);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void setup(){
  Serial.begin(9600);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.print("RTOS Queue Demo");

  //create queue 
  Queuemessage = xQueueCreate(QueueLength,sizeof(int));

  xTaskCreatePinnedToCore(
    printMessage,
    "printMessage",
    1024,
    NULL,
    1,
    NULL,
    ESP_RUNING_CORE
  );
  
}

void loop(){
  static int num =0;

  if(xQueueSend(Queuemessage,(void *)&num,10) != pdTRUE){
    Serial.println("Queue is full");
  }
  num++;
  vTaskDelay(1000/portTICK_PERIOD_MS);
}
