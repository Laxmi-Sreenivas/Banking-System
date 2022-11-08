#pragma once


//Common Structs Used
typedef struct InfoPair
{
    int ID;
    char password[50];
}InfoPair;

typedef struct UserInfo
{
    int ID;
    char password[50];
    int accountID;
    int accountType;
}UserInfo;

typedef struct AdminInfo
{
    char username[50];
    char password[50];
}AdminInfo;

typedef struct AccountInfo
{
    int ID;
    int balance;
}AccountInfo;


