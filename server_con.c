#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 8085
#define MAXLINE  99999 

int main(){
    size_t recvLen,newLen = 46258;
    char msg[MAXLINE];
    int sockfd;
    FILE *fp1;

    struct sockaddr_in serv_addr,cli_addr;
    if((sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        perror("Server socket not created");
    } 
    memset (&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr =  htonl(+);

    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
        perror("Bind Failed");
        return 0;
    }
    int lsn = listen(sockfd,5);
    
    if(lsn<0){
        perror("Error in listening");
    }

    int pid=0;
    while(1){
        int clilength = sizeof(cli_addr);
        int clisock = accept(sockfd,(struct sockaddr*)&cli_addr,&(clilength));
        if(clisock < 0){
            printf(" problem in server client socket %s",inet_ntoa(cli_addr.sin_addr));
        }
        pid = fork();
        int recvng;
        if(pid == 0){
            fp1 = fopen("Recv.mp4","wb");
            while(1){
                recvng= recv(clisock,msg,MAXLINE,0);
                if(recvng <0){
                    perror("Problem in recv");
                }
                else if(recvng == 0){ 
                    recvLen = fwrite(msg,sizeof(char),recvng,fp1);
                    break;
                }

                recvLen = fwrite(msg,sizeof(char),recvng,fp1);
            }    
        }
        else {
            close(sockfd);//sock is closed BY PARENT
        }

        printf("Msg from client %lu\n",recvLen);
    }
    
    fclose(fp1);
    return 0;
}