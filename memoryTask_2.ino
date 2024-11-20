#if CONFIG_FREERTOS_UNICORE
#define ESP_RUNING_CORE 0
#else
#define ESP_RUNING_CORE 1
#endif

void memoryTask(void *pvParameters){
  (void) pvParameters;
 
  while(1){
   int a =1;
   int b[100];
   for(int i = 0 ; i< 100; i++){
    b[i] = a+1;
   }
   Serial.println(b[0]);
   Serial.println("Remaining Stack Size(word size):");
   Serial.println(uxTaskGetStackHighWaterMark(NULL));

   Serial.println("Free heap before malloc (bytes)");
   Serial.println(xPortGetFreeHeapSize());

  int *prt = (int*) pvPortMalloc(1024*sizeof(int));

    if(prt == NULL){
      Serial.print("Not enough memory");
    }
    else{
      for (int i = 0; i<1024; i++){
        prt[i] = 5 ;
      }
    }
  Serial.println("Free heap AFTER malloc (bytes)");
  Serial.println(xPortGetFreeHeapSize());

  vPortFree(prt);
  vTaskDelay(100/portTICK_PERIOD_MS);
  }
  
}


void setup() {
Serial.begin(11520);
vTaskDelay(1000/portTICK_PERIOD_MS);
Serial.println();

Serial.println("freeRTOS memory demo");


xTaskCreatePinnedToCore(
  memoryTask,
  "Task1",
  1024,
  NULL,
  1,
  NULL,
  ESP_RUNING_CORE
);


}

void loop() {

}
