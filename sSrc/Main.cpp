#include "Packets.h"
void clientThread(SOCKET* acceptSock, SOCKET** allSocks, Hand hand, Deck* deck, int index){
    SOCKADDR_IN clientInfo = {0};
    SOCKADDR_IN allInfo[4];
    int phase = 0;
    int addrsize = sizeof(clientInfo);
    std::cout << "1\n";
    hand.setDeck(deck);

    std::cout << std::format("Player {} starting Deck: ", index) << hand;
    getpeername(*acceptSock, reinterpret_cast<SOCKADDR*>(&clientInfo), &addrsize);
    for(int i = 0; i < 4; i++){
        initPacket pack0;
        pack0.index = index;
  

        if(allSocks[i] == nullptr){
            continue;
        }
        getpeername(*allSocks[i], reinterpret_cast<SOCKADDR*>(&allInfo[i]), &addrsize);
    }
    int recvCount;
    while (true) {
        if ((recvCount = recv(*acceptSock, (char *)&phase, sizeof(int), 0)) != SOCKET_ERROR) {
            std::cout << "recieved " << phase;
            switch (phase) {
            case 0: { 
                packet1 pack;
                if ((recvCount = recvfrom(*acceptSock, (char *)&pack, sizeof(packet1), 0, reinterpret_cast<SOCKADDR *>(&clientInfo), &addrsize)) != SOCKET_ERROR) {
                    std::cout << "22222\n";
                    for (int i = 0; i < 4; i++) {
                        if (i == index || allSocks[i] == nullptr) {
                            continue;
                        }
                        sendto(*allSocks[i], (char *)&pack, sizeof(packet1), 0, reinterpret_cast<SOCKADDR *>(&allInfo[i]), sizeof(allInfo[i]));
                    }
                    break;

                }
            }
            break;
            
            case 1: {
                packet2 pack;
                auto& discarded = pack.discarded;
                if((recvCount = recv(*acceptSock, (char*)&pack, sizeof(packet2), 0) != SOCKET_ERROR)){
                    hand.setDiscarded(discarded);
                    hand.discardCards();
                }
                for(int i = 0; i < 5; i++){
                    pack.cards[i].first = hand.at(i).getNumber();
                    pack.cards[i].second = hand.at(i).getSuite();
                }
                send(*acceptSock, (char*)&pack, sizeof(packet2), 0);
                
            }
            break;

            default:

                break;
            }
        }
        else{
            break;
        }
    }
    std::cout << std::format("{}:{} disconnected\n\n", inet_ntoa(clientInfo.sin_addr), clientInfo.sin_port);
    closesocket(*acceptSock);
    delete acceptSock;
    acceptSock = nullptr;
    allSocks[index] = nullptr;
}

int main(){
    srand(time(NULL));
    WSAData data;
    WORD version = MAKEWORD(2,2);
    Deck* deck = new Deck;
    Hand* hand = new Hand[4];

    initPacket initPacket;

    for(int i = 0; i < 4; i++){
        hand[i].setDeck(deck);
        for(int j = 0; j < 5; j++){
            initPacket.cards[i][j].first = hand[i][j].getNumber();
            initPacket.cards[i][j].second = hand[i][j].getSuite();
        }
    }
    int wsaerr = WSAStartup(version, &data);
    const int MAX_CLIENTS = 4;
    const unsigned short PORT = 1234;
    
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
    SOCKET *allSocks[MAX_CLIENTS] = {nullptr, nullptr, nullptr, nullptr};
    int clientSize = 0;
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
            client[i] = std::thread(clientThread, acceptSock, allSocks, hand[i], deck, i);
            initPacket.index = i;
            sendto(*acceptSock, (char*)&initPacket, sizeof(initPacket), 0, reinterpret_cast<SOCKADDR *>(&clientInfo), sizeof(clientInfo));
        }
        clientSize++;
    }
    client[0].join();
    delete[] deck;
    delete[] hand;
    delete[] client;
    WSACleanup();
}