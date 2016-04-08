
//#define ADDRESS     "192.168.5.12:1883"
#define ADDRESS     "localhost:1883"
//#define ClientID    "abcd"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "pthread.h"
#include "messagesManager.h"
#include "mqttProtocol.h"
#include "linux_json.h"


// Thread Messager
pthread_t th_messager;

char ClientID[50]="BUGGY_";

int  mqttMsgArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void sendMQTTreply(struct JsonCommand srcMessage, char * msgType);
int pushMsgStack(void);
int pullMsgStack(unsigned char ptrStack);
char clearMsgStack(unsigned char ptrStack);

// Initialisation des variables
unsigned char mqttDataReady=0;

char MqttDataBuffer[500];

// Initialisation principale du system de messagerie

void *MessagerTask (void * arg){	 													// duty cycle is 50% for ePWM0A , 25% for ePWM0B;
	int mqttStatus;
	int lastMessage;
	int i;


	// Initialisation de la pile de reception de message
	for(i=0;i<10;i++)
		clearMsgStack(i);

	// Creation d'un id unique avec l'adresse mac
	sprintf(&ClientID[6], "%s", getMACaddr());

	// Connexion au broker MQTT
	mqttStatus=mqtt_init(ADDRESS, ClientID, mqttMsgArrived);

	if(!mqttStatus){
		printf("Connect� au broker MQTT IP: %s avec ID: %s\n",ADDRESS,ClientID   );
		if(!mqttAddRXChannel("MONTEST"))
			printf("Inscription au topic: OK\n");
		else printf("Inscription au topic: ERREUR\n");

	}else {
		printf("Connexion au broker MQTT impossible !\n");
	}
// BOUCLE PRINCIPALE
		while(1)
	    {
	    	// Verification de l'arriv�e d'un message MQTT
	    	if(mqttDataReady){
	    	printf("Nouveau message MQTT recu: \n");
	    	//printf("[DEBUG] message: %s", MqttDataBuffer);
	    	// RECEPTION DES DONNES UTILES
			if(GetAlgoidMsg(AlgoidMessageRX, MqttDataBuffer)>0){
				// Enregistrement du message dans la pile
				lastMessage=pushMsgStack();
				if(lastMessage>=0){
					sendMQTTreply(AlgoidMsgRXStack[lastMessage], "ACK");
					printf("\n- Message recu mis en file d'attente -\n");
				}
				else{
					printf("\n! File d'attente pleine !\n");
				}
			}else{
				sendMQTTreply(AlgoidMessageRX, "ERROR");
				printf("MESSAGE ALGOID INCORRECT RECU\n");
			}
			mqttDataReady=0;
	    	}

	    	usleep(10000);
	    }
 // FIN BOUCLE PRINCIPAL

  usleep(5000);
  pthread_exit (0);
}



int pushMsgStack(void){
	int ptrMsgRXstack=0;
	int i;

	// Recherche un emplacement vide dans la pile
	for(ptrMsgRXstack=0;(ptrMsgRXstack<10) && (AlgoidMsgRXStack[ptrMsgRXstack].msgID>=0);ptrMsgRXstack++);

	// CONTROLE DE L'ETAT DE LA PILE DE MESSAGE
	if(ptrMsgRXstack>=10){
		return -1;
	}else{

		// ENREGISTREMENT DU MESSAGE DANS LA PILE
		AlgoidMsgRXStack[ptrMsgRXstack]=AlgoidMessageRX;
/*
		// AFFICHAGE DES DONNEES
		printf("Message to   %s   from   %s with ID %d \n",AlgoidMsgRXStack[ptrMsgRXstack].msgTo,AlgoidMsgRXStack[ptrMsgRXstack].msgFrom, AlgoidMsgRXStack[ptrMsgRXstack].msgID );
		printf("type:  %d\n",AlgoidMsgRXStack[ptrMsgRXstack].msgType);
		printf("param: %d\n",AlgoidMsgRXStack[ptrMsgRXstack].msgParam);
		printf("ValCount: %d\n",AlgoidMsgRXStack[ptrMsgRXstack].msgValueCnt);
		for(i=0;i<AlgoidMsgRXStack[ptrMsgRXstack].msgValueCnt;i++){
			if(AlgoidMsgRXStack[ptrMsgRXstack].msgParam==LL_WD)
				printf("wheel: %s   velocity: %d   time: %d\n", AlgoidMsgRXStack[ptrMsgRXstack].msgValArray[i].wheel, AlgoidMsgRXStack[ptrMsgRXstack].msgValArray[i].velocity, AlgoidMsgRXStack[ptrMsgRXstack].msgValArray[i].time);
			else
				printf("mode: %s   value: %d\n", AlgoidMsgRXStack[ptrMsgRXstack].msgValArray[i].mode, AlgoidMsgRXStack[ptrMsgRXstack].msgValArray[i].value);
		}
*/
		ptrMsgRXstack++;
		return ptrMsgRXstack-1;
	}
}

