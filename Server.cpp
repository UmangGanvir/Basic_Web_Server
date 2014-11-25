#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
// --
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
// --
#include <fstream>
// --

using namespace std;

#define MAXLEN 4096
#define PORT 1111

string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

int main(){

    int listen_fd, connfd;
    socklen_t length;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLEN + 1];

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Socket opening error\n");
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0){
        printf("binding error\n");
        return 0;
    }

    if(listen(listen_fd, 1024) < 0){
        printf("listen queue full probably\n");
        return 0;
    }

    length = sizeof(cliaddr);
    if((connfd = accept(listen_fd, (struct sockaddr*) &cliaddr, &length)) < 0){
        printf("accept error\n");
        return 0;
    }

    int n, res_ctr = 0;
    while(n=read(connfd, buffer, MAXLEN)){

        // Request Handling in a child process
        res_ctr++;
        if(!fork()){
            printf("\nReceived request : %d\n\n%s", res_ctr, buffer);
            //request.write(buffer, 1024);    // Only writes when size is >= 1024
            int bufferIndex = 0;
            string path = "";
            while(buffer[bufferIndex] != 32){
                bufferIndex++;
            }
            bufferIndex++;
            while(buffer[bufferIndex] != 32){
                path += buffer[bufferIndex];
                bufferIndex++;
            }

            string request = (string)buffer;
            //cout<<"  --  "<<request<<"  --  "<<endl;
            string parameters="";
            for(int i=request.length()-1; request[i] != 10; i--){
                parameters += request[i];
            }
            for(int i=0; i<parameters.length()/2; i++){
                char a = parameters[i];
                parameters[i] = parameters[parameters.length()-1-i];
                parameters[parameters.length()-1-i] = a;
            }
            //cout<<"Parameters = "<<parameters<<endl;

            /*
            while(buffer[bufferIndex] != 10 && buffer[bufferIndex + 1] != 10 && buffer[bufferIndex+2] != 10){
                cout<<"Reaching"<<endl;
                cout<<request[bufferIndex]<<endl;
                bufferIndex ++;
            }
            */

            string filename;
            if(path[path.length()-1] == 47){
                filename = "index.html";
            } else {
                int fnctr = 0;
                for(int i=0; i<path.length(); i++){
                    if(path[i] == 47)
                        fnctr = i;
                }
                filename = path.substr( fnctr+1, path.length() - (fnctr + 1) );
                path = path.substr(0, path.length() - filename.length());
            }

            //cout<<"Path = "<<path<<endl;
            //cout<<"Filename = "<<filename<<endl;


            // Response Handling
            string response = "";

            struct dirent *dirStruct=NULL;
            DIR *dirHandle=NULL;
            string directory = "." + path;
            //cout<<"Dir = "<<directory<<endl;
            dirHandle=opendir(directory.c_str());
            struct stat fileStat;

            bool found = false;
            if(dirHandle != NULL){
                while(dirStruct = readdir(dirHandle)){
                    stat(dirStruct->d_name,&fileStat);
                    if(strcmp(dirStruct->d_name, filename.c_str()) == 0){
                        found = true;
                        //cout<<dirStruct->d_name<<" - "<<filename<<endl;
                        if((fileStat.st_mode & S_IFMT) == S_IFDIR){
                            // If found resource is a directory
                            path += filename + "/";
                            filename = "index.html";
                        }
                    }
                }
            } else {
                //cout<<"Couldn't open the directory"<<endl;
            }
            string status_line = "";
            string headers = "";
            string content = "";
            if(found){
                ifstream file;
                string completePath = "." + path + filename;
                //cout<<"Complete Path = "<<completePath<<endl;
                string type = "";
                size_t found = filename.find(".");
                if (found!=std::string::npos){
                    type = filename.substr(found+1);
                    if(type == "html"){
                        file.open(completePath.c_str());
                        string line;
                        if(file.is_open()){
                            while(getline(file, line)){
                                content += line;
                            }
                            file.close();
                        } else {
                            cout<<"File is closed."<<endl;
                        }
                        headers += "Content-Type: text/html\r\n";
                    } else if(type == "jpg" || type == "jpeg" || type == "png"){
                        cout<<"Not Handling images"<<endl;
                        content = "Not Handling images";
                        headers += "Content-Type: text/plain\r\n";
                    } else if(type == "php"){
                        //content = execl("/bin/ls", "a=$(ls al)", " && echo $a", (char *)0);
                        string command = "php "+ completePath;
                        if(parameters.length() > 0){
                            command += (" '" + parameters + "'");
                        }
                        content = exec((char*)command.c_str());
                        headers += "Content-Type: text/html\r\n";
                    } else {
                        cout<<"Not Handling this format"<<endl;
                        content = "Not Handling this format";
                    }
                }

                stringstream ss;
                ss << content.length();

                status_line = "HTTP/1.1 200 OK\r\n";
                headers += "Content-Length: " + ss.str() + "\r\n";
            } else {
                //cout<<"File Not Found"<<endl;
                // Send a 404 Error - Resource Not Found!
                status_line = "HTTP/1.1 404 Not Found\r\n";
                headers += "Content-Length: 0\r\n";
                content = "Error 404 : File not found!";
            }

            response += status_line;
            response += headers;
            response += "\n";
            response += content;

            cout<<endl<<endl<<"Response : "<<res_ctr<<endl<<endl;
            cout<<response<<endl;


            write(connfd, response.c_str(), response.length());
            exit(0);
        }

    }


    if(close(connfd) < 0){
        printf("close error\n");
        return 0;
    }

    return 0;
}
