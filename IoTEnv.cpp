#include "WString.h"
#include "IoTEnv.h"
#include "GSM_MQTT.h"
#include "gps.h"



/*IOTENV*/
extern IOTENV iot;
extern char * dispositivo;
extern char * brokerHost;
extern char * brokerPorta;
extern char * topico;
extern char * intervalorColeta;

/*MQTT*/
void GSM_MQTT::AutoConnect(void){connect(dispositivo, 0, 0, "", "", 1, 0, 0, 0, "", "");}
void GSM_MQTT::OnConnect(void){}
void GSM_MQTT::OnMessage(char *Topic, int TopicLength, char *Message, int MessageLength){}

String MQTT_HOST = brokerHost;
String MQTT_PORT = brokerPorta;
String MQTT_TOPICO = topico;
boolean MQTT_SENT = true;
GSM_MQTT MQTT(20);

/*GPS*/
GPSGSM gps;
boolean gpsInit = true;
boolean gpsData = false;

IOTENV::IOTENV(char * dispositivo, char * brokerHost, char * brokerPorta, char * topico, long intervaloColeta){
  iot.device = dispositivo;
  iot.MQTT_HOST = brokerHost;
  iot.MQTT_PORT = brokerPorta;
  iot.MQTT_TOPICO = topico;
  iot.intervalo = intervaloColeta; 
}

void IOTENV::start(){
  Serial.println("Startando a aplicação foi chamada");
  MQTT.gsmOn();
}

void IOTENV::process(){
  if(MQTT_SENT){
    message.concat('{');
    Serial.println("Processo foi chamada");
    getLocalizacao();

    MQTT_SENT = false;
    MQTT.begin(); 
  }else{  
    if(MQTT.available()){
      Serial.println("MQTT Disponível");
      addIndice("dispositivo", String(iot.dispositivo()));
      iot.coletarIndices();    
      sendMessage();
    }else{
      Serial.println("NOT MQTT");
    }

    MQTT_SENT = MQTT.publishSent();
    Serial.print("O MQTTSENT DEU ");
    Serial.println(MQTT_SENT);
    MQTT.processing();
  }
  
  delay(iot.intervaloColeta());
}

char* IOTENV::brokerHost(){return MQTT_HOST;}
char* IOTENV::brokerPort(){return MQTT_PORT;}
char* IOTENV::topico(){return MQTT_TOPICO;};
char* IOTENV::dispositivo(){return device;}
long IOTENV::intervaloColeta(){return intervalo;}

void IOTENV::getLocalizacao(){
  Serial.println("getLocalizacao foi chamada");
  if (gps.attachGPS()){
    Serial.println("status=GPSREADY");
    gpsInit = true;
    delay(20000); //Time for fixing
  }else{
    Serial.println("status=ERROR");
    gpsInit = false;
    getLocalizacao();
  }   
  
  Serial.println("Getting GPS data");
  int stat = 0;
  while(stat!=2 && stat!=3){
    stat=gps.getStat();  
    switch(stat){
      case 1: Serial.println("NOT FIXED");
      break;
      case 2: Serial.println("2D FIXED");
      break;
      case 3: Serial.println("3D FIXED");
      break;
    }
  }
      
  gps.getPar(lon,lat,alt,time,vel);

  Serial.println(lon);
  Serial.println(lat);
  Serial.println(alt);
  Serial.println(time);
  Serial.println(vel);
  
  message.concat("loc:[{");
  message.concat('"');
  message.concat("lat");
  message.concat('"');
  message.concat(':');
  message.concat('"');
  message.concat(lat);
  message.concat('"');
  message.concat(',');
  message.concat('"');
  message.concat("lon");
  message.concat('"');
  message.concat(':');
  message.concat('"');
  message.concat(lon);
  message.concat('"');
  message.concat("}],");

  gps.deattachGPS();
  
} 

void IOTENV::addIndice(char * nome, String valor){
  Serial.println("Adicionar Indice foi chamada");
  message.concat('"');
  message.concat(nome);
  message.concat('"');
  message.concat(":");
  message.concat('"');
  message.concat(valor);
  message.concat('"');
  message.concat(",");
}

void IOTENV::sendMessage(){
    message.remove(message.length() - 1);/*Removendo ultima ','*/
    message.concat('}');
    char m[message.length()];
    message.toCharArray(m, message.length());
    MQTT.publish(0, 0, 0, "iotmessage", iot.topico(), m);
    message = "";  
}