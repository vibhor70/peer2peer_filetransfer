
#include "commons.h"
using namespace std;

bool recieveFile(char* saveAs, int sockfd){
    FILE* fp = fopen(saveAs, "wb");
    char* BUFFER;
    size_t recvLen;
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
        cout << received/BUFFERSIZE << "%" << endl;

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
    fclose(fp);
    cout << "Total size transferred: " << totalSize*(1.0)/1000 << "kB" << endl;  
    return true;
}

void showAvailablePeers(){
    std::ifstream input( "peers.txt" );
    cout << "Here are the peer list, please choose the peer IP:" << endl;
    int i = 1;
    for( std::string line; getline( input, line ); ){
        cout << i++ << ".) " << line << endl;
    }
}


int main(){
    int sockfd;
    struct  sockaddr_in myaddr;
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
    showAvailablePeers();
    cout << "Please enter the IP of the network: ";
    cin >> ip;
    
    cout << "Waiting for the directory structure...";
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = inet_addr(ip.c_str());

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

    // while(true){
        char dirPath[1000];
        cout << "Choose your path of the file: ";
        cin >> dirPath;

        char saveAs[1000];
        cout << "Saving the file with name as: ";
        cin >> saveAs;
        if( send(sockfd, dirPath, 1000, 0) < 0 ){
            perror("Send to failed");
            return 0;
        }

        recieveFile(saveAs, sockfd);

        // cout << endl << endl;
        // cout << "Do you want to download more file from same peer (Y/N)?: ";
        // string choice;
        // cin >> choice;
        // if(choice == "Y" || choice == "y"){
        //     char msg[10] = "Y";
        //     if( send(sockfd, msg, 10, 0) < 0 ){
        //         perror("Send to failed");
        //         return 0;
        //     }
        // }
        // else
        //     break;
    // }

    close(sockfd);
    return 0;
}