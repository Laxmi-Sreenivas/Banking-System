#pragma once

//Includes - Defined
#include "common.h"

//Includes - STD
#include <stdio.h>
#include <string.h>

//Includes - System Calls
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<errno.h>

//MACROS
#define PERMS 0744
#define USERS "Users.txt"
#define VAULT "Accounts.txt"
#define ADMIN "Admins.txt"
#define DPSWD "MoonINAN"

//Prototypes
UserInfo getUserInfo(int userID);
int getAccountID(int userID);
int depositUpdate(int deposit, int userID);
int WithdrawUpdate(int withdraw, int userID);
int* SearchAccount(int userID);
int updatePassword(char *newPassword, int userID);
int checkBalance(int userID);
int CheckLogin(InfoPair *loginDetails);
int compareString(char string1[],char string2[]);
int CheckAdminLogin(AdminInfo *loginDetails);
int RemoveAccount(int value);
int EditAccountType(int accountNumber, int accountType);
int updateAdminPassword(char *newPassword, char *username);
int AddNewAccount(int accountNumber);
