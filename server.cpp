#include "commons.h"

using namespace std;

string handleNewPeer(char* ipAddr){
    string line;
    ofstream outfile("peers.txt");
    if(outfile.fail()){
        exit(-1);
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
    string mySharedDir = "";
    DIR *dp;
    cout << "Please enter your shared directory path: ";
    cin >> mySharedDir;

    while(!mySharedDir.length()){
        cout << "You have not entered the path" << endl;
        cout << "Please enter your shared directory path: ";
        cin >> mySharedDir;
        if(mySharedDir.length() > 0){
            dp = opendir(mySharedDir.c_str());
            if (dp == NULL) {
                cout << "Error " << errno << " while trying opening" << mySharedDir << endl;
            }
            else
                break;
        }
    }

    return mySharedDir;
}

void dieError(const char* msg){
    perror(msg);
    exit(-1);
}



bool sendFile(string fullPath, int clisock, int lsn){
    char *source = NULL;
    size_t newLen;
    FILE *fp = fopen(fullPath.c_str(), "rb");

    if (fp < 0) {
        /* failure */
        if (errno == EEXIST) {
            cout << "Error " << errno << " while trying accessing " << fullPath << endl;
            return false;                
        }
    }
    else if(fp == NULL){
        cout << "fp=Null Error " << errno << " while trying accessing " << fullPath << endl;
        dieError("Error in opening file");
    }
    else {
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
        fclose(fp);
    }

    int i=0,j=0,offset = OFFSET;
    int no = newLen/offset;
    int remain = newLen%offset;
    while(i<no){
        if(send(clisock,(source+j),offset,0)<0){
            perror("Send to failed");
            return false;
        }
        j+=offset;
        i++;
    }
    if(send(clisock,source+j,remain,0)<0){
        perror("Send to failed");
        return 0;
    }
    printf("Total size of the file transferred: %0.2f kB \n", newLen*1.0/1000);

    return true;
}


int main(){
    size_t recvLen;
    char* BUFFER;
    int sockfd;

    struct sockaddr_in serv_addr,cli_addr;
    if((sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
        dieError("Server socket not created");
    } 
    memset (&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
        dieError("Bind Failed");
    }
    int lsn = listen(sockfd,5);
    if(lsn<0){
        dieError("Error in listening");
    }

    int pid=0;
    string mySharedDir = "";
    mySharedDir = getSharedDir(); 
    // mySharedDir = "/home/jai/Desktop/projects/peer2peer_filetransfer";
    
    cout << "Listening from clients..." << endl;
    while(1){
        socklen_t clilength = sizeof(cli_addr);
        int clisock = accept(sockfd,(struct sockaddr*)&cli_addr,&(clilength));
        char *ip = inet_ntoa(cli_addr.sin_addr);
        handleNewPeer(ip);
        printf("New client with IP address: %s is connected\n", ip);

        if(clisock < 0){
            printf("Problem in server client socket %s\n",inet_ntoa(cli_addr.sin_addr));
        }

        if ((pid = fork()) == -1){
            close(clisock);
            continue;
        }
        else if(pid == 0){ //child
            // dirStr == global var
            vector<string> ignore = buildIgnoreVector();
            getDirectoryStructure(mySharedDir, "│", ignore);

            if(send(clisock,dirStructure.c_str(),dirStructure.length(), 0) < 0){
                perror("Send to failed");
                return 0;
            }

            // receive the file path     
            BUFFER = new char[2000];       
            recv(clisock, BUFFER, 2000, 0);  

            string sBuffer(BUFFER);
            delete BUFFER;
            string fullPath;
            if(mySharedDir.back() != '/' && mySharedDir.back() != '\\'){
                #ifdef OS_Windows
                    fullPath = mySharedDir + "\\" + sBuffer; 
                #else
                    fullPath = mySharedDir + "/" + sBuffer; 
                #endif   
            }
            else{
                fullPath = mySharedDir + sBuffer; 
            }
            cout << "Client wants to download path: " << fullPath << endl;


            string buffer;
            bool isLastPacket;
            int bufsize;
            // while(true){
            bool res = sendFile(fullPath, clisock, lsn);
            if(!res){
                dieError("Error in sending the file");
            }
                // BUFFER = new char[10];
                // cout << "Test" << endl;     
                // recv(clisock, BUFFER, 10, 0); 
                // if(strcmp(BUFFER, "y") == 0 || strcmp(BUFFER, "Y") == 0)
                //     cout << "Client wants to download new file again" << endl;
                // else
                //     break;
            // }


            close(clisock);
            close(lsn);
        }

        else if(pid > 0) { //parent
            // receive a ip from new client
            close(clisock);
            continue;
        }

    } // end of while
    
    return 0;
}