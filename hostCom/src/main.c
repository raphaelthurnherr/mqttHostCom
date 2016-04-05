/*
 ============================================================================
 Name        : hostCom.c
 Author      : raph
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>


#include "messagesManager.h"
#include "linux_json.h"
#include "udpPublish.h"
#include "main.h"

int move2WDbuggy(int leftWheelSpeed, int rightWheelSpeed);

int main(void) {
	if(InitMessager())
		printf ("# Creation tache messagerie : ERREUR\n");
	else {
		printf ("# Demarrage tache Messager: OK\n");
	}

	initUDP();

	while(1){
    	char myChar;
    	int endState;
    	//printf("\n->");
    	//myChar=getchar();

// COMMANDE ALGOID RECUE
		if(pullMsgStack(0)){
			printf("[main] messageID:  %d  param: %d   cmd: %d\n\n",AlgoidCommand.msgID,AlgoidCommand.msgParam,AlgoidCommand.msgType );
			//move2WDbuggy(AlgoidCommand.msgValArray[0].velocity, AlgoidCommand.msgValArray[1].velocity);
			if(AlgoidCommand.msgType==COMMAND){
				switch(AlgoidCommand.msgParam){
					case LL_WD : move2WDbuggy(AlgoidCommand.msgValArray[0].velocity, AlgoidCommand.msgValArray[1].velocity); break;
					default : printf("\n blabla\n"); break;
				}
			}
		}




    	if(myChar=='q'){
    		endState=CloseMessager();
    		if(!endState)
    			  printf( "# ARRET tache Messager - status: %d\n", endState);
    		else printf( "# ARRET tache Messager erreur - status: %d\n", endState);

    		break;
    	}

    	if(myChar=='h'){
    		char udpMessage[50];
    		sprintf(&udpMessage[0], "[ %s ] I'm here",ClientID);
    		sendUDPHeartBit(udpMessage);
		    printf("\n MYMAC %s", getMACaddr());
    	}


    	if(myChar=='d'){
    		char stackPtr;
    		char i;
    		for(stackPtr=0;stackPtr<10;stackPtr++){
    			printf("----- PILE DATA %d -----\n", stackPtr);
    			// AFFICHAGE DES DONNEES
    			printf("Message to   %s   from   %s with ID %d \n",AlgoidMsgRXStack[stackPtr].msgTo,AlgoidMsgRXStack[stackPtr].msgFrom, AlgoidMsgRXStack[stackPtr].msgID );
    			printf("type:  %d\n",AlgoidMsgRXStack[stackPtr].msgType);
    			printf("param: %d\n",AlgoidMsgRXStack[stackPtr].msgParam);

    			if(AlgoidMsgRXStack[stackPtr].msgParam==LL_WD){
    				for(i=0;i<AlgoidMsgRXStack[stackPtr].msgValueCnt;i++){
    					printf("wheel: %s   velocity: %d   time: %d\n", AlgoidMsgRXStack[stackPtr].msgValArray[i].wheel, AlgoidMsgRXStack[stackPtr].msgValArray[i].velocity, AlgoidMsgRXStack[stackPtr].msgValArray[i].time);
    				}
    			}
    			else{
    				for(i=0;i<AlgoidMsgRXStack[stackPtr].msgValueCnt;i++){
    				    printf("mode: %s   value: %d\n", AlgoidMsgRXStack[stackPtr].msgValArray[i].mode, AlgoidMsgRXStack[stackPtr].msgValArray[i].value);
    			}
    			}

    			printf("\n");
    		}
    	}

    	usleep(100000);
	}
	return EXIT_SUCCESS;
}


// -------------------------------------------------------------------
// MOVE2WDBUGGY
// -------------------------------------------------------------------

int move2WDbuggy(int leftWheelSpeed, int rightWheelSpeed){
	printf("\n ROBOT AVANCE DE %d GAUCHE, %d DROITE\n", leftWheelSpeed,rightWheelSpeed);
	return 0;
}
