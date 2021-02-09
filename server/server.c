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

int free_mem(char arr[],int s){
    for(int i=0;i<s;i++){
        arr[i]='\0';
    }
}

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
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    //valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer
    //int i=atoi(buffer);
    //printf("%d\n",i );
    int i;
    recv(new_socket,&i,sizeof(i),0);

    for(int j=0;j<i;j++){
        char xx[1024]={0};
        valread = read(new_socket,xx,1024);
        //printf("%s\n",xx );
        int fd=open(xx,O_RDONLY);
        if(fd<0){
            printf("No %s exists\n\n",xx );
            char *bl="f";
            send(new_socket,bl,strlen(bl),0);
        }
        else{
            printf("file %s exits\n\n",xx );
            char *bl="t";
            send(new_socket,bl,strlen(bl),0);

            long long int ofset=lseek(fd,0,SEEK_END);
            //printf("%lld\n",ofset );
            long long int c=ofset/10000;
            long long int len=ofset;
            long long int wrtbt=0;
            long long int p;
            //printf("%lld\n",c );
            send(new_socket,&ofset,sizeof(ofset),0);
            long long int y=ofset%10000;
            printf("Transferring data....\n\n");
            for(p=0;p<c;p++){
                ofset=lseek(fd,p*10000,SEEK_SET);
                char *arr=(char*)calloc(10005,sizeof(char));
                read(fd,arr,10000);
                send(new_socket,arr,strlen(arr),0);
                progress(ofset,len);
                free(arr);
            }
            //printf("%lld\n",y );
            if(y){
                ofset=lseek(fd,-y,SEEK_END);
                char *arr=(char*)calloc(10005,sizeof(char));
                read(fd,arr,10000);
                //printf("%s\n",arr );
                progress(len,len);
                send(new_socket,arr,strlen(arr),0);
                free(arr);
            }
            printf("\n\n");
            close(fd);

        }
    }
    if(i==0){
        while(1){
            int v;
            recv(new_socket,&v,sizeof(v),0);
            if(v==0){
                printf("exiting\n");
                break;
            }
            else if(v==1){
                //printf("files\n");
                int cnt;
                recv(new_socket,&cnt,sizeof(cnt),0);
                //printf("%d\n",cnt );
                for(int j=0;j<cnt;j++){
                    char xx[1024]={0};
                    valread = read(new_socket,xx,1024);
                    //printf("%s\n",xx );
                    int fd=open(xx,O_RDONLY);
                    if(fd<0){
                        printf("No %s exists\n",xx );
                        perror(" ");
                        char *bl="f";
                        send(new_socket,bl,strlen(bl),0);
                    }
                    else{
                        printf("file %s exits\n\n",xx );
                        char *bl="t";
                        send(new_socket,bl,strlen(bl),0);

                        long long int ofset=lseek(fd,0,SEEK_END);
                        //printf("%lld\n",ofset );
                        long long int c=ofset/10000;
                        long long int len=ofset;
                        long long int wrtbt=0;
                        long long int p;
                        //printf("%lld\n",c );
                        send(new_socket,&ofset,sizeof(ofset),0);
                        long long int y=ofset%10000;
                        for(p=0;p<c;p++){
                            ofset=lseek(fd,p*10000,SEEK_SET);
                            char *arr=(char*)calloc(10005,sizeof(char));
                            read(fd,arr,10000);
                            send(new_socket,arr,strlen(arr),0);
                            progress(ofset,len);
                            free(arr);
                        }
                        if(y){
                            ofset=lseek(fd,-y,SEEK_END);
                            char *arr=(char*)calloc(10005,sizeof(char));
                            read(fd,arr,10000);
                            //printf("%s\n",arr );
                            progress(len,len);
                            send(new_socket,arr,strlen(arr),0);
                            free(arr);
                        }
                        printf("\n\n\n");
                        close(fd);

                    }
                }
            }
            else{
                printf("wrong command\n\n");
            }
        }
    }
    return 0;
}