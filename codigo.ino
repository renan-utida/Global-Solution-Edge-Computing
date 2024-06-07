#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h"

// Definições de constantes e pinos
#define LOG_OPTION 1
#define SERIAL_OPTION 1
#define UTC_OFFSET -3

#define DHTPIN 2
#define DHTTYPE DHT22

#define red_Pin 6 
#define yellow_Pin 5
#define green_Pin 4
#define buzzer_Pin 9

// Instanciando os objetos para os sensores e o display LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

// Definindo os limites de armazenamento de registros na EEPROM
const int maxRecords = 100;
const int recordSize = sizeof(uint32_t) + 2 * sizeof(float);
int startAddress = 0;
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

int lastLoggedMinute = -1;
int displayMode = 0;
unsigned long lastSwitchTime = 0;
unsigned long lastTimeUpdated = 0;

// Definindo o caractere especial para o símbolo de gota
byte gota[8] = { 
  B00000, 
  B00100, 
  B01110, 
  B11111, 
  B11111, 
  B11101, 
  B01110, 
  B00000 
};

// Função para exibir o logo animado "gotinha"
void gotinha_logo() {
  lcd.createChar(1, gota);
  lcd.setCursor(4, 1);
  lcd.write(byte(1));
  delay(500);
  lcd.createChar(2, gota);
  lcd.setCursor(7, 1);
  lcd.write(byte(2));
  delay(500);
  lcd.createChar(3, gota);
  lcd.setCursor(10, 1);
  lcd.write(byte(3));
  delay(2000);
}

// Função para exibir uma animação de espera com a "gotinha"
void gotinha_aguarde() {
  lcd.createChar(1, gota);
  lcd.setCursor(4, 1);
  lcd.write(byte(1));
  delay(250);
  lcd.createChar(2, gota);
  lcd.setCursor(7, 1);
  lcd.write(byte(2));
  delay(250);
  lcd.createChar(3, gota);
  lcd.setCursor(10, 1);
  lcd.write(byte(3));
  delay(2000);
  lcd.clear();
}

// Função de introdução com animação e som
void introducao() {
  int musica[] = {100, 200, 150, 250, 300};

  for (int i = 0; i < 7; i++) {
    tone(buzzer_Pin, musica[i]);
    delay(200);
    noTone(buzzer_Pin);
  }

  lcd.clear();
  char logo[] = "Clean Ocean";

  for (int i = 0; i < sizeof(logo) - 1; i++) {
    lcd.setCursor(i + 2, 0);
    lcd.print(logo[i]);
    delay(500);
  }

  gotinha_logo();
}

// Função de animação de espera
void aguarde() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Aguarde");
  gotinha_aguarde();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Aguarde");
  gotinha_aguarde();
}

// Função para sinalização amarela e som correspondente
void buzzer_yellow() {
  delay(1000);
  digitalWrite(red_Pin, LOW);
  digitalWrite(yellow_Pin, HIGH);
  digitalWrite(green_Pin, LOW);

  tone(buzzer_Pin, 1000, 700);
  digitalWrite(buzzer_Pin, HIGH);
  delay(1000);
  digitalWrite(buzzer_Pin, LOW);
  delay(4000);
}

// Função para sinalização vermelha e som correspondente
void buzzer_red() { 
  digitalWrite(yellow_Pin, LOW);
  digitalWrite(green_Pin, LOW);

  for (int i = 0; i < 3; i++) {
    tone(buzzer_Pin, 1200, 750);
    delay(400);
    tone(buzzer_Pin, 950, 700);
    delay(400);
    digitalWrite(red_Pin, HIGH);
    delay(150);
    digitalWrite(red_Pin, LOW);
    delay(150);
    digitalWrite(red_Pin, HIGH);
  }
  noTone(buzzer_Pin);
  delay(3000);
}

// Configurações iniciais
void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  dht.begin();
  RTC.begin();

  pinMode(red_Pin, OUTPUT);
  pinMode(yellow_Pin, OUTPUT);
  pinMode(green_Pin, OUTPUT);

  Serial.begin(9600);
  EEPROM.begin();

  introducao();
  aguarde();
}

// Função principal do loop
void loop() {
  delay(1000);
  float temp = dht.readTemperature();
  int sensorValue = analogRead(A0);
  float pH = map(sensorValue, 0, 1023, 0, 14);

  // Verificação de condições críticas de temperatura e pH
  bool tempRed = temp < 16 || temp > 35;
  bool tempYellow = (temp >= 16 && temp < 22) || (temp > 30 && temp <= 35);
  bool tempGreen = temp >= 22 && temp <= 30;
  
  bool pHRed = pH < 7 || pH > 10;
  bool pHYellow = pH == 7 || pH == 10;
  bool pHGreen = pH == 8 || pH == 9;

  DateTime now = RTC.now();

  // Ajuste do tempo para o fuso horário local
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adicionando o deslocamento ao tempo atual

  // Adicionando 3 horas para ajustar para UTC-3
  now = now + 3 * 3600;
  
  // Convertendo o novo tempo para DateTime
  DateTime adjustedTime = DateTime(now);

  // Prioridade para sinalização vermelha
  if (tempRed || pHRed) {
    buzzer_red();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Perigo!");

    if (tempRed && pHRed) {
      lcd.setCursor(0, 1);
      lcd.print("Temperatura e pH");
    } else if (tempRed) {
      lcd.setCursor(0, 1);
      lcd.print("Temperatura!");
    } else if (pHRed) {
      lcd.setCursor(0, 1);
      lcd.print("pH!");
    }

    if (SERIAL_OPTION) {
      Serial.print("Perigo! ");
      if (tempRed && pHRed) {
        Serial.print("Temperatura e pH em Perigo (Vermelho)");
      } else if (tempRed) {
        Serial.print("Temperatura em Perigo (Vermelho)");
      } else if (pHRed) {
        Serial.print("pH em Perigo (Vermelho)");
      }
      Serial.print("\tHora: ");
      Serial.println(adjustedTime.timestamp(DateTime::TIMESTAMP_FULL));
      Serial.print("\tTemperatura: ");
      Serial.print(temp);
      Serial.print(" C\t pH: ");
      Serial.println(pH);
    }
  }
  // Prioridade para sinalização amarela
  else if (tempYellow || pHYellow) {
    buzzer_yellow();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Atencao");

    if (tempYellow && pHYellow) {
      lcd.setCursor(0, 1);
      lcd.print("Temperatura e pH!");
    } else if (tempYellow) {
      lcd.setCursor(0, 1);
      lcd.print("Temperatura!");
    } else if (pHYellow) {
      lcd.setCursor(0, 1);
      lcd.print("pH!");
    }
    delay(2000);

    if (SERIAL_OPTION) {
      Serial.print("Atenção! ");
      if (tempYellow && pHYellow) {
        Serial.print("Temperatura e pH Alterados (Amarelo)");
      } else if (tempYellow) {
        Serial.print("Temperatura Alterada (Amarelo)");
      } else if (pHYellow) {
        Serial.print("pH Alterado (Amarelo)");
      }
      Serial.print("\tHora: ");
      Serial.println(adjustedTime.timestamp(DateTime::TIMESTAMP_FULL));
      Serial.print("\tTemperatura: ");
      Serial.print(temp);
      Serial.print(" C\t pH: ");
      Serial.println(pH);
    }
  }
  // Sinalização verde
  else if (tempGreen && pHGreen) {
    digitalWrite(red_Pin, LOW);
    digitalWrite(yellow_Pin, LOW);
    digitalWrite(green_Pin, HIGH);

    unsigned long currentMillis = millis();
    if (currentMillis - lastSwitchTime >= 10000) {
      lastSwitchTime = currentMillis;
      displayMode = (displayMode + 1) % 2;
    }

    updateDisplay(adjustedTime, temp, pH);

    // Registro de dados na EEPROM a cada minuto
    if (now.minute() != lastLoggedMinute) {
      lastLoggedMinute = now.minute();
      logData(temp, pH);
    }
  }
}

// Função para atualizar o display LCD
void updateDisplay(DateTime adjustedTime, float temp, float pH) {
  lcd.clear();
  if (displayMode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("DATA: ");
    lcd.print(adjustedTime.day() < 10 ? "0" : ""); // Adiciona zero à esquerda se dia for menor que 10
    lcd.print(adjustedTime.day());
    lcd.print("/");
    lcd.print(adjustedTime.month() < 10 ? "0" : ""); // Adiciona zero à esquerda se mês for menor que 10
    lcd.print(adjustedTime.month());
    lcd.print("/");
    lcd.print(adjustedTime.year());

    lcd.setCursor(0, 1);
    lcd.print("HORA: ");
    lcd.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
    lcd.print(adjustedTime.hour());
    lcd.print(":");
    lcd.print(adjustedTime.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se minuto for menor que 10
    lcd.print(adjustedTime.minute());
    lcd.print(":");
    lcd.print(adjustedTime.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se segundo for menor que 10
    lcd.print(adjustedTime.second());
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(pH);
  }
}

// Função para registrar dados na EEPROM
void logData(float temp, float pH) {
  if (currentAddress + recordSize > endAddress) {
    currentAddress = startAddress;
  }

  DateTime now = RTC.now();

  EEPROM.put(currentAddress, now.unixtime());
  currentAddress += sizeof(uint32_t);

  EEPROM.put(currentAddress, temp);
  currentAddress += sizeof(float);

  EEPROM.put(currentAddress, pH);
  currentAddress += sizeof(float);
}
