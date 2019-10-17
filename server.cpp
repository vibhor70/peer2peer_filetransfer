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
#include <fstream>
#include <sstream>
#include "pwd.cpp"


using namespace std;

#define PORT 8085
#define BUFFERSIZE 99999 


string handleNewPeer(char* ipAddr){
    string line;
    ofstream outfile("peers.txt");
    if(outfile.fail()){
        cout<<"file open fail"<<endl;
        exit(-1);
    } else{
        cout<<"file open success"<<endl;
    }
    
    ifstream infile("peers.txt");

    // check if the ip already exists
    bool isFound = false;
    string ipAddrString(ipAddr);

    string allIPadress;
    while (getline(infile, line)){
        if((line.compare(ipAddrString)) == 0){ //
            isFound = true;
        }
        else{
            allIPadress += line + '\n';
        }
    }
    if(!isFound){
        outfile << ipAddrString; 
    }
    return allIPadress;
}

string getSharedDir(){
    string mySharedDir;
    DIR *dp;
    cout << "Please enter your shared directory path: ";
    cin >> mySharedDir;
    dp = opendir(mySharedDir.c_str());
    if (dp == NULL) {
        cout << "Error " << errno << " while trying opening" << mySharedDir << endl;
        exit(-1);
    } 
    return mySharedDir;
}



int main(){
    size_t recvLen,newLen = 46258;
    char BUFFER[BUFFERSIZE];
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
        else if(pid == 0){ //child
            // recv(clisock, buffer, BUFFERSIZE, 0);  //ip address

            string mySharedDir = getSharedDir();
            vector<string> ignore = buildIgnoreVector();
            printDirectoryStructure(mySharedDir, "│", ignore);
            const char* dirStr = directoryStructure.c_str();

            if( send(clisock,directoryStructure.c_str(),directoryStructure.length(), 0) < 0 ){
                perror("Send to failed");
                return 0;
            }
            
            recv(sockfd, BUFFER, BUFFERSIZE, 0);  
            
            DIR *dp;
            string sBuffer(BUFFER);
            string fullPath = mySharedDir + sBuffer; 

            /* TODO
                CHECK FOR PROPER PATH
            */ 
            
            dp = opendir(fullPath.c_str());
            if (dp == NULL) {
                cout << "Error " << errno << " while trying opening" << fullPath << endl;
                exit(-1);
            }
            cout << "Path entered was: " << fullPath << endl;


            // CHECK THE PATH

            fp1 = fopen(fullPath.c_str(), "wb");
            int totalSize = 0;
            while(1){
                recvng= recv(clisock,BUFFER,BUFFERSIZE,0);
                totalSize += recvng;
                if(recvng < 0){
                    perror("Problem in recv");
                    break;
                }
                else if(recvng == 0){ 
                    recvLen = fwrite(BUFFER,sizeof(char),recvng,fp1);
                    break;
                }
                recvLen = fwrite(BUFFER,sizeof(char),recvng,fp1);
            }
            cout << "Total size" << totalSize << endl;  
            close(clisock);
            close(lsn);
        }

        else if(pid > 0) { //parent
            // recieve a ip from new client


            close(clisock);
            // counter++;
            continue;
        }

        printf("BUFFER from client %lu\n",recvLen);
    }
    
    fclose(fp1);
    return 0;
}