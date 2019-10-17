#include <bits/stdc++.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT 8085
#define MAXLINE 1024 
#define PEER_IP "127.0.0.1"

int main(){
    int sockfd;
    struct  sockaddr_in myaddr;
    size_t newLen;

    char *source = NULL;
    FILE *fp = fopen("bbt.mp4", "rb");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(fp);

            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            source = new char[bufsize+1];

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            newLen = fread(source, sizeof(char), bufsize, fp);
            
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        
        printf("%lu",newLen);
        fclose(fp);
    }

    if((sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        perror("Cannot create socket");
        return 0;
    }
    memset(&myaddr,0,sizeof(myaddr));

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = inet_addr(PEER_IP);

    if( connect(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr)) < 0 ){
        perror("connect Failed");
        return 0;
    }

    int i=0,j=0,offset = 40000;
    int no = newLen/offset;
    int remain = newLen%offset;
    while(i<no){
        if(send(sockfd,(source+j),offset,0)<0){
            perror("Send to failed");
            return 0;
        }
        j+=offset;
        i++;
    }
    if(send(sockfd,source+j,remain,0)<0){
        perror("Send to failed");
        return 0;
    }
    close(sockfd);
    return 0;
}