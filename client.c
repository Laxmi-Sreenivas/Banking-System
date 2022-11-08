#include "client.h"

int main()
{
    //Connecting To The Server
    int sd = connectToServer(INADDR_ANY,SERVERPORT);
    perror("Connection Status ");

    //Logging In
    login(sd);

    //Exiting
    close(sd);
    perror("Connection Terminated ");
    return 0;
}

int connectToServer(int serverAddress,int serverPort)
{
    //Making Raw Socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    //Connecting To Server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = serverAddress;
    server.sin_port = htons(serverPort);
    connect(sd, (struct sockaddr*)&server, sizeof(server));

    return sd;
}

void login(int sd)
{
    //Displaying Choice
    printf("*******Type*********\n");
    printf("0 : Normal User\n");
    printf("1 : Admin User\n");

    //Logging According UserType
    int choice;scanf("%i", &choice);
    switch(choice)
    {
        case 0:
            NormalLogger(sd);
            break;
        case 1:
            AdminLogger(sd);
            break;
    }

}

void ExitHandler(long sessionID, int sd)
{
    //Sending Query To Server
    int queryType = 6;
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));

    //Closing Connection
    close(sd);
    perror("Disconnected ");

    exit(0);
}

void DetailsQuery(long sessionID, int sd)
{
    //Sending Query 
    int queryType = 5;int accountDetails[4];

    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int)); 
    
    //Receiving Details
    read(sd, accountDetails, 4*sizeof(int));
    
    char* fields[4] = {"UserID: ","Account No : ","AccountType : ","Account Balance : "};
    for(int i=0;i<4;i++)
        printf("%s%d\n",fields[i],accountDetails[i]);
}

void ChangePassword(long sessionID, int sd)
{
    int queryType = 4;char password[50];

    //Taking Input
    printf("Enter New Password : ");scanf("%s",password);

    //Sending Query 
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, password, 50*sizeof(int));

    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status > 0)
        printf("Password Changed\n");
    else
        printf("Query Failed\n");
}

void BalanceQuery(long sessionID, int sd)
{
    int queryType = 3;

    //Sending Query 
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));

    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status > 0)
        printf("Present Balance : %i\n", status);
    else
        printf("Query Failed\n");
}

void DepositMoney(long sessionID, int sd)
{
    int queryType = 1;

    //Taking Inputs
    printf("Deposit Money : ");
    int deposit;scanf("%i", &deposit);
    
    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, &deposit, sizeof(int));

    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status == 1)
        printf("Money Deposited\n");
    else
        printf("Query Failed\n");
}

void WithdrawMoney(long sessionID, int sd)
{
    int queryType = 2;

    //Taking Inputs
    printf("Withdraw Amount : ");
    int withdraw;scanf("%i", &withdraw);
    
    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, &withdraw, sizeof(int));
    
    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status == 1)
        printf("Money Withdrawn\n");
    else
        printf("Query Failed\n");
}

void viewAccountQuery(long sessionID, int sd)
{
    int queryType = 5;int accountDetails[4];

    //Taking Inputs
    printf("User ID : ");
    int userID;scanf("%i", &userID);

    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, &userID, sizeof(int));

    //Receiving Details 
    read(sd, accountDetails, 4*sizeof(int));

    char* fields[4] = {"UserID: ","Account No : ","AccountType : ","Account Balance : "};
    for(int i=0;i<4;i++)
        printf("%s%d\n",fields[i],accountDetails[i]);
}

void ChangeAdminPassword(long sessionID, int sd, char *username)
{
    int queryType = 4;

    //Taking input
    printf("New Password : ");
    char newPassword[50];scanf("%s",newPassword);

    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, newPassword, 50*sizeof(char));
    write(sd, username, sizeof(username));

    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status == 1)
        printf("Password Changed\n");
    else
        printf("Query Failed\n");

}

void EditAccountType(long sessionID, int sd)
{
    //Taking Inputs
    printf("Account Number : ");
    int accountNumber;scanf("%d", &accountNumber);
    
    printf("New Account Type:\n");
    printf("1.Single Account\n");
    printf("2.Joint Account\n");

    int accountType;scanf("%i", &accountType);

    //Sending Query
    int queryType = 3;    
    write(sd,&sessionID,sizeof(long));
    write(sd,&queryType,sizeof(int));
    write(sd,&accountNumber,sizeof(int));
    write(sd,&accountType,sizeof(int));

    //Checking Status
    int status;read(sd,&status,sizeof(status));

    if (status == 1)
        printf("Type Changed : %i\n",accountType);
    else
        printf("Query Failed\n");

}
void DeleteAccount(long sessionID, int sd)
{
    int queryType = 2;

    //Taking Inputs
    printf("Account Number : ");
    int accountNumber;scanf("%i", &accountNumber);
    
    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, &accountNumber, sizeof(int));

    //Checking Status
    int status;
    read(sd, &status, sizeof(status));
    if (status == 1)
        printf("Account Deleted : %i\n",accountNumber);
    else
        printf("Query Failed\n");
}

void AddAccount(long sessionID, int sd)
{
    int queryType = 1;

    //Taking Input
    printf("Account Number : ");
    int accountNumber;scanf("%i", &accountNumber);
    
    //Sending Query
    write(sd, &sessionID, sizeof(long));
    write(sd, &queryType, sizeof(int));
    write(sd, &accountNumber, sizeof(int));
    
    //Checking Status
    int status;read(sd, &status, sizeof(int));
    if (status == 1)
        printf("Account Added : %i\n", accountNumber);
    else
        printf("Query Failed\n");
}

void NormalUser(long sessionID, int sd)
{
    printf("*******Choice*********\n");

    //Displaying Fields
    char* fields[6] = {"1.Deposit","2.Withdraw","3.Balance Query",
                       "4.Change Password","5.Account Details","6.Exit"};

    for(int i=0;i<6;i++)
        printf("%s\n",fields[i]);

    //Menu Driven
    int choice;scanf("%d", &choice);
    printf("******%s******\n",fields[choice-1]);

    switch(choice)
    {
        case 1:
            DepositMoney(sessionID, sd);
            break;
        case 2:
            WithdrawMoney(sessionID, sd);
            break;
        case 3:
            BalanceQuery(sessionID, sd);
            break;
        case 4:
            ChangePassword(sessionID, sd);
            break;
        case 5:
            DetailsQuery(sessionID, sd);
            break;
        case 6:
            ExitHandler(sessionID, sd);
            break;
    }

    NormalUser(sessionID, sd);
}

void AdminlUser(long sessionID, int sd, char *username)
{
    printf("*******Choice*********\n");

    //Displaying Fields
    char* fields[6] = {"1.Add Account","2.Delete Account","3.Change Account Type",
                       "4.Change Password","5.Account Details","6. Exit"};
    
    for(int i=0;i<6;i++)
        printf("%s\n",fields[i]);

    //Menu Driven
    int choice;scanf("%i", &choice);
    printf("******%s******\n",fields[choice-1]);
    
    switch(choice)
    {
        case 1:
            AddAccount(sessionID, sd);
            break;
        case 2:
            DeleteAccount(sessionID, sd);
            break;
        case 3:
            EditAccountType(sessionID, sd);
            break;
        case 4:
            ChangeAdminPassword(sessionID, sd, username);
            break;
        case 5:
            viewAccountQuery(sessionID, sd);
            break;
        case 6:
            ExitHandler(sessionID, sd);
            break;
    }

    AdminlUser(sessionID, sd, username);
}

void NormalLogger(int sd)
{
    //Sending Server : Login Type
    int loginType = 0;
    write(sd, &loginType, sizeof(int));
    
    //Taking Input
    InfoPair loginDetails;
    printf("********Login******** \n");
    printf("UserID: ");
    scanf("%i",&loginDetails.ID);

    printf("Password: ");
    scanf("%s",loginDetails.password);
    
    //Sending Crendentials
    int ret = write(sd,&loginDetails,sizeof(InfoPair));
    
    //Checking Session Status
    long sessionID;read(sd, &sessionID, sizeof(long));
    if (sessionID == -1)
        login(sd);
    else
        NormalUser(sessionID, sd); 
}

void AdminLogger(int sd)
{
    //Sending Server : Login Type
    int loginType = 1;
    write(sd, &loginType, sizeof(int));

    //Taking Input
    AdminInfo loginDetails;
    printf("*********Admin Login*********\n");
    printf("username: ");
    scanf("%s",loginDetails.username);
    printf("password: ");
    scanf("%s",loginDetails.password);

    //Sending Credentials
    int ret = write(sd,&loginDetails, sizeof(AdminInfo));
    
    //Checking Session Status
    long sessionID;read(sd, &sessionID, sizeof(long));
    if (sessionID == -1)
        login(sd);
    else
        AdminlUser(sessionID, sd, loginDetails.username); 
}
