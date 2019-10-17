#include <bits/stdc++.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

using namespace std;

#define PORT 8085
#define MAXLINE 99999 

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
    // serv_addr.sin_addr.s_addr =  htonl(0);
    serv_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

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
        socklen_t clilength = sizeof(cli_addr);
        int clisock = accept(sockfd,(struct sockaddr*)&cli_addr,&(clilength));
        if(clisock < 0){
            printf(" problem in server client socket %s",inet_ntoa(cli_addr.sin_addr));
        }
        int recvng;

        if ((pid = fork()) == -1){
            close(clisock);
            continue;
        }
        else if(pid == 0){
            //child
            char fname[100];
            // getchar();
            // cout << "Enter file name: ";
            // cin >> fname;
            fp1 = fopen("fname.mp4","wb");
            int totalSize = 0;
            while(1){
                recvng= recv(clisock,msg,MAXLINE,0);
                totalSize += recvng;
                if(recvng < 0){
                    perror("Problem in recv");
                    break;
                }
                else if(recvng == 0){ 
                    recvLen = fwrite(msg,sizeof(char),recvng,fp1);
                    break;
                }
                recvLen = fwrite(msg,sizeof(char),recvng,fp1);
            }
            cout << "Total size" << totalSize << endl;  
            close(clisock);
            close(lsn);
        }

        else if(pid > 0) { //parent
            close(clisock);
            // counter++;
            continue;
        }

        printf("Msg from client %lu\n",recvLen);
    }
    
    fclose(fp1);
    return 0;
}