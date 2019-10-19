
#include <bits/stdc++.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "os.h"
using namespace std;


 


string dirStructure= "";


void getDirectoryStructure(string dir, string prefix, vector<string>& ignore) {
    string filepath;
    DIR *dp;
    struct dirent *dirp;
    struct stat filestat;

    dp = opendir(dir.c_str());

    if (dp == NULL) {
        cout << "Error " << errno << " while trying opening" << dir << endl;
        return;
    } 

    while (dirp = readdir(dp)) {

        #ifdef OS_Windows
            filepath = dir + "\\" + dirp->d_name;
        #else
            filepath = dir + "/" + dirp->d_name;
        #endif   
        

        string nameString(dirp->d_name);

        if(std::find(ignore.begin(), ignore.end(), nameString) != ignore.end() || 
            std::find(ignore.begin(), ignore.end(), filepath) != ignore.end()) {
            continue;
        } else {
            string tmpprefix = prefix;
            tmpprefix.replace(tmpprefix.size()-3, 3, "├─");
            dirStructure += tmpprefix + dirp->d_name + "\n";
        }

        if (stat(filepath.c_str(), &filestat)) {
            continue;
        }

        if (S_ISDIR(filestat.st_mode)) {
            getDirectoryStructure(filepath, prefix + "   │", ignore);
        }

    }
    closedir(dp);
}

vector<string> buildIgnoreVector() {
    vector<string> ignore;
    /* Ignore the following file and folder names */
    ignore.push_back(".");
    ignore.push_back("..");
    ignore.push_back("node_modules");
    ignore.push_back("bower_components");
    ignore.push_back(".git");
    ignore.push_back(".vscode");
    return ignore;
}
