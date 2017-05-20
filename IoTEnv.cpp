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
GSM_MQTT MQTT(20);

/*GPS*/
GPSGSM gps;
boolean gpsInit = false;

IOTENV::IOTENV(char * dispositivo, char * brokerHost, char * brokerPorta, char * topico, long intervaloColeta){
  iot.device = dispositivo;
  iot.MQTT_HOST = brokerHost;
  iot.MQTT_PORT = brokerPorta;
  iot.MQTT_TOPICO = topico;
  iot.intervalo = intervaloColeta; 
}

void IOTENV::start(){
  Serial.println("Startando a aplicação foi chamada");
	MQTT.begin();
  message.concat('{');
	if (gps.attachGPS()){
    	Serial.println("status=GPSREADY");
	}else{
		Serial.println("status=ERROR");
		start(); //restart gps
	}
}

void IOTENV::process(){
  Serial.println("Processo foi chamada");
	if(MQTT.available()){
    Serial.println("O MQTT está disponível");
		addIndice("dispositivo", String(iot.dispositivo()));
		//getLocalizacao();
		iot.coletarIndices();
	  sendMessage();
	}else{
    Serial.println("NOT MQTT");
	}
 
	MQTT.processing();
  delay(iot.intervaloColeta());
}

char* IOTENV::brokerHost(){return MQTT_HOST;}
char* IOTENV::brokerPort(){return MQTT_PORT;}
char* IOTENV::topico(){return MQTT_TOPICO;};
char* IOTENV::dispositivo(){return device;}
long IOTENV::intervaloColeta(){return intervalo;}

void IOTENV::getLocalizacao(){
	Serial.println("getLocalizacao foi chamada");
	int stat = 0;
  if(!gpsInit){
    if (gps.attachGPS()){
      Serial.println("status=GPSREADY");
      gpsInit = true;
      delay(20000); //Time for fixing
      getLocalizacao();
    }else{
      Serial.println("status=ERROR");
      gpsInit = false;
      getLocalizacao();
    }   
  }else{
    stat=gps.getStat();
    if(stat==1){
      Serial.println("NOT FIXED");
      Serial.println("Trying again ...");
      getLocalizacao();
    }
    else if(stat==0){
      Serial.println("GPS OFF");
      gpsInit = false;
      getLocalizacao(); 
    }
    else if(stat==2 || stat==3){
      Serial.print(stat);
      Serial.println("D FIXED");
      gps.getPar(lon,lat,alt,time,vel);
      Serial.println(lon);
      Serial.println(lat);
      Serial.println(alt);
      Serial.println(time);
      Serial.println(vel);
  
      message.concat("localizacao:[{lat:");
      message.concat(lat);
      message.concat(",lon:");
      message.concat(lon);
      message.concat("}],");
    }
  }
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
  message.concat('}');
  char m[message.length()];
  message.toCharArray(m, message.length());
	MQTT.publish(0, 0, 0, "iotmessage", iot.topico(), m);
	message = "";
}
