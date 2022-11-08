#include "database.h"

extern int errno;

UserInfo getUserInfo(int userID)
{
    //Opening File
    int fd = open(USERS, O_RDONLY,PERMS);

    //Locking File
    struct flock lock = {F_RDLCK,SEEK_SET
                            ,(userID-1)*sizeof(UserInfo),
                                    sizeof(UserInfo),getpid()};
    fcntl(fd, F_SETLKW, &lock);

    //Critical Section
    lseek(fd,(userID-1)*sizeof(UserInfo),SEEK_SET);
    UserInfo customer; read(fd, &customer,sizeof(UserInfo));

    //Unlocking
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLKW, &lock);
    
    //Closing FIle
    close(fd);

    return customer;
}

int getAccountID(int userID)
{
    //Extracting User Info - for account ID
    UserInfo customer = getUserInfo(userID);
    return customer.accountID;
}


int depositUpdate(int deposit, int userID)
{
    //Getting Account Number
    int accountID = getAccountID(userID);

    //Opening File
    int fd = open(VAULT,O_RDWR,PERMS);

    //Locking File
    struct flock lock = {F_WRLCK,SEEK_SET,
                            (accountID-1)*sizeof(AccountInfo),
                                    sizeof(AccountInfo),getpid()};

    fcntl(fd, F_SETLKW, &lock);
    
    //Critical Section 
    lseek(fd,(accountID-1)*sizeof(AccountInfo),SEEK_SET);
    
    //Reading -> New Data -> Writing
    AccountInfo wealth;read(fd, &wealth, sizeof(AccountInfo));
    wealth.balance += deposit;
    lseek(fd, (accountID-1)*(sizeof(AccountInfo)), SEEK_SET);
    write(fd, &wealth, sizeof(AccountInfo));
     
    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd,F_SETLKW, &lock);

    //Closing File
    close(fd);

    return 1;
}


int WithdrawUpdate(int withdraw, int userID)
{
    //Getting Account Number
    int accountID = getAccountID(userID);
    
    //Opening File
    int fd = open(VAULT,O_RDWR,PERMS);
    
    //Locking File
    struct flock lock = {F_WRLCK,SEEK_SET,
                                        (accountID-1)*sizeof(AccountInfo),
                                                sizeof(AccountInfo),getpid()};
    fcntl(fd, F_SETLKW, &lock);
    
    //Critical Section
    lseek(fd,(accountID-1)*sizeof(AccountInfo),SEEK_SET);
    AccountInfo wealth;read(fd, &wealth, sizeof(AccountInfo));

    //Error Condition
    if (wealth.balance < withdraw)  return -1;
    
    //Updating Balance
    wealth.balance -= withdraw;

    lseek(fd,(accountID-1)*(sizeof(AccountInfo)),SEEK_SET);
    write(fd,&wealth, sizeof(AccountInfo)); 
    
    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd,F_SETLKW, &lock);

    //Closing File
    close(fd);

    return 1;
}

int* SearchAccount(int userID)
{
    //Getting User Balance Info
    UserInfo user = getUserInfo(userID);

    //Returning Details
    static int accountDetails[4];
    accountDetails[0] = userID;
    accountDetails[1] = user.accountID;
    accountDetails[2] = user.accountType;
    accountDetails[3] = checkBalance(userID);
    return accountDetails;
}

int updatePassword(char *newPassword, int userID)
{ 
    //Opening File 
    int fd = open(USERS,O_RDWR,PERMS);

    //Locking File
    struct flock lock = {F_WRLCK,SEEK_SET,
                                (userID-1)*sizeof(UserInfo),
                                        sizeof(UserInfo),getpid()};
    fcntl(fd, F_SETLKW, &lock);

    //Critcial Section
    lseek(fd,(userID-1)*sizeof(UserInfo),SEEK_SET);
    UserInfo userData;read(fd, &userData, sizeof(UserInfo));

    //Updating Pass
    for(int i=0;i <sizeof(newPassword);i++)
        userData.password[i] = newPassword[i];

    lseek(fd,(userID-1)*sizeof(UserInfo),SEEK_SET);
    write(fd, &userData, sizeof(UserInfo));
    
    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLKW, &lock);

    //Closing File
    close(fd);

    return 1;
}

int checkBalance(int userID)
{
    //Getting Account ID
    int accountID = getAccountID(userID);

    //Opening File
    int fd = open(VAULT, O_RDWR);

    //Locking File
    struct flock lock = {F_RDLCK,SEEK_SET,
                                (accountID-1)*sizeof(AccountInfo),
                                        sizeof(AccountInfo),getpid()};
    fcntl(fd, F_SETLKW, &lock);
    
    //Critical Section
    lseek(fd,(accountID-1)*sizeof(AccountInfo),SEEK_SET);
    AccountInfo wealth;read(fd, &wealth, sizeof(AccountInfo));
    
    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd,F_SETLKW, &lock);

    //Closing File
    close(fd);
    
    return wealth.balance;
}


int CheckLogin(InfoPair *loginDetails) 
{
    //Chekcing UserName
    int userID = loginDetails->ID;
    UserInfo customer = getUserInfo(userID);

    //Comparing User IDs
    if(customer.ID != userID) return -1;

    //Comparing Passwords
    char* correctPwd = customer.password;
    char* givenPwd = loginDetails->password;

    return compareString(correctPwd,givenPwd);
} 


int CheckAdminLogin(AdminInfo *loginData) 
{ 
    //Opening File
    int fd = open(ADMIN, O_RDONLY,PERMS);

    //Mandatory Locking
    struct flock lock = {F_RDLCK,SEEK_SET,0,0,getpid()};
    fcntl(fd, F_SETLKW, &lock);
    
    //Critical Section
    int status = -1;

    while(1)
    {
        //Reading All Records Until EOF
        AdminInfo acutalData;
        if(!read(fd, &acutalData, sizeof(AdminInfo))) break;

        //Comparing With All Records
        int nameCorrect = compareString(acutalData.username,loginData->username);
        int pswdCorrect = compareString(acutalData.password,loginData->password);

        if(nameCorrect == 1 & pswdCorrect == 1)
        {
            status = 1;
            break;
        }
    }

    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLKW, &lock);

    //Closing File
    close(fd);

    return status;
} 

int RemoveAccount(int accountNumber)
{
    /*Removing Account From Account Database*/

    int fd = open(VAULT, O_RDWR,PERMS);
    struct flock lock = {F_WRLCK,SEEK_SET,0,0,getpid()};
    fcntl(fd, F_SETLKW, &lock);int flag = -1;
    while(1)
    {
        AccountInfo temp;AccountInfo deadAccount = {-1,-1};
        if(!read(fd,&temp,sizeof(AccountInfo))) break;

        if(temp.ID == accountNumber)
        {
            lseek(fd,-sizeof(AccountInfo), SEEK_CUR);
            write(fd, &deadAccount, sizeof(AccountInfo));
            flag = 1;
            break;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);

    //No such Account 
    if(flag == -1) return -1;
    
    /*Removing Account From User Database*/

    fd = open(USERS, O_RDWR,PERMS);
    fcntl(fd, F_SETLK, &lock);flag = -1;
    while(1)
    {
        UserInfo temp;
        if(!read(fd,&temp,sizeof(UserInfo))) break;
        temp.ID = -1;
        if(temp.accountID == accountNumber)
        {
            lseek(fd,-sizeof(UserInfo), SEEK_CUR);
            write(fd, &temp, sizeof(UserInfo));
            flag = 1;
            break;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return flag;

}

int EditAccountType(int accountNumber, int accountType)
{
    //Opening File
    int fd = open(USERS, O_RDWR,PERMS);

    //Mandatory Locking    
    struct flock lock = {F_WRLCK,SEEK_SET,0,0,getpid()};
    fcntl(fd, F_SETLKW, &lock);

    //Critical Section
    int flag = -1;
    while(1)
    {
        UserInfo temp;
        if(!read(fd,&temp,sizeof(UserInfo))) break;
        
        if(temp.accountID == accountNumber)
        {
            temp.accountType = accountType;
            lseek(fd,-sizeof(UserInfo), SEEK_CUR);
            write(fd, &temp, sizeof(UserInfo));
            flag = 1;
            break;
        }
    }    
    //Unlocking File
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLKW, &lock);

    //Closing File
    close(fd);

    return flag;
}

int updateAdminPassword(char *newPassword, char *username)
{
    //Finding Admin -> From UserName 
    int fd = open(ADMIN,O_RDWR,PERMS);
    struct flock lock = {F_WRLCK,SEEK_SET,0,0,getpid()};

    fcntl(fd, F_SETLKW, &lock);

    //Updating Password Accordingly 
    AdminInfo admin;int status = -1;
    while(read(fd,&admin,sizeof(AdminInfo)))
    {
        if (compareString(admin.username,username) == 1)
        {
            for(int i=0;i<sizeof(newPassword);i++)  
                admin.password[i] = newPassword[i];

            lseek(fd,-sizeof(AdminInfo), SEEK_CUR);
            write(fd,&admin,sizeof(AdminInfo));

            status = 1;
            break;
        }
    }
    
    //Unlocking & Closing
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLKW, &lock);
    close(fd);

    return status;
}

int AddNewAccount(int accountNumber)
{
    
    /*Adding User To Accounts Database*/
    int fd = open(VAULT, O_RDWR | O_APPEND,PERMS);
    struct flock lock = {F_WRLCK,SEEK_SET,0,0,getpid()};
    fcntl(fd, F_SETLKW, &lock);

    AccountInfo baseAccount = {0};baseAccount.ID = accountNumber;
    write(fd, &baseAccount, sizeof(AccountInfo));

    lock.l_type = F_UNLCK;fcntl(fd,F_SETLKW, &lock);
    close(fd);

    /*Adding User To Users Database*/
    fd = open(USERS,O_RDWR,PERMS);
    fcntl(fd,F_SETLKW,&lock);    
    
    //Finding Max User ID 
    int maxUserID = 1;
    while(1)
    {
        UserInfo temp;
        if(!read(fd,&temp,sizeof(UserInfo))) break;
        maxUserID += 1;
    }

    //Updating Database
    UserInfo newUser;
    newUser.ID = maxUserID; 
    newUser.accountID =accountNumber; 
    newUser.accountType = 0;

    char defaultPwd[50] = DPSWD;
    for(int i=0;i<sizeof(defaultPwd);i++)
        newUser.password[i] = defaultPwd[i];
    
    write(fd, &newUser, sizeof(newUser)); 
    
    lock.l_type = F_UNLCK;fcntl(fd, F_SETLK, &lock);
    close(fd);

    return 1;
}

//Compares Strings 1 : Same String -1 : Different Strings 
int compareString(char string1[],char string2[])
{
    while(*string1 && *string2)
    {
        if(*string1 != *string2) 
            return -1;

        *string1++;
        *string2++;
    }

    if(*string1 == *string2)
        return 1;
    else
        return -1;
}


