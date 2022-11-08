#pragma once

//Includes - Defined
#include "database.h"
#include "common.h"

//Inclucdes - std
#include<stdio.h>
#include<stdlib.h>

//Includes  - System Calls
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//Marcos
#define SERVERPORT 5555
#define QUEUELENGTH 10

//Prototypes
void NormalUser(int client,int userID, long sessionID);
void AdminUser(int client, long sessionID);
void AccountDetails(int client);
void ChangeAdminPassword(int client);
void ChangeAccountType(int client);
void DeleteAccount(int client);
void AddAccount(int client);
void exitLogin(int client);
void pwdChangeQuery(int client, int userID);
void DetailQuery(int client, int userID);
void BalanceQuery(int client, int userID);
void DepositQuery(int client, int userID);
void WithdrawQuery(int client, int userID);
void normallogger(int client);
void adminLogger(int client);
void login(int client);
int configureServer(int serverAddress,int serverPort,int queueLength);