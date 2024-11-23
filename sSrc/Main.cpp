#include "Game.h"


int main(){
    WSAData data;
    WORD version = MAKEWORD(2,2);
    int wsaerr = WSAStartup(version, &data);
    const int MAX_CLIENTS = 4;
    const unsigned short PORT = 55555;

    if(wsaerr != 0){
        std::cout << "ERROR: Winsock dll not found\n";
        std::cin.get();
        return -1;
    }
    else{
        std::cout << "Winsock dll found.\n";
        std::cout << "System status: " << data.szSystemStatus << "\n";
    }

    SOCKET serverSock = INVALID_SOCKET;
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSock == INVALID_SOCKET) {
        std::cout << "Socket Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
    }
    else {
        std::cout << "Server Socket initialized\n";
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(PORT);
    if(bind(serverSock, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR){
        std::cout << "Server bind failed: " << WSAGetLastError() << "\n";
        std::cin.get();

        WSACleanup();
        return -1;
    }
    else{
        std::cout << "Server binding successful.\n";

    }

    if (listen(serverSock, MAX_CLIENTS) == SOCKET_ERROR) {
        std::cout << "Server Error: " << WSAGetLastError();
        WSACleanup();
        std::cin.get();
        return -1;
    }
    else {
        std::cout << "Waiting for connections. \n";
    }

    SOCKADDR_IN clientInfo = {0};
    int addrsize = sizeof(clientInfo);
    std::thread *client = new std::thread[MAX_CLIENTS];
    SOCKET *allSocks[MAX_CLIENTS];

    for(int i = 0; i < MAX_CLIENTS; i++){
                SOCKET *acceptSock = new SOCKET(accept(serverSock, reinterpret_cast<SOCKADDR *>(&clientInfo), &addrsize));
                allSocks[i] = acceptSock;
                if (*acceptSock == INVALID_SOCKET) {
                    std::cout << "Server Failure: " << WSAGetLastError() << "\n";
                    std::cin.get();
                    WSACleanup();
                    return -1;
                }
                else {
                    std::cout << std::format("Connection established with {}:{}\n\n",
                                             inet_ntoa(clientInfo.sin_addr),
                                             clientInfo.sin_port);
                    client[i];
                }    
    }
}