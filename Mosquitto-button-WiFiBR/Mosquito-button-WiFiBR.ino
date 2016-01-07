// Codigo do botao de uso geral em comunicacao MQTT
// Responsavel: Luis Fernando Gomez Gonzalez (luis.gonzalez@inmetrics.com.br)
// Projeto Konker

// Agora usando um gerenciador de conexão.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManagerBR.h>
#include <PubSubClient.h>

extern "C" {
  #include "user_interface.h"
}

#define sec             1000

//Variaveis do MQTT
const char* mqtt_server = "lfgomez.com";//"162.243.211.113";
char sensor1Topic[]    = "topic_test/C/request";
char sensor1Payload[]    = "{\"event\":{\"payloadData\":{\"device\":\"ClientPaho2\",\"metric\":\"test.boolean\",\"value\":1.0,\"timestamp\":144966264001}}}";
int buttonPin = 2;
int buttonState = 0;
char message_buffer[20];
String pubString;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int value = 0;
int count = 0;


// Funcao de Callback para o MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}
// Setup do Microcontrolador: Vamos configurar o acesso serial, conectar no Wifi, configurar o MQTT e o GPIO do botao
void setup(){
  Serial.begin(115200);  
  // Wifi Mananger BR
  WiFiManager wifiManager;
  wifiManager.setTimeout(300);
  //wifiManager.resetSettings(); //Caso seja necessario resetar os valores para Debug
  if(!wifiManager.autoConnect("KonkerConfig")) {
    ESP.reset();
    delay(1000);
  } 
  // MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(buttonPin, INPUT);
  
}
// Funcao para conectar ao broker MQTT e reconectar quando perder a conexao
void reconnect() {
  // Loop ate estar conectado
  while (!client.connected()) {
    Serial.print("Tentando conectar no Broker MQTT...");
    // Tentando conectar no broker MQTT (PS.: Nao usar dois clientes com o mesmo nome. Causa desconexao no Mosquito)
    if (client.connect("ESP8266ClientB")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println("Tentando conectar novamente em 3 segundos");
      // Esperando 3 segundos antes de re-conectar
      delay(3000);
    }
  }
}
 // Loop com o programa principal
void loop(){
  // Aguardando 0.3 segundos entre loops (PS.: O clique no botao deve ocorrer por um tempo superior a 0.3 segundos) 
 delay(300);
    // Se desconecato, reconectar.
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Lendo o estado do botao. Como ele esta conectado em um divisor, o estado natural eh HIGH. Isso ocorre pois o GPIO2 nao pode ser inicializado em LOW.
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
      //pubString = "{\"event\":{\"payloadData\":{\"device\":\"ClientPaho2\",\"metric\":\"test.boolean\",\"value\":1.0,\"timestamp\":0}}}"; // Para disparar o CEP
      pubString = "1"; // Para testes com o LED.
      pubString.toCharArray(message_buffer, pubString.length()+1);
      Serial.print("Publishing new value: ");
      Serial.println(message_buffer);     
      client.publish(sensor1Topic, message_buffer);
  }

}

