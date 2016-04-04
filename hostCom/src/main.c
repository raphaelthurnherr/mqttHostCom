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
    	printf("\n->");
    	myChar=getchar();

    	if(myChar=='q'){
    		endState=CloseMessager();
    		if(!endState)
    			  printf( "# ARRET tache Messager - status: %d\n", endState);
    		else printf( "# ARRET tache Messager erreur - status: %d\n", endState);

    		break;
    	}

    	if(myChar=='e'){
    		pullMsgStack(0);
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
    			for(i=0;i<AlgoidMsgRXStack[stackPtr].msgValueCnt;i++){
    				 printf("Name: %s   Data: %d\n", AlgoidMsgRXStack[stackPtr].msgValArray[i].mode, AlgoidMsgRXStack[stackPtr].msgValArray[i].value);
    			}
    			printf("\n");
    		}
    	}
	}
	return EXIT_SUCCESS;
}


// ***************************************************************************
// ---------------------------------------------------------------------------
// INIT MULTITASKING
// ---------------------------------------------------------------------------
// ***************************************************************************

