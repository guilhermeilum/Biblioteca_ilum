/**
  Progeto biblioteca
  Nome: Sensor de umidade e temperatura para a biblioteca. 
  Objetivo:  Medir a temperatura e umidade da biblioteca para analizar os dados obtidos.

  @autor Guilherme Gurian Dariani
  @versão 1.0 24/06/22 

  Código utilizado para consulta.
    
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-thingspeak-publish-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
// Define todas as bibliotecas e variaveis usadas.
#include <DHT.h>
int LED = 2;
float leitura_umidade;
float leitura_temperatura;
float calibrado_umidade;
float calibrado_temperatura;
#define DHTTYPE DHT11  
uint8_t pino_dados = 4;
DHT sensor_dht(pino_dados, DHTTYPE);


#include <WiFi.h>
#include "ThingSpeak.h"

const char* ssid = "Ilum";   // your network SSID (name) 
const char* password = "SENHA";   // your network password

#define pino_led_vermelho2 5
#define pino_led_vermelho 18
#define pino_led_amarelo 19
#define pino_led_verde 21
#define pino_led_azul 22
WiFiClient  client;

unsigned long myChannelNumber = 1; // Numero do canal do thingspeak
const char * myWriteAPIKey = "APIKEY"; //Chave privada do site

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 300000; // Tempo de espera para enviar os dados

void setup() {
  delay(50);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);   // Modo de comunicação esp32-WIFI
  
  ThingSpeak.begin(client);  // Inicializa o Thingspeak
  sensor_dht.begin();
  
  // Todas as variaveis criadas são definidas
  pinMode(LED, OUTPUT);
  pinMode(pino_led_vermelho2, OUTPUT);
  pinMode(pino_led_vermelho, OUTPUT);
  pinMode(pino_led_amarelo, OUTPUT);
  pinMode(pino_led_verde, OUTPUT);
  pinMode(pino_led_azul, OUTPUT);
}
void apagaleds() // Função que apaga todos os leds
{
  digitalWrite(pino_led_vermelho2, LOW);
  digitalWrite(pino_led_vermelho, LOW);
  digitalWrite(pino_led_amarelo, LOW);
  digitalWrite(pino_led_verde, LOW);
  digitalWrite(pino_led_azul, LOW);
}

void loop() {
  leitura_umidade = sensor_dht.readHumidity(); // Lê o valor de umidade identificado
  leitura_temperatura = sensor_dht.readTemperature(); // Lê o valor de temperatura idenficado

  // Calibração do sensor para umidade e temperatura, dados retirados do arquivo python.
  calibrado_umidade = (3.6398067467864834 + (0.6332527238104956*leitura_umidade)); 
  calibrado_temperatura = (9.355976022706546 +(0.6627333544331856*leitura_temperatura));

  // Conectar no wifi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){ // loop que só quebra quando a esp32 conecta na internet 
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
      digitalWrite(LED, HIGH);
    }

    
  if ((millis() - lastTime) > timerDelay) { // If para enviar os dados em cada 5 minutos.
    Serial.print("Umidade (em porcentagem): ");
    Serial.print(calibrado_umidade);
    Serial.println();

    Serial.print("Temperatura (Celsius): ");
    Serial.print(calibrado_temperatura);
    Serial.println();
  
    // Se o WiFi desconectar irá tentar conectar novamente até conseguir.
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
      digitalWrite(LED, HIGH);
    }
    // Define que valores serão enviados para cada Field, que sera o local q o dado é gravado no site
    ThingSpeak.setField(1, calibrado_temperatura); 
    ThingSpeak.setField(2, calibrado_umidade);
    
    // Aqui escreveos os dados no site, do Field 1 e 2.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){ // Se o valor de x for igual a 200 tudo ocorreu certo, se não ira printar o código HTTP de erro. 
      Serial.println("Channel update successful.");
      digitalWrite(LED, HIGH);
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(LED, LOW);
    }
    lastTime = millis();
  }

  // Serie de if para conseguir acender os leds com a umidade definida.
  // Respectivamente da maior umidade para a menor.
  // Os dados serão conferidos de 1 em 1 segundo.
    if(calibrado_umidade > 70 && calibrado_umidade <99) // Liga os dois leds vermelhos
  {
  apagaleds();
  digitalWrite(pino_led_vermelho, HIGH);
  digitalWrite(pino_led_vermelho2, HIGH);
  }
  
  if(calibrado_umidade > 60 && calibrado_umidade <69) // Liga somente um vermelho
  {
  apagaleds();
  digitalWrite(pino_led_vermelho, HIGH);
  }
  
  if(calibrado_umidade > 55 && calibrado_umidade <59) // Liga o amarelo
  {
  apagaleds();
  digitalWrite(pino_led_amarelo, HIGH);
  }

  if(calibrado_umidade > 45 && calibrado_umidade <54) // Liga o verde
  {
  apagaleds();
  digitalWrite(pino_led_verde, HIGH);
  }
  
  if(calibrado_umidade > 1 && calibrado_umidade <44) // Liga o azul
  {
  apagaleds();
  digitalWrite(pino_led_azul, HIGH);
  }
  Serial.println(calibrado_umidade);
  Serial.println(calibrado_temperatura);
  delay(1000);
}
