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
#include <regex> 

#define PORT 8085
#define BUFFERSIZE 99999
#define OFFSET 40000
#define PEER_IP "127.0.0.1"

using namespace std;

int main(){


    int sockfd;
    struct  sockaddr_in myaddr;
    size_t newLen;
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

    ofstream outfile("peers.txt");
    outfile << ip; 
    outfile.close();



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
    
    char *source = NULL;
    FILE *fp = fopen(fileName.c_str(), "rb");
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



    int i=0,j=0,offset = OFFSET;
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