#include "server.h"

int main()
{
     //Configuring Server
    int rawSD = configureServer(INADDR_ANY,SERVERPORT,QUEUELENGTH);
    perror("Server Configuration ");

    //Accepting Clients
    while(1)
    {
        //Accepting Connections
        struct sockaddr_in client = {0};int size = sizeof(client);
        int clientSD = accept(rawSD, (struct sockaddr *)&client,&size);

        if(!fork())
        {
            fprintf(stderr,"Connection From %s : ",inet_ntoa(client.sin_addr));
            perror("");

            login(clientSD);

            close(clientSD);
            printf("Disconnected From %s : \n",inet_ntoa(client.sin_addr));
        }
        else
            close(clientSD);
    }
    
    return 0;
}

//Functions
void NormalUser(int client,int userID, long sessionID)
{
    int requestChoice;long receivedID;
    
    //Getting Session ID & Request Choice
    read(client, &receivedID, sizeof(long));
    read(client, &requestChoice, sizeof(int));

    //Correct Session
    if (receivedID == sessionID)
    { 
        //Menu Driven System
        switch(requestChoice)
        {
            case 1:
                DepositQuery(client, userID);
                break;

            case 2:
                WithdrawQuery(client, userID);
                break;

            case 3:
                BalanceQuery(client, userID);
                break;

            case 4:
                pwdChangeQuery(client, userID);
                break;

            case 5:
                DetailQuery(client, userID);
                break;

            case 6:
                exitLogin(client);
                break;
        } 
    }
    
    //Answering Queries Again
    NormalUser(client, userID, sessionID);
}

void AdminUser(int client, long sessionID)
{
    int queryType;long receivedID;

    //Getting Session ID & Request Choice
    read(client, &receivedID, sizeof(long));
    read(client, &queryType, sizeof(int));

    //Correct Session
    if (receivedID == sessionID)
    { 
        //Menu Driven System
        switch(queryType)
        {
            case 1:
                AddAccount(client);
                break;

            case 2:
                DeleteAccount(client);
                break;

            case 3:
                ChangeAccountType(client);
                break;

            case 4:
                ChangeAdminPassword(client);
                break;

            case 5:
                AccountDetails(client);
                break;

            case 6:
                exitLogin(client);
                break;
        } 
    }

    //Answering Queries Again
    AdminUser(client, sessionID);
}

void AccountDetails(int client)
{
    //Reading UserId
    int userID;
    read(client, &userID, sizeof(userID));

    //Servicing Query
    DetailQuery(client, userID);
}

void ChangeAdminPassword(int client)
{
    char newpassword[50],username[50];

    //Reading Data From Client
    read(client,newpassword,50*sizeof(char));
    read(client, username,50*sizeof(char));

    //Modifying & Returning Status
    int status = updateAdminPassword(newpassword, username);
    write(client, &status, sizeof(int)); 
}

void ChangeAccountType(int client)
{
    int accountNumber,accountType; 

    //Reading Data From Client
    read(client, &accountNumber, sizeof(int));
    read(client, &accountType, sizeof(int)) ;

    //Modifying & Returning Status
    int status = EditAccountType(accountNumber, accountType);
    write(client, &status, sizeof(int)); 
}

void DeleteAccount(int client)
{
    //Getting Account Number
    int accountNumber;
    read(client, &accountNumber, sizeof(int));

    //Returning Status
    int status = RemoveAccount(accountNumber);
    write(client, &status, sizeof(int));
}

void AddAccount(int client)
{
   //Getting Account Number
   int accountNumber;
   read(client, &accountNumber, sizeof(accountNumber));

   //Returning Status
   int status = AddNewAccount(accountNumber);
   write(client, &status, sizeof(int));
}

void exitLogin(int client)
{
    //Closing Connection
    close(client);

    if (client == -1)
        printf("Closing Admin Connection\n");
    else
        printf("Client %i connection Closed\n",client);

    //Stoping Execution
    exit(0);
}

void pwdChangeQuery(int client, int userID)
{
    //Getting New Password Data
    char newpassword[50];
    read(client, newpassword, sizeof(newpassword));

    //Chainging Password
    int returnValue = updatePassword(newpassword,userID);

    //Sending Status
    write(client, &returnValue, sizeof(int));
}

void DetailQuery(int client, int userID)
{
    //Sending User Details
    int *userDetails = (int *)malloc(sizeof(int)*4);
    userDetails = SearchAccount(userID); 
    write(client, userDetails, sizeof(int)*4);
    //free(userDetails);
} 

void BalanceQuery(int client, int userID)
{
    //Sending BalanceQuery Data
    int BalanceQuery = checkBalance(userID);
    write(client, &BalanceQuery, sizeof(BalanceQuery));
}

void DepositQuery(int client, int userID)
{
    //Getting Data From Client
    int amountDeposited;
    read(client, &amountDeposited, sizeof(int));

    //Returning The New BalanceQuery
    int BalanceQuery = depositUpdate(amountDeposited,userID);
    write(client, &BalanceQuery, sizeof(BalanceQuery));
}

void WithdrawQuery(int client, int userID)
{
    //Getting Data From Client
    int withdrawAmount;
    read(client,&withdrawAmount,sizeof(int));

    //Returning The New BalanceQuery
    int BalanceQuery = WithdrawUpdate(withdrawAmount, userID);
    write(client, &BalanceQuery, sizeof(BalanceQuery));
}

void normallogger(int client)
{
    //Collecting Credentials
    InfoPair loginDetails;
    read(client,&loginDetails, sizeof(InfoPair));

    //Validating Login ID
    int validity = CheckLogin(&loginDetails);

    //Generating Session ID
    long sessionID;
    (validity == 1)?(sessionID = random()):(sessionID = -1);

    //Sending Back Session ID
    write(client, &sessionID, sizeof(sessionID));  

    if (validity == 1)
        NormalUser(client,loginDetails.ID,sessionID);
    else
    {
        printf("\n");
        login(client);
    }
} 

void adminLogger(int client)
{  
    //Collecting Credentials
    AdminInfo loginDetails;
    read(client,&loginDetails, sizeof(AdminInfo));

    //Validating Login ID
    int validity = CheckAdminLogin(&loginDetails);

    //Generating Session ID
    long sessionID;
    (validity == 1)?(sessionID = random()):(sessionID = -1);

    //Sending Back Session ID
    write(client, &sessionID, sizeof(sessionID));  

    if (validity == 1)
        AdminUser(client,sessionID);
    else
    {
        printf("\n");
        login(client);
    }
}

void login(int client)
{
    //Type of Login
    int loginChoice;
    int status = read(client,&loginChoice,sizeof(int));

    switch(loginChoice)
    {
        case 0:
            normallogger(client);
            break;

        case 1:
            adminLogger(client);
            break;
    } 
}


int configureServer(int serverAddress,int serverPort,int queueLength)
{
    //Configuring Server
    struct sockaddr_in server;

    //Making Raw Socket
    int rawSD = socket(AF_INET, SOCK_STREAM, 0);

    //Enabling Multiple use of Port & address
    int option = 1;
    setsockopt(rawSD,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&option,sizeof(option));

    //Binding With Server IP & Port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = serverAddress;
    server.sin_port = htons(serverPort);
    bind(rawSD, (struct sockaddr *)&server, sizeof(server));

    //Waiting For Connections
    listen(rawSD, queueLength);

    return rawSD;
}

