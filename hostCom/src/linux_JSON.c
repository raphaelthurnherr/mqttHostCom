/*
 ============================================================================
 Name        : linux_JSON.c
 Author      : Raph
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "linux_json.h"
#include "libs/lib_json/jRead.h"
#include "libs/lib_json/jWrite.h"

void SetAlgoidAck(char * buffer, ALGOID srcMessage, char * myID, char * ackMessage);
char GetAlgoidMsg(ALGOID destMessage, char *srcBuffer);

ALGOID myReplyMessage;

// -----------------------------------------------------------------------------
// MAIN, APPLICATION PRINCIPALE-------------------------------------------------
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// GetAlgoidMsg
// Get message from buffer and set in the message structure
// -----------------------------------------------------------------------------

char GetAlgoidMsg(ALGOID destMessage, char *srcBuffer){
	struct jReadElement element;
	int i;

	// ENTETE DE MESSAGE
		jRead_string((char *)srcBuffer, "{'To'", AlgoidMessageRX.msgTo, 15, NULL );
		jRead_string((char *)srcBuffer, "{'From'", AlgoidMessageRX.msgFrom, 15, NULL );
		AlgoidMessageRX.msgID= jRead_int((char *)srcBuffer,  "{'MsgID'", NULL);

	// MESSAGE TYPE
				char myDataString[20];
				// Clear string
				for(i=0;i<20;i++) myDataString[i]=0;

				jRead_string((char *)srcBuffer,  "{'Message'{'MsgType'",myDataString,15, NULL);

				AlgoidMessageRX.msgType=	-1;
				if(!strcmp(myDataString, "command")) AlgoidMessageRX.msgType = COMMAND;
				if(!strcmp(myDataString, "request")) AlgoidMessageRX.msgType = REQUEST;
				if(!strcmp(myDataString, "ack")) AlgoidMessageRX.msgType = ACK;
				if(!strcmp(myDataString, "response")) AlgoidMessageRX.msgType = RESPONSE;
				if(!strcmp(myDataString, "event")) AlgoidMessageRX.msgType = EVENT;
				if(!strcmp(myDataString, "negoc")) AlgoidMessageRX.msgType = NEGOC;
				if(!strcmp(myDataString, "error")) AlgoidMessageRX.msgType = ERROR;

	// MESSAGE PARAM
				// Clear string
				for(i=0;i<20;i++) myDataString[i]=0;
				jRead_string((char *)srcBuffer,  "{'Message'{'MsgParam'",myDataString,15, NULL);

				AlgoidMessageRX.msgParam=-1;
					if(!strcmp(myDataString, "forward")) AlgoidMessageRX.msgParam = FORWARD;
					if(!strcmp(myDataString, "back")) AlgoidMessageRX.msgParam = BACK;
					if(!strcmp(myDataString, "left")) AlgoidMessageRX.msgParam = LEFT;
					if(!strcmp(myDataString, "right")) AlgoidMessageRX.msgParam = RIGHT;
					if(!strcmp(myDataString, "rotateLeft")) AlgoidMessageRX.msgParam = ROTATE_LEFT;
					if(!strcmp(myDataString, "rotateRight")) AlgoidMessageRX.msgParam = ROTATE_RIGHT;
					if(!strcmp(myDataString, "stop")) AlgoidMessageRX.msgParam = STOP;
					if(!strcmp(myDataString, "2wd")) AlgoidMessageRX.msgParam = LL_WD;

	// DATA ARRAY
				  jRead((char *)srcBuffer, "{'Message'{'MsgValue'", &element );
				  if( element.dataType == JREAD_ARRAY )
				  {
					  AlgoidMessageRX.msgValueCnt=element.elements;
				      for(i=0; i<element.elements; i++ )    // loop for no. of elements in JSON
				      {
				    	  if(AlgoidMessageRX.msgParam == LL_WD){
					    	  jRead_string((char *)srcBuffer, "{'Message'{'MsgValue'[*{'wheel'", AlgoidMessageRX.msgValArray[i].wheel, 15, &i );
					    	  AlgoidMessageRX.msgValArray[i].velocity= jRead_long((char *)srcBuffer, "{'Message'{'MsgValue'[*{'velocity'", &i);
					    	  AlgoidMessageRX.msgValArray[i].time= jRead_long((char *)srcBuffer, "{'Message'{'MsgValue'[*{'time'", &i);
				    	  }
				    	  else{
					    	  jRead_string((char *)srcBuffer, "{'Message'{'MsgValue'[*{'mode'", AlgoidMessageRX.msgValArray[i].mode, 15, &i );
					    	  AlgoidMessageRX.msgValArray[i].value= jRead_long((char *)srcBuffer, "{'Message'{'MsgValue'[*{'value'", &i);
				    	  }

				    }
				  }

				  if(AlgoidMessageRX.msgParam < 0 || AlgoidMessageRX.msgType < 0){
					  return 0;
				  }else return 1;
}


// -----------------------------------------------------------------------------
// replyToHost
// convert the structure in JSON format & Send to host
// -----------------------------------------------------------------------------
void SetAlgoidAck(char * buffer, struct JsonCommand srcMessage, char * myID, char * ackMessage){
//	struct jWriteControl jwc;
	unsigned int buflen= 1024;
	int i;

// Formatage de la réponse en JSON
	jwOpen( buffer, buflen, JW_OBJECT, JW_PRETTY );		// start root object
		jwObj_string( "To", srcMessage.msgFrom );				// add object key:value pairs
		jwObj_string( "From", myID );				// add object key:value pairs
		jwObj_int( "MsgID", srcMessage.msgID );
		jwObj_object( "Message" );
			jwObj_string( "MsgType", ackMessage );				// add object key:value pairs
		jwEnd();
		jwClose();
}
