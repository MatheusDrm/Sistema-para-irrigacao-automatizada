#include <Arduino.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "freertos/semphr.h"
// #include "freertos/timers.h"

// Configuração para utilizar apenas um core -> Usar segundo core (app_cpu) da esp32
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Variáveis para configuração da fila
static const int queue_len = 5;     // Tamanho da fila
static QueueHandle_t queue_handle;  // Handle para a fila

// Variáveis para configuração do mutex
static SemaphoreHandle_t mutex_handle;

// Variáveis para configuração do timer
static const TickType_t timer_delay = 200 / portTICK_PERIOD_MS;
static TimerHandle_t software_timer_handle = NULL;

// Configuração do LCD
// Define o endereço do LCD em 0x27 para um display de 16x2
// LiquidCrystal_I2C lcd(0x27, 16, 2);

const int rs = 19, en = 23, d4 = 18, d5 = 17, d6 = 16, d7 = 15;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variáveis do sensor resistivo de umidade
const int umiAnalog = A0; // define a porta de leitura do sensor resistivo
const int umiAnalog = A0; // define a porta de leitura do sensor capacitivo

volatile int umiR; // declara a variável de umidade

// Válvula solenoíde
const int solenoid_pin = 26;

// ESP32 In-built led -> Testes
static const int led_pin = 2;  // Pino para o LED embutido na ESP32-WROOM-32

// Função para obtenção dos dados do sensor resistivo
void leitura_sensorResistivo(void *parameter) {
  while(1) {
    if (xSemaphoreTake(mutex_handle, portMAX_DELAY) == pdTRUE) {
      umiR = analogRead(umiAnalog); // lê a porta da umidade
      Serial.println("Enviando dados de umidade para a fila");
      if (xQueueSend(queue_handle, (void *)&umiR, 10) != pdTRUE) {
        Serial.println("ERRO: Envio de dados para fila incompleto");
      }
      xSemaphoreGive(mutex_handle);
    } else {
      Serial.println("FALHA: Obtenção do mutex incompleta no SENSOR");
    }
    // Sensor delay
    vTaskDelay(150 / portTICK_PERIOD_MS);
  }
}

// Função para controle da válvula solenóide
void valv_control(void *parameter) {
  while(1) {
    int rcv_data;
    // Limite de umidade
    const int limite_umidade_superior = 50; // Umidade para desligar válvula
    const int limite_umidade_inferior = 35; // Umidade para ligar solenoíde    // Caso tenha algo novo na fila considere para fazer o controle da válvula
    if (xQueueReceive(queue_handle, (void *)&rcv_data, 10) == pdTRUE) {
      int umidade_medida = map(umiR, 4095, 2100, 0, 100);
      Serial.print("Umidade recebida: ");
      Serial.println(umidade_medida);
      if (umidade_medida < limite_umidade_inferior && digitalRead(solenoid_pin) == LOW) {
        Serial.println("Ligando solenoide");
        digitalWrite(solenoid_pin, HIGH);
      } else if (umidade_medida >= limite_umidade_superior && digitalRead(solenoid_pin) == HIGH) {
        Serial.println("Desligando solenoide");
        digitalWrite(solenoid_pin, LOW);
      }
    }
    // Task delay
    vTaskDelay(150 / portTICK_PERIOD_MS);
  }
}

// Callback do timer
void softwareTimer_callback(TimerHandle_t xTimer) {
  if (xSemaphoreTake(mutex_handle, portMAX_DELAY) == pdTRUE) {
    // Adicione o valor de umidade no display
    int umidade = map(umiR, 4095, 2000, 0, 100);
    lcd.setCursor(0,1);
    lcd.clear();
    lcd.print("Umidade: ");
    lcd.print(umidade);
    xSemaphoreGive(mutex_handle);
  } else {
    Serial.println("FALHA: Obtenção do mutex incompleta no DISPLAY");
  }
}

void setup() {
  // Inicia comunicação serial
  Serial.begin(115200);

  // Delay
  vTaskDelay(500 / portTICK_PERIOD_MS);

  Serial.println("-- Inicializando sistema de irrigação automatizado --");

  // Configurações do LCD
  lcd.begin(16, 2); // Inicializa o LCD
  // lcd.setBacklight(HIGH); // Liga o backlight do LCD

  // Configuração do pino da válvula solenoide
  pinMode(solenoid_pin, OUTPUT);
  digitalWrite(solenoid_pin, LOW); // Válvula começa fechada

  // Criando fila
  queue_handle = xQueueCreate(queue_len, sizeof(int));
  if (queue_handle == NULL) {
    Serial.println("ERRO: Falha ao criar a fila");
    while (1);
  }

  // Criando mutex
  mutex_handle = xSemaphoreCreateMutex();
  if (mutex_handle == NULL) {
    Serial.println("ERRO: Falha ao criar o mutex");
    while (1);
  }

  // Criação do timer por software
  software_timer_handle = xTimerCreate("software_timer_handle", timer_delay, pdTRUE, (void *)0, softwareTimer_callback);  // Parâmetros: Name of timer, Period of timer (in ticks), Auto-reload, Timer ID, Callback function
  if (software_timer_handle == NULL) {
    Serial.println("ERRO: Falha ao criar o timer por software");
    while (1);
  }

  // Inicia timer
  xTimerStart(software_timer_handle, portMAX_DELAY); // Tarefa fica bloqueada indefinidamente até que o comando enviado com sucesso para a fila do timer

  // Criação de tarefas
  xTaskCreatePinnedToCore(
              leitura_sensorResistivo,     // Função a ser executada
              "leitura_sensorResistivo",   // Nome para se referir a tarefa
              1024,                        // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,                        // Parametro para a função
              1,                           // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,                        // Handle da tarefa
              app_cpu);                    // Roda em um core somente

  xTaskCreatePinnedToCore(
              valv_control,            // Função a ser executada
              "valv_control",          // Nome para se referir a tarefa
              1024,                    // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,                    // Parametro para a função
              1,                       // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,                    // Handle da tarefa
              app_cpu);                // Roda em um core somente

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // setup() e loop() rodam em sua própria tarefa com prioridade 1 no core 1 na ESP32
}
