#include "WString.h"
#ifndef IOTENV_H
#define IOTENV_H
#endif

class IOTENV{
private:
  
	/*GPS*/
	char lon[15];
	char lat[15];
	char alt[15];
	char time[20];
	char vel[15];
	
	/*MQTT Config*/
	char* MQTT_HOST;
  char* MQTT_PORT;
	char* MQTT_TOPICO;

	/*DISPOSITIVO CONFIG*/
	char* device;
	long intervalo;

	/*DADOS DO GPS*/
	char * latitude;
	char * longitude;

public:
  IOTENV(char *dispositivo, char *brokerHost, char *brokerPorta, char *topico, long intervaloColeta);
	void start();
	void process();
 
  String message;
	char* dispositivo();
	long intervaloColeta();
	char* brokerHost();
	char* brokerPort();
	char* topico();

  void sendMessage();
	void getLocalizacao();
	void coletarIndices(); /*Função onde os dados serão setados*/
	void addIndice(char * nome, String valor); /*Função chamada dentro de coletarIndices para adicionar um indice coletado*/

};
