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


**Dependências**
DHT Sensor Library: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "DHT sensor library".
RTClib: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "RTClib".
LiquidCrystal I2C: Para instalação, vá até o Arduino IDE e selecione Sketch > Include Library > Manage Libraries e procure por "LiquidCrystal I2C".


**Contribuições**
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e pull requests para melhorias.


**Licença**
Este projeto é licenciado sob a licença MIT - veja o arquivo LICENSE para detalhes.
