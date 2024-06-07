# Global-Solution-Edge-Computing
Global Solution - 1ESPG
Edge Computing

Integrantes
Renan Dias Utida - RM 558540
Gustavo Melanda da Nova - RM 556081

Link Youtube
https://www.youtube.com/watch?v=hgSzF-FiJIg&t=7s

# ReadMe


# Projeto de Monitoramento de Recifes de Coral


**Descrição do Projeto**
Este projeto visa monitorar e mitigar as ameaças aos recifes de coral utilizando a plataforma Arduino. O sistema monitora as condições ambientais ao redor dos recifes, como temperatura e o pH, e alerta para situações de perigo, como temperaturas extremas ou níveis de pH prejudiciais. Os dados coletados são registrados para análise posterior, contribuindo para a preservação desses ecossistemas vitais.

**Motivação**
Os recifes de corais são ecossistemas marinhos altamente diversificados, essenciais para a biodiversidade marinha. No entanto, enfrentam sérias ameaças devido às atividades humanas, mudanças climáticas e poluição. Este projeto busca oferecer uma solução tecnológica para monitorar e preservar esses importantes ecossistemas.

**Componentes Utilizados**
Arduino Uno
Sensor de Temperatura e Umidade DHT22
Sensor de pH
Módulo RTC DS1307
Display LCD I2C
LEDs (vermelho, amarelo, verde)
Buzzer
Resistores e fios de conexão

**Projeto Wokwi - Link**
https://wokwi.com/projects/400048684327179265


**Instruções de Uso**

**Montagem do Circuito:**
Conecte os sensores e atuadores ao Arduino conforme o esquema de ligação fornecido.

**Upload do Código:**
Abra o Arduino IDE e carregue o código sketch.ino no Arduino.

**Funcionamento:**
O sistema monitorará continuamente a temperatura, pH e umidade.
Alertas visuais e sonoros serão ativados em situações de risco.
Os dados serão registrados na EEPROM para análise posterior.


**Requisitos**
Arduino IDE instalado
Biblioteca DHT para o sensor de temperatura e umidade
Biblioteca RTClib para o módulo RTC
Biblioteca LiquidCrystal_I2C para o display LCD I2C
Biblioteca EEPROM para registro de dados


**Codigo Fonte**

#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h"

#define LOG_OPTION 1
#define SERIAL_OPTION 1
#define UTC_OFFSET -3

#define DHTPIN 2
#define DHTTYPE DHT22

#define red_Pin 6 
#define yellow_Pin 5
#define green_Pin 4
#define buzzer_Pin 9

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

const int maxRecords = 100;
const int recordSize = sizeof(uint32_t) + 2 * sizeof(float);
int startAddress = 0;
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

int lastLoggedMinute = -1;
int displayMode = 0;
unsigned long lastSwitchTime = 0;
unsigned long lastTimeUpdated = 0;

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

void loop() {
  delay(1000);
  float temp = dht.readTemperature();
  int sensorValue = analogRead(A0);
  float pH = map(sensorValue, 0, 1023, 0, 14);

  bool tempRed = temp < 16 || temp > 35;
  bool tempYellow = (temp >= 16 && temp < 22) || (temp > 30 && temp <= 35);
  bool tempGreen = temp >= 22 && temp <= 30;
  
  bool pHRed = pH < 7 || pH > 10;
  bool pHYellow = pH == 7 || pH == 10;
  bool pHGreen = pH == 8 || pH == 9;

  DateTime now = RTC.now();

  // Calculando o deslocamento do fuso horário
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adicionando o deslocamento ao tempo atual

  // Adicionando 3 horas para ajustar para UTC-3
  now = now + 3 * 3600;
  
  // Convertendo o novo tempo para DateTime
  DateTime adjustedTime = DateTime(now);

  // Prioridade para vermelho
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
  // Prioridade para amarelo
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
  // Verde
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

    // Logging to EEPROM every minute
    if (now.minute() != lastLoggedMinute) {
      lastLoggedMinute = now.minute();
      logData(temp, pH);
    }
  }
}

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


**Dependências**
DHT Sensor Library: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "DHT sensor library".
RTClib: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "RTClib".
LiquidCrystal I2C: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "LiquidCrystal I2C".


**Contribuições**
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e pull requests para melhorias.


**Licença**
Este projeto é licenciado sob a licença MIT - veja o arquivo LICENSE para detalhes.
