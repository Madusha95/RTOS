#if CONFIG_FREERTOS_UNICORE
#define ESP_RUNING_CORE 0
#else
#define ESP_RUNING_CORE 1
#endif

//msg to print 
const char msg[] = "freetos impleneted in ESP32";

//Task Handlers 
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

void startTask(void *pvParameters){
  (void) pvParameters;
  int msgLen = strlen(msg);
  while(1){
    Serial.println();
    for (int i =0 ; i < msgLen; i++){
      Serial.print(msg[i]) ;
    }
    Serial.println();
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void secondTask(void *pvParameters){
  (void) pvParameters;

  while(1){
    Serial.println("#");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void setup() {
Serial.begin(9600);
vTaskDelay(1000/portTICK_PERIOD_MS);
Serial.println("FreeRTOS Demo");

Serial.println("Setup and loop tasks running on core:");
Serial.println(xPortGetCoreID());
Serial.println("with proirity: ");
Serial.println(uxTaskPriorityGet(NULL));

xTaskCreatePinnedToCore(
  startTask,
  "Task1",
  1024,
  NULL,
  1,
  &task1,
  ESP_RUNING_CORE
);

xTaskCreatePinnedToCore(
  secondTask,
  "Task2",
  1024,
  NULL,
  2,
  &task2,
  ESP_RUNING_CORE
);

}

void loop() {
  //suspent high priority task
  for(int i;i<3;i++){
    vTaskSuspend(task2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task2);
    vTaskDelay(2000/portTICK_PERIOD_MS);  
  }

  //lowest pririty task 
    if(task1 !=NULL){
      vTaskDelete(task1);
      task1 =NULL;
    }

}
