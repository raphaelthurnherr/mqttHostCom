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
#include <string.h>

#include "messagesManager.h"
#include "linux_json.h"
#include "udpPublish.h"
#include "main.h"

int processMsgCommand(void);
int processMsgRequest(void);
int make2WDaction(void);
int getWDvalue(char * wheelName);

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
//			printf("[main] messageID:  %d  param: %d   cmd: %d\n\n",AlgoidCommand.msgID,AlgoidCommand.msgParam,AlgoidCommand.msgType );
			switch(AlgoidCommand.msgType){
				case COMMAND : processMsgCommand(); break;
				case REQUEST : processMsgRequest(); break;
				default : break;
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
// PROCESSCOMMAND
// -------------------------------------------------------------------
int processMsgCommand(void){
	switch(AlgoidCommand.msgParam){
		case LL_WD : make2WDaction(); break;
		default : printf("\n blabla\n"); break;
	}
	return 0;
}

// -------------------------------------------------------------------
// PROCESSMSGRQUEST
// -------------------------------------------------------------------
int processMsgRequest(void){

	return 0;
}

// -------------------------------------------------------------------
// MAKE2WDACTION
// -------------------------------------------------------------------
int make2WDaction(void){
	int ptrData;
	char test[10]="";

	//wheel.left.velocity=0;
	//wheel.right.velocity=0;

	ptrData=getWDvalue("left");
	if(ptrData >=0)
		wheel.left.velocity=AlgoidCommand.msgValArray[ptrData].velocity;

	ptrData=getWDvalue("right");
	if(ptrData >=0)
		wheel.right.velocity=AlgoidCommand.msgValArray[ptrData].velocity;

	move2WDbuggy(wheel.left.velocity, wheel.right.velocity);
	return 0;
}


// -------------------------------------------------------------------
// GETWDVALUE
// -------------------------------------------------------------------
int getWDvalue(char* wheelName){
	int i;
	int searchPtr = -1;
	char searchText[50];
	char * mySearch;

	// Recherche dans les donnée recues la valeur correspondante au paramètre "wheelName"

	for(i=0;i<AlgoidCommand.msgValueCnt;i++){
		memset(searchText, 0, 50);
		mySearch=AlgoidCommand.msgValArray[i].wheel;
		strncpy(searchText,mySearch, strlen(AlgoidCommand.msgValArray[i].wheel));

		if(!strcmp(searchText, wheelName))
			searchPtr=i;
	}
	return searchPtr;
}

// -------------------------------------------------------------------
// MOVE2WDBUGGY
// -------------------------------------------------------------------

int move2WDbuggy(int leftWheelSpeed, int rightWheelSpeed){
	printf("\n ROBOT AVANCE DE %d GAUCHE, %d DROITE\n", leftWheelSpeed,rightWheelSpeed);
	return 0;
}