int pullMsgStack(unsigned char ptrStack){
		int i;

		if(AlgoidMsgRXStack[ptrStack].msgType!=-1){
			AlgoidCommand=AlgoidMsgRXStack[ptrStack];

			// D�place les element de la pile
			for(i=ptrStack;i<9;i++){
				AlgoidMsgRXStack[ptrStack]=AlgoidMsgRXStack[ptrStack+1];
				ptrStack++;
			}

			// EFFACE LES DONNEES DE LA PILE
			strcpy(AlgoidMsgRXStack[9].msgFrom, "");
			strcpy(AlgoidMsgRXStack[9].msgTo, "");
			AlgoidMsgRXStack[9].msgID=-1;

			AlgoidMsgRXStack[9].msgParam=-1;
			AlgoidMsgRXStack[9].msgType=-1;

			for(i=0;i<AlgoidMsgRXStack[9].msgValueCnt;i++){
				strcpy(AlgoidMsgRXStack[9].msgValArray[i].mode, "");
				AlgoidMsgRXStack[9].msgValArray[i].value=-1;
			}

			return 1;
		}else return 0;
}
// ----------------------

char clearMsgStack(unsigned char ptrStack){
		int i;

		if(ptrStack<10){
			// EFFACE LES DONNEES DE LA PILE
			strcpy(AlgoidMsgRXStack[ptrStack].msgFrom, "");
			strcpy(AlgoidMsgRXStack[ptrStack].msgTo, "");
			AlgoidMsgRXStack[ptrStack].msgID=-1;

			AlgoidMsgRXStack[ptrStack].msgParam=-1;
			AlgoidMsgRXStack[ptrStack].msgType=-1;

			for(i=0;i<AlgoidMsgRXStack[ptrStack].msgValueCnt;i++){
				strcpy(AlgoidMsgRXStack[ptrStack].msgValArray[i].mode, "");
				AlgoidMsgRXStack[ptrStack].msgValArray[i].value=-1;

				strcpy(AlgoidMsgRXStack[ptrStack].msgValArray[i].wheel, "");
				AlgoidMsgRXStack[ptrStack].msgValArray[i].time=-1;
				AlgoidMsgRXStack[ptrStack].msgValArray[i].velocity=-1;
			}
			return 0;
		}
		return 1;
}

// ------------------------------------------------------------------------------------
// INITMESSAGER: Initialisation du gestionnaire de message
// - Demarrage thread messager
// -
// ------------------------------------------------------------------------------------
int InitMessager(void){
	// CREATION DU THREAD DE MESSAGERIE (Tache "MessagerTask")
	  if (pthread_create (&th_messager, NULL, MessagerTask, NULL)< 0) {
		  //Connexion au brocker MQTT

		return (1);
	  }else return (0);

}

// ------------------------------------------------------------------------------------
// CLOSEMESSAGE: Fermeture du gestionnaire de messages
// - Fermeture MQTT
// - Stop le thread manager
// ------------------------------------------------------------------------------------
int CloseMessager(void){
	int result;
	// TERMINE LE THREAD DE MESSAGERIE
	pthread_cancel(th_messager);
	// Attends la terminaison du thread de messagerie
	result=pthread_join(th_messager, NULL);
	return (result);
}


// -------------------------------------------------------------------
// Fonction Call-Back reception d'un message MQTT
// -------------------------------------------------------------------
int mqttMsgArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    unsigned int i;
    char* payloadptr;
    int messageCharCount=0;

    payloadptr = message->payload;
    messageCharCount=message->payloadlen;

    for(i=0; i<messageCharCount; i++)
    {
    	MqttDataBuffer[i]=payloadptr[i];
    }

    // Termine la chaine de caract�re
    MqttDataBuffer[messageCharCount]=0;

	mqttDataReady=1;

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


void sendMQTTreply(ALGOID srcMessage, char * msgType){
	char MQTTbuf[1024];

	SetAlgoidAck(MQTTbuf, srcMessage, ClientID, msgType);
	mqttPutMessage("MONRET", MQTTbuf, strlen(MQTTbuf));
}


