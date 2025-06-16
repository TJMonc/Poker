#include "Packets.h"

void clientThread(int* acceptSock, int** allSocks, Hand* hand, Deck* deck, int index){
    using namespace std::chrono_literals;
    //std::this_thread::sleep_for(20s);
    
    sockaddr_in clientInfo = {0};
    sockaddr_in allInfo[4];
    int phase = 0;
    unsigned int addrsize = sizeof(clientInfo);

    std::cout << std::format("Player {} starting Deck: ", index) << hand;
    getpeername(*acceptSock, reinterpret_cast<sockaddr*>(&clientInfo), &addrsize);
    for(int i = 0; i < 4; i++){
        initPacket pack0;
        pack0.index = index;
  

        if(allSocks[i] == nullptr){
            continue;
        }
        getpeername(*allSocks[i], reinterpret_cast<sockaddr*>(&(allInfo[i])), &addrsize);
    }
    int recvCount;
    
    auto bet = [&]() {
        
        packet1 pack;
        if ((recvCount = recv(*acceptSock, (char *)&pack, sizeof(packet1), 0)) != -1 && recvCount == sizeof(packet1)) {
            std::cout << std::format(
                "Recieved Packet1:\n"
                "Player: {}\n"
                "Raise Amount: {}\n\n",
                index, pack.raiseAmount);

            for (int i = 0; i < 4; i++) {
                if (allSocks[i] == nullptr) {
                    continue;
                }
                sendto(*(allSocks[i]), (char *)&pack, sizeof(packet1), 0, reinterpret_cast<sockaddr *>(&(allInfo[i])), sizeof(allInfo[i]));
            }
        }
    };
    while (true) {
        if ((recvCount = recv(*acceptSock, (char *)&phase, sizeof(int), 0)) != -1 && recvCount == sizeof(int)) {
            std::cout << std::format("Recieved Phase: {}\n", phase);
            switch (phase) {
            case 0:
                bet();
            break;
            
            case 1: {
                packet2 pack;

                if((recvCount = recv(*acceptSock, (char*)&pack, sizeof(packet2), 0)) != -1){
                    std::cout << std::format("Packet Size: {}\n", recvCount);
                    std::cout << std::format("Index: {}\n", pack.index);
                    std::cout << std::format("DiscardNum: {}\n", pack.discardNum);
                    if(!pack.folded){
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
                    }
                    for (int i = 0; i < 4; i++) {
                        if (allSocks[i] == nullptr) {
                            continue;
                        }
                        sendto(*(allSocks[i]), (char *)&pack, sizeof(packet2), 0, reinterpret_cast<sockaddr *>(&(allInfo[i])), sizeof(allInfo[i]));
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
                    sendto(*(allSocks[i]), (char *)&pack, sizeof(packet3), 0, reinterpret_cast<sockaddr *>(&(allInfo[i])), sizeof(allInfo[i]));
                }
                break;
            }

            default:

                break;
            }
        }
        else{
            std::cout << "No\n";
            break;
        }
    }

    std::cout << std::format("{}:{} disconnected\n\n", inet_ntoa(clientInfo.sin_addr), clientInfo.sin_port);
    std::cout << "Socket Error: " << errno << std::endl;

    close(*acceptSock);
    delete acceptSock;
    acceptSock = nullptr;
    allSocks[index] = nullptr;
  //  exit(-1);   // NOTE: Depending on how this works, it's a possible memory leak.
}

int initThread(int* acceptSock, sockaddr_in* info, Time* initTime, Clock* initClock, initPacket* pack, int index){

    if(index == 0){
        initClock->restart();
    }
    while(true){
        if(initClock->getElapsedTime() > *initTime){
            initPacket playerPack = *pack;
            playerPack.index = index;
            sendto(*acceptSock, (char*)&playerPack, sizeof(initPacket), 0, reinterpret_cast<sockaddr*>(info), sizeof(*info));
            
            return 0;
        }
    }
}

int main(){
    srand(time(NULL));

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
    const int MAX_CLIENTS = 4;
    const unsigned short PORT = 1234;
    

    int serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSock == -1) {
        std::cout << "Socket Error: " << errno << std::endl;
        exit(-1);
    }
    else {
        std::cout << "Server Socket initialized\n";
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(PORT);


    fchmod(serverSock, S_IRWXU | S_IRWXG | S_IRWXO);

    if(bind(serverSock, reinterpret_cast<sockaddr*>(&service), sizeof(service)) == -1){
        std::cout << "Server bind failed: " << errno << "\n";
        std::cin.get();

        exit(-1);
        return -1;
    }
    else{
        std::cout << "Server binding successful.\n";

    }

    if (listen(serverSock, MAX_CLIENTS) == -1) {
        std::cout << "Server Error: " << errno;
        exit(-1);
        return -1;
    }
    else {
        std::cout << "Waiting for connections. \n";
    }

    sockaddr_in clientInfo = {0};
    sockaddr_in checkInfo = {0};

    unsigned int addrsize = sizeof(clientInfo);
    std::thread *client = new std::thread[MAX_CLIENTS];
    int *allSocks[MAX_CLIENTS] = {nullptr, nullptr, nullptr, nullptr};
    sockaddr_in allInfo[MAX_CLIENTS];
    int clientSize = 0;

    std::vector<int> clientIndexes;
    std::thread* init = new std::thread[MAX_CLIENTS];
    Clock initClock;
    Time initTime = seconds(15);
    int index = 0;
    while(true){
        int *acceptSock = new int;
        *acceptSock = accept(serverSock, reinterpret_cast<sockaddr *>(&clientInfo), &addrsize);
        if((index != 0) && (initTime < initClock.getElapsedTime())){
            for(int i = 0; i < clientSize; i++){
                client[i].join();
            }
            index = 0;
            clientSize = 0;
            index = 0;
            deck->reset();
            for(int i = 0; i < 4; i++){
                hand[i].setDeck(deck);
                for(int j = 0; j < 5; j++){
                    initPacket.cards[i][j].first = hand[i][j].getNumber();
                    initPacket.cards[i][j].second = hand[i][j].getSuite();
                }
                allSocks[i] = nullptr;
            }
            init = new std::thread[MAX_CLIENTS];
            client = new std::thread[MAX_CLIENTS];
            std::cout << "End: " << index;
        }
        allInfo[index] = clientInfo;
        if (*acceptSock == -1) {
            std::cout << "Socket Time Out: " << errno << "\n";
            std::cin.get();
            break;
        }
        else {
            allSocks[index] = acceptSock;

            std::cout << std::format("Connection established with {}:{}\n\n",
                                     inet_ntoa(clientInfo.sin_addr),
                                     clientInfo.sin_port);
            clientSize++;
            client[index] = std::thread(clientThread, acceptSock, allSocks, hand, deck, index);
            initPacket.index = index;
           // sendto(*allSocks[i], (char*)&initPacket, sizeof(initPacket), 0, reinterpret_cast<sockaddr *>(&clientInfo), sizeof(clientInfo));
            initPacket.playerNum = clientSize;
            init[index] = std::thread(initThread, acceptSock, &(allInfo[index]), &initTime, &initClock, &initPacket, initPacket.index);
            std::cout << std::format("Client Size = {}\n", clientSize);
            index++;
        }
        if(clientSize > 3){
            for(int i = 0; i < clientSize; i++){
                client[i].join();
            }
            clientSize = 0;
            index = 0;
            deck->reset();
            for(int i = 0; i < 4; i++){
                hand[i].setDeck(deck);
                for(int j = 0; j < 5; j++){
                    initPacket.cards[i][j].first = hand[i][j].getNumber();
                    initPacket.cards[i][j].second = hand[i][j].getSuite();
                }
                allSocks[i] = nullptr;
            }
            init = new std::thread[MAX_CLIENTS];
            client = new std::thread[MAX_CLIENTS];
            std::cout << "End: " << index;
        }
    }

    delete deck;
    delete[] hand;
    delete[] client;
    delete[] init;
    std::cout << "ddd";
    exit(-1);
}