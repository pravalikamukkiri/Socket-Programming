// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#define PORT 8000

void progress(long long int x,long long int y){
    char prog[50]={0};
    double p=((x)*1.0/y)*100;
    sprintf(prog," %.2lf",p);
    printf("\r");
    printf("%.2lf",p );
    //write(1,"\r",strlen(prog));
    //write(1,prog,strlen(prog));
    //printf("%.2lf\n",p );
    fflush(stdout);

}
void free_array(char arr[],long long int n){
    for(long long int i=0;i<n;i++)
        arr[i]='\0';
}

int main(int argc, char const *argv[])
{
    //printf("s\n");
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int i=argc-1;

    send(sock,&i,sizeof(i),0);
    i=1;
    for(i=1;i<argc;i++){
        //printf("%s\n",argv[i] );
        send(sock,argv[i],strlen(argv[i]),0);
        int x;
        char xx[1]={0};
        x=read(sock,xx,1);
        //printf("%s\n",xx );
        if(xx[0]=='t'){
            printf("\nfile %s found\n\n",argv[i] );
            printf("Downloading....\n");

            long long int ofset;
            x=recv(sock,&ofset,sizeof(ofset),0);
            long long int len=ofset;
            long long int c=ofset/10000;

            int fy=open(argv[i],O_RDWR | O_CREAT| O_TRUNC,0770);

            int y=ofset%10000;

            for(long long int p=0;p<c;p++){
                char *arr=(char*)calloc(10005,sizeof(char));
                x=read(sock,arr,10000);
                write(fy,arr,10000);
                progress(p*10000,len);
                free(arr);
            }
            if(y){
                char *arr=(char*)calloc(10005,sizeof(char));
                x=read(sock,arr,10000);
                write(fy,arr,strlen(arr));
                progress(len,len);
                free(arr);
            }
            close(fy);
            printf("\n");

        }
        else{
            printf("file %s not found\n\n",argv[i] );
        }
    }
    if(i==1){
        while(1){
            printf("client>  ");
            char *str=(char*)calloc(10005,sizeof(char));
            fgets(str,2000,stdin);
            char args[100][100];
            char *token;
            token=strtok(str," \n\t\r");
            int j;
            for(j=0;j<100;j++)
                free_array(args[j],100);
            j=0;
            while(token != NULL){
                strcpy(args[j],token);
                j++;
                token=strtok(NULL," \n\t\r");
            }
            if(strcmp(args[0],"get")==0){
                int v=1;
                send(sock,&v,sizeof(v),0);
                //printf("%d\n",j );
                j--;
                send(sock,&j,sizeof(j),0);
                if(j==0){
                    printf("please enter a file name to download\n\n\n");
                }
                for(int f=1;f<=j;f++){
                    //printf("%s\n",args[f] );
                    send(sock,args[f],strlen(args[f]),0);
                    int x;
                    char xx[1]={'\0'};
                    x=read(sock,xx,1);
                    //printf("%s\n",xx );
                    if(xx[0]=='t'){
                        printf("\nfile %s found\n\n",args[f] );
                        printf("Downloading....\n");

                        long long int ofset;
                        x=recv(sock,&ofset,sizeof(ofset),0);
                        long long int len=ofset;
                        long long int c=ofset/10000;

                        int fy=open(args[f],O_RDWR | O_CREAT| O_TRUNC,0770);

                        int y=ofset%10000;

                        for(long long int p=0;p<c;p++){
                            char *arr=(char*)calloc(10005,sizeof(char));
                            x=read(sock,arr,10000);
                            write(fy,arr,10000);
                            progress(p*10000,len);
                            free(arr);
                        }
                        if(y){
                            char *arr=(char*)calloc(10005,sizeof(char));
                            x=read(sock,arr,10000);
                            write(fy,arr,strlen(arr));
                            progress(len,len);
                            free(arr);
                        }
                        close(fy);
                        printf("\n\n\n");

                    }
                    else{
                        printf("file %s not found\n\n\n",args[f] );
                    }
                }

            }
            else if(strcmp(args[0],"exit")==0){
                printf("exiting\n");
                int v=0;
                send(sock,&v,sizeof(v),0);
                break;
            }
            else{
                int v=2;
                printf("wrong command\n\n");
                send(sock,&v,sizeof(v),0);

            }

        }
    }
    return 0;
}