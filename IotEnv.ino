#include "IoTEnv.h"

char* dispositivo = "tcc-bruno"; /*Identificador do Dispositivo*/
char* brokerHost = "broker.mqttdashboard.com"; /*MQTT - Host do Broker*/
char* brokerPorta = "1883"; /*MQTT - Porta do Broker*/
char* topico = "iotenv"; /*MQTT - Topico*/
long intervaloColeta = 5000; /*Tempo em milisegundos*/

IOTENV iot(dispositivo, brokerHost, brokerPorta, topico, intervaloColeta);

void IOTENV::coletarIndices(){
  /**
  TODOS OS DADOS DEVERÃO SER COLETADOS E ADICIONADOS AO FRAMEWORK NESTA FUNÇÃO
  Ex: 
  temperatura = DHT.temperatura();
  IOT.addIndice("temperatura", String(temperatura));
  umidade = DHT.umidade();
  IOT.addIndice("umidade", String(umidade)); 
  */
  Serial.println("Coletar Indices foi chamada");
  iot.addIndice("umidade", String(75));
}



void setup(){
  iot.start();
}

void loop(){
  iot.process(); /*É nesta função em que os dados serão coletados e enviados no intervalo determinado*/
}