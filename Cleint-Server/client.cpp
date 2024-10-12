#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    
    std::string filename;
    std::cout << "Name file for send!" << std::endl;
    std::cin >> filename;

    std::fstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "File not found!" << std::endl;
        return 1;
    }

    int client;
    int portnum = 1500;
    int bufSize = 1024;
    char buffer[bufSize];
    char *ip = "127.0.0.1";

    struct sockaddr_in server_addr;

    // initializing socket
    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0){
        std::cerr << "Error creating socket..." << std::endl;
        exit(1);
    }

    std::cout << "Client Socket connection created..." << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // connect to server

    if(connect(client, (struct sockaddr*)& server_addr, sizeof(server_addr)) == 0){
        std::cout << "Connecting to server..." << std::endl;
    } else{
        std::cerr << "Connection failed!" << std::endl;
        exit(1);
    }

    // send file
    while(file.read(buffer, bufSize)) {
        send(client, buffer, bufSize, 0);
    }
    send(client, buffer, file.gcount(), 0); //Send if less than bufsize

    std::cout << "File sent successfully!" << std::endl;
    file.close();
    close(client);

    return 0;
}