#include "Packets.h"

void clientThread(SOCKET* acceptSock, SOCKET** allSocks, Hand* hand, Deck* deck, int index){
    using namespace std::chrono_literals;
    //std::this_thread::sleep_for(20s);
    SOCKADDR_IN clientInfo = {0};
    SOCKADDR_IN allInfo[4];
    int phase = 0;
    int addrsize = sizeof(clientInfo);

    std::cout << std::format("Player {} starting Deck: ", index) << hand;
    getpeername(*acceptSock, reinterpret_cast<SOCKADDR*>(&clientInfo), &addrsize);
    for(int i = 0; i < 4; i++){
        initPacket pack0;
        pack0.index = index;
  

        if(allSocks[i] == nullptr){
            continue;
        }
        getpeername(*allSocks[i], reinterpret_cast<SOCKADDR*>(&(allInfo[i])), &addrsize);
    }
    int recvCount;
    
    auto bet = [&]() {
        
        packet1 pack;
        if ((recvCount = recv(*acceptSock, (char *)&pack, sizeof(packet1), 0)) != SOCKET_ERROR && recvCount == sizeof(packet1)) {
            std::cout << std::format(
                "Recieved Packet1:\n"
                "Player: {}\n"
                "Raise Amount: {}\n\n",
                index, pack.raiseAmount);

            for (int i = 0; i < 4; i++) {
                if (allSocks[i] == nullptr) {
                    continue;
                }
                sendto(*(allSocks[i]), (char *)&pack, sizeof(packet1), 0, reinterpret_cast<SOCKADDR *>(&(allInfo[i])), sizeof(allInfo[i]));
            }
        }
    };
    while (true) {
        if ((recvCount = recv(*acceptSock, (char *)&phase, sizeof(int), 0)) != SOCKET_ERROR && recvCount == sizeof(int)) {
            std::cout << std::format("Recieved Phase: {}\n", phase);
            switch (phase) {
            case 0:
                bet();
            break;
            
            case 1: {
                packet2 pack;

                if((recvCount = recv(*acceptSock, (char*)&pack, sizeof(packet2), 0)) != SOCKET_ERROR){
                    std::cout << std::format("Packet Size: {}\n", recvCount);
                    std::cout << std::format("Index: {}\n", pack.index);
                    std::cout << std::format("DiscardNum: {}\n", pack.discardNum);
                    std::vector<int> d;
                    for(int i = 0; i < pack.discardNum; i++){
                        d.push_back(pack.discarded[i]);
                    }
                    hand[pack.index].setDiscarded(d);

                    hand[pack.index].discardCards();
                    for(int i = 0; i < pack.discardNum; i++){
                        std::cout << pack.discarded[i];
                    }
                
                    for(int i = 0; i < 5; i++){
                        pack.cards[i].first = hand[pack.index].at(i).getNumber();
                        pack.cards[i].second = hand[pack.index].at(i).getSuite();
                    }
                    for (int i = 0; i < 4; i++) {
                        if (allSocks[i] == nullptr) {
                            continue;
                        }
                        sendto(*(allSocks[i]), (char *)&pack, sizeof(packet2), 0, reinterpret_cast<SOCKADDR *>(&(allInfo[i])), sizeof(allInfo[i]));
                    }              
                }

            }
            break;

            case 2:
                bet();
                break;
            case 3:{
                packet3 pack;
                deck->reset();
                for(int i = 0; i < 4; i++){
                    hand[i].setDeck(deck);
                    std::cout << "Setdeck\n";
                    for(int j = 0; j < 5; j++){
                        pack.cards[i][j].first = hand[i][j].getNumber();
                        pack.cards[i][j].second = hand[i][j].getSuite();
                    }
                }
                for (int i = 0; i < 4; i++) {
                    if (allSocks[i] == nullptr) {
                        continue;
                    }
                    sendto(*(allSocks[i]), (char *)&pack, sizeof(packet3), 0, reinterpret_cast<SOCKADDR *>(&(allInfo[i])), sizeof(allInfo[i]));
                }
                break;
            }

            default:

                break;
            }
        }
        else{
            std::cout << "No\n";
            std::cin.get();
            break;
        }
    }
    std::cout << "No\n";
    std::cin.get();
    std::cout << std::format("{}:{} disconnected\n\n", inet_ntoa(clientInfo.sin_addr), clientInfo.sin_port);
    std::cout << "Socket Error: " << WSAGetLastError() << std::endl;

    closesocket(*acceptSock);
    delete acceptSock;
    acceptSock = nullptr;
    allSocks[index] = nullptr;
    WSACleanup();   // NOTE: Depending on how this works, it's a possible memory leak.
}

int initThread(SOCKET* acceptSock, sockaddr_in* info, Time* initTime, Clock* initClock, initPacket* pack, int index){

    while(true){
        if(initClock->getElapsedTime() > *initTime){
            initPacket playerPack = *pack;
            playerPack.index = index;
            sendto(*acceptSock, (char*)&playerPack, sizeof(initPacket), 0, reinterpret_cast<SOCKADDR*>(info), sizeof(*info));
            return 0;
        }
    }
}

int  main(){
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
    SOCKADDR_IN checkInfo = {0};

    int addrsize = sizeof(clientInfo);
    std::thread *client = new std::thread[MAX_CLIENTS];
    SOCKET *allSocks[MAX_CLIENTS] = {nullptr, nullptr, nullptr, nullptr};
    SOCKADDR_IN allInfo[MAX_CLIENTS];
    int clientSize = 0;

    std::vector<int> clientIndexes;
    std::thread* init = new std::thread[MAX_CLIENTS];
    Clock initClock;
    Time initTime = seconds(15);

    for(int i = 0; i < MAX_CLIENTS; i++){
        SOCKET *acceptSock = new SOCKET(accept(serverSock, reinterpret_cast<SOCKADDR *>(&clientInfo), &addrsize));
        allInfo[i] = clientInfo;
        if (*acceptSock == INVALID_SOCKET) {
            std::cout << "Socket Time Out: " << WSAGetLastError() << "\n";
            std::cin.get();
            break;
        }
        else {
            allSocks[i] = acceptSock;

            std::cout << std::format("Connection established with {}:{}\n\n",
                                     inet_ntoa(clientInfo.sin_addr),
                                     clientInfo.sin_port);
            clientSize++;
            client[i] = std::thread(clientThread, acceptSock, allSocks, hand, deck, i);
            clientIndexes.push_back(i);
            initPacket.index = i;
           // sendto(*allSocks[i], (char*)&initPacket, sizeof(initPacket), 0, reinterpret_cast<SOCKADDR *>(&clientInfo), sizeof(clientInfo));
            initPacket.playerNum = clientSize;
            init[i] = std::thread(initThread, acceptSock, &(allInfo[i]), &initTime, &initClock, &initPacket, initPacket.index);
            std::cout << std::format("Client Size = {}\n", clientSize);
        }
    }

    client[0].join();
    delete[] deck;
    delete[] hand;
    delete[] client;
    std::cin.get();
    WSACleanup();
}