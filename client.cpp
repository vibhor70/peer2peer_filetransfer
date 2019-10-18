#include <bits/stdc++.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define PORT 8069
#define BUFFERSIZE 99999
#define OFFSET 40000
#define PEER_IP "127.0.0.1"

using namespace std;

int main(){
    int sockfd;
    struct  sockaddr_in myaddr;
    size_t recvLen;

    char BUFFER[BUFFERSIZE];

    if((sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        perror("Cannot create socket");
        return 0;
    }
    memset(&myaddr,0,sizeof(myaddr));

    /*Get the ip of the newtwork when client is newly connected*/
    string ip;
    int port;
    cout << "Please enter the IP of the network: ";
    cin >> ip;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    cout << "Please enter your IP address ";
    cin >> ip;

    /* connect with the network */
    if( connect(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr)) < 0 ){
        perror("connect Failed");
        return 0;
    }

    recv(sockfd, BUFFER, BUFFERSIZE, 0);  //directory structure
    cout << BUFFER << endl;

    string dirPath;
    cout << "Choose your path wisely for e.g. /home/jai/" << endl;
    cin >> dirPath;


    if( send(sockfd, dirPath.c_str(), dirPath.length(), 0) < 0 ){
        perror("Send to failed");
        return 0;
    }
    string fileName;
    for (int i = dirPath.length(); i > 0; i--){
        if(dirPath[i] == '/' || dirPath[i] == '\\'){
            fileName = fileName + dirPath[i];
        }
    }
    reverse(fileName.begin(), fileName.end());

    // DIR* dp = opendir(fileName.c_str());
    // if (dp == NULL) {
    //     fileName = [1] + fileName;
    // }
    cout << fileName << endl;
    
    int totalSize = 0, received;
    FILE *fp = fopen(fileName.c_str(), "wb");
    while(1){
        received= recv(sockfd,BUFFER,BUFFERSIZE,0);
        totalSize += received;
        if(received < 0){
            perror("Problem in recv");
            break;
        }
        else if(received == 0){ 
            recvLen = fwrite(BUFFER,sizeof(char),received,fp);
            break;
        }
        recvLen = fwrite(BUFFER,sizeof(char),received,fp);
    }
    cout << "Total size" << totalSize << endl;  

    close(sockfd);
    return 0;
}