// Codigo do LED (atuador) de uso geral em comunicacao MQTT
// Responsavel: Luis Fernando Gomez Gonzalez (luis.gonzalez@inmetrics.com.br)
// Projeto Konker

// Usando ainda o AP do Android configurado em meu telefone.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
  #include "user_interface.h"
  uint16 readvdd33(void);
}

#define sec             1000

//Variaveis do MQTT

const char* ssid = "Nexus 5"; 
const char* password = "batatinha"; 
const char* mqtt_server = "52.10.210.91";
char sensor1Topic[]    = "topic_test/C/request"; // Teste sem utilizacao do CEP.
char message_buff[20];

String pubString;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  // Aguardando 0.01 segundo
  delay(10);

  // Descrevendo o AP via Serial 
  Serial.println();
  Serial.print("Conectando a rede ");
  Serial.println(ssid);
  // Conectando 
  WiFi.begin(ssid, password);
  // Aguardando a conexao
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count ++;
    Serial.print(".");
  }
  //Conexao feita com sucesso
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereco de IP: ");
  Serial.println(WiFi.localIP());
}

// Funcao de Callback para o MQTT e para mudar o estado do LED
void callback(char* topic, byte* payload, unsigned int length) {
  int i;
  int state =0;
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
    Serial.print((char)payload[i]);
  }
  message_buff[i] = '\0';
  String msgString = String(message_buff);
  Serial.println();
  Serial.print(message_buff);
  
  // Checa o estado atual do LED.
  state = digitalRead(2);
  
  // Checa se o valor recebido foi "1". Caso positivo, muda o estado do LED (variavel state).
  if (msgString == "1"){   
    digitalWrite(2, !state); 
    Serial.println("Switching LED"); 
  }
  
}

// Setup do Microcontrolador: Vamos configurar o acesso serial, conectar no Wifi, configurar o MQTT e o GPIO do LED
void setup(){
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe(sensor1Topic);
  pinMode(2, OUTPUT);

}

// Funcao para conectar ao broker MQTT e reconectar quando perder a conexao
void reconnect() {
  // Loop ate estar conectado
  while (!client.connected()) {
    Serial.print("Tentando conectar no Broker MQTT...");
    // Tentando conectar no broker MQTT (PS.: Nao usar dois clientes com o mesmo nome. Causa desconexao no Mosquito)
    if (client.connect("ESP8266ClientA")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando conectar novamente em 3 segundos");
      // Esperando 3 segundos antes de re-conectar
      delay(3000);
    }
  }
}
 
 // Loop com o programa principal
void loop(){
  // Aguardando 0.3 segundos entre loops 
 delay(300);
    // Se desconecato, reconectar.
    if (!client.connected()) {
    reconnect();
  }
  client.loop();  
  client.subscribe(sensor1Topic);
}
