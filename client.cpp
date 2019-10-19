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

#define PORT 8071
#define BUFFERSIZE 100000
#define OFFSET 4000
#define PEER_IP "127.0.0.1"

using namespace std;

int main(){
    int sockfd;
    struct  sockaddr_in myaddr;
    size_t recvLen;
    char* BUFFER;

    if((sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        perror("Cannot create socket");
        return 0;
    }
    memset(&myaddr,0,sizeof(myaddr));

    /*Get the ip of the newtwork when client is newly connected*/
    string ip;
    int port;

    cout << "Please enter your IP address: ";
    cin >> ip;

    cout << "Please enter the IP of the network: ";
    cin >> ip;
    ip = "127.0.0.1";

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    cout << "Waiting for the directory structure...";
    /* connect with the network */
    if( connect(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr)) < 0 ){
        perror("connect Failed");
        return 0;
    }

    BUFFER = new char[BUFFERSIZE];
    recv(sockfd, BUFFER, BUFFERSIZE, 0);  //directory structure
    cout << endl << "Here is the directory list" << endl;
    cout << BUFFER << endl;
    delete BUFFER;

    char dirPath[200];
    cout << "Choose your path of the file: ";
    cin >> dirPath;

    if( send(sockfd, dirPath, 200, 0) < 0 ){
        perror("Send to failed");
        return 0;
    }

    char saveAs[1000];
    cout << "Saving the file with name as: ";
    cin >> saveAs;
    FILE* fp = fopen(saveAs, "wb");
        
    int totalSize = 0, received;
    float progress = 0.0;

    while(1){
        int barWidth = 100;
        cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        BUFFER = new char[BUFFERSIZE];
        received = recv(sockfd, BUFFER, BUFFERSIZE, 0);
        cout << received << "kB" << endl;

        cout << "] " << int(progress/1000) << "kB \r";
        cout.flush();
        progress += BUFFERSIZE; // for demonstration only

        totalSize += received;
        if(received < 0){
            perror("Problem in recv");
            break;
        }
        else if(received == 0){ 
            recvLen = fwrite(BUFFER,sizeof(char),received,fp);
            break;
        }
        recvLen = fwrite(BUFFER, sizeof(char), received, fp);
        delete BUFFER;
    } // end of while
    cout << "Total size " << totalSize/1000 << "kB" << endl;  
    fclose(fp);
    close(sockfd);
    return 0;
}