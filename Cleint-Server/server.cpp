#include <iostream>
#include <fstream>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int server;
bool running = true;

void signal_handler(int signal) {
    if (signal == SIGTERM || signal == SIGHUP) {
        std::cout << "Received signal to terminate. Closing server..." << std::endl;
        running = false;
        close(server);
    } 
}

void daemonized() {
    pid_t pid = fork();

    if(pid < 0) {
        std::cerr << "Fork Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(pid > 0) {
        std::cout << "Daemon created with PID"<< pid << std::endl;
        exit(EXIT_SUCCESS);
    }

    if(setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    freopen("dev/null", "r", stdin);
    freopen("dev/null", "w", stdout);
    freopen("dev/null", "2", stderr);
}

int main (){

    daemonized();

    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    
    int client;
    int portnum = 1500;
    int bufSize = 1024;
    char buffer[bufSize];

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        std::cerr << "Error establishing socket..." << std::endl;
        exit(1);
    }

    std::cout << "Server Socket connection created..." << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portnum);

    //Bind

    if(bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket..." << std::endl;
        exit(1);
    }

    size = sizeof(client_addr);
    std::cout << "Looking for clients..." << std::endl;

    //listen
    listen(server, 5);

    while(running){
        client = accept(server, (struct sockaddr*)&client_addr, &size);
        if(client < 0) {
            std::cerr << "Error on accecpting..." << std::endl;
            continue;
        }
    

    std::ofstream outputFile("received_file.txt", std::ios::binary);
    if(!outputFile){
         std::cerr << "Error opening file for writing..." << std::endl;
         close(client);
    }

    int bytesReceived;
    while((bytesReceived = recv(client, buffer, bufSize, 0)) > 0){
        outputFile.write(buffer, bytesReceived);
    }

    std::cout << "File received and saved!" << std::endl;
    outputFile.close();
    close(client);
    }

    close(server);
    return 0;
}