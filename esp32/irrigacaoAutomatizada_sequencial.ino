#include <Arduino.h>
#include <Wire.h>

// Configuração do LCD
// Define o endereço do LCD em 0x27 para um display de 16x2
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// inclui a biblioteca:
#include <LiquidCrystal.h>

// define os pinos de conexão entre o Arduino e o Display LCD
const int rs = 19, en = 23, d4 = 18, d5 = 17, d6 = 16, d7 = 15;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variáveis do sensor resistivo de umidade
const int umiAnalog = A0; // define a porta de leitura do sensor resistivo
int umiR; // declara a variável de umidade

// Válvula solenoíde
const int solenoid_pin = 26;

// ESP32 In-built led
//static const int led_pin = 2;  // Pino para o LED embutido na ESP32-WROOM-32

// Limite de umidade
const int limite_umidade_superior = 50; // Umidade para desligar válvula
const int limite_umidade_inferior = 35; // Umidade para ligar solenoíde


void setup() {
  // Inicia comunicação serial
  Serial.begin(115200);

  Serial.println("-- Inicializando sistema de irrigação automatizado --");

  // Configurações do LCD
  lcd.begin(16, 2); // Inicializa o LCD

  // Configuração do pino da válvula solenoide
  pinMode(solenoid_pin, OUTPUT);
  digitalWrite(solenoid_pin, LOW); // Válvula começa fechada
}

void loop() {
  // Tarefa 1 -  Leitura do sensor de umidade
  umiR = analogRead(umiAnalog); // lê a porta da umidade
  //Serial.print("Umidade pura: ");
  //Serial.println(umiR);
  // Mapeia o valor lido (0-1023) para umidade percentual (0-100)
  int umidade_medida = map(umiR, 4095, 2100, 0, 100);
  // Prints para debug no monitor serial
  Serial.println("Umidade medida: ");
  Serial.println(umidade_medida);

  // Tarefa 2 - COntrole da válvula solenoide
  if (umidade_medida < limite_umidade_inferior && digitalRead(solenoid_pin) == LOW) {
    Serial.println("Ligando solenoide");
    digitalWrite(solenoid_pin, HIGH);
    // Delay esperar água dispersar na terra
    delay(5000);
  } else if (umidade_medida >= limite_umidade_superior && digitalRead(solenoid_pin) == HIGH) {
    Serial.println("Desligando solenoide");
    digitalWrite(solenoid_pin, LOW);
  }

  // Atualiza o display LCD com o valor de umidade - Função callback do timer
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Umidade: ");
  lcd.print(umidade_medida);
  
  // Delay para próxima medição e ação de controle
  delay(200);
}