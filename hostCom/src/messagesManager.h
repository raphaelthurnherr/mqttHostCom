/*
 * messagesManager.h
 *
 *  Created on: 15 mars 2016
 *      Author: raph
 */

#ifndef MESSAGESMANAGER_H_
#define MESSAGESMANAGER_H_

extern char ClientID[50];

// Initialisation de la messagerie system (JSON<->MQTT)
int InitMessager(void);
int CloseMessager(void);

#endif /* MESSAGESMANAGER_H_ */
