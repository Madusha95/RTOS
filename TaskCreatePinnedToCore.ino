#if CONFIG_FREERTOS_UNICORE
#define ESP_RUNING_CORE 0
#else
#define ESP_RUNING_CORE 1
#endif


#define LED_BUILTIN 2


void TaskBlink(void *pvPrameters){
(void)pvPrameters; 
for(;;){
digitalWrite(LED_BUILTIN,HIGH);
vTaskDelay(1000/portTICK_PERIOD_MS);
digitalWrite(LED_BUILTIN,LOW);
vTaskDelay(1000/portTICK_PERIOD_MS);
}
}
void setup() {

pinMode(LED_BUILTIN,OUTPUT);

xTaskCreatePinnedToCore(
TaskBlink,   //call function
"TaskBlink", //pC name 
1024,        // stack size 
NULL,        //parameters pass to function
1,           //priority
NULL,        //task handle pointer 
ESP_RUNING_CORE); //running cpu core 
}

void loop() {
  

}
