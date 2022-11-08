#pragma once

//Includes - Defined
#include "common.h"

//Inclucdes - std
#include<stdio.h>
#include<stdlib.h>

//Includes  - System Calls
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include <errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//Marcos
#define SERVERPORT 5555

//Prototypes
int connectToServer(int serverAddress,int serverPort);
void login(int sd);
void ExitHandler(long sessionID, int sd);
void DetailsQuery(long sessionID, int sd);
void ChangePassword(long sessionID, int sd);
void BalanceQuery(long sessionID, int sd);
void DepositMoney(long sessionID, int sd);
void WithdrawMoney(long sessionID, int sd);
void viewAccountQuery(long sessionID, int sd);
void ChangeAdminPassword(long sessionID, int sd, char *username);
void EditAccountType(long sessionID, int sd);
void DeleteAccount(long sessionID, int sd);
void AddAccount(long sessionID, int sd);
void NormalUser(long sessionID, int sd);
void AdminlUser(long sessionID, int sd, char *username);
void NormalLogger(int client);
void AdminLogger(int client); 




