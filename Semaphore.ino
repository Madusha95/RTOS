#include <string.h>
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t wCPU = 0;
#else
  static const BaseType_t wCPU = 1;
#endif

//num of task
static const int numTask = 5;

//Structure for parameter passing 
typedef struct message{
  char body[20];
  uint8_t length;
}message;

static SemaphoreHandle_t semaparameters;

void myTask(void* parameters){
  message msg = *(message *)parameters;

  Serial.print("Received: ");
  Serial.print(msg.body);
  Serial.print("Length");
  Serial.print(msg.length);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  vTaskDelete(NULL);

}

void setup(){
  Serial.begin(9600);

  char taskName[12];
  message msg;
  char text[20] = "Hello World";
  vTaskDelay(1000/portTICK_PERIOD_MS);
  
  Serial.println(" Demo Semaphore");

  semaparameters = xSemaphoreCreateCounting(numTask,0);
  strcpy(msg.body, text);
  msg.length= strlen(text);

for(int i=0; i<numTask; i++){
  sprintf(taskName,"Task %", i);

  xTaskCreatePinnedToCore(myTask,
                    taskName,
                    1024,
                    (void *)&msg,
                    1,
                    NULL,
                    wCPU
  );
}

 for(int i=0; i<numTask; i++){
  xSemaphoreTake(semaparameters, portMAX_DELAY);
 } 
 Serial.println("All tasks created");
 
}

void loop(){
  
}
