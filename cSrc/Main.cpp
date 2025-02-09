#include "Game.h"

/*The new plan is to use the server to store the "real" Deck / playerHands (AND ONLY THE DECK AND PLAYERHANDS)*/
int main(){

    WSADATA wsaData;
    int wsaerr;
    WORD version = MAKEWORD(2,2);

    wsaerr = WSAStartup(version, &wsaData);
    if(wsaerr != 0){
        std::cout << "Winsock dll not found.\n";
        std::cin.get();
        return -1;
    }
    else{
        std::cout << "Winsock dll found.\n";
        std::cout << "System status: " << wsaData.szSystemStatus << "\n";
    }

    //Socket Initialization
    SOCKET clientSock = INVALID_SOCKET;
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(clientSock == INVALID_SOCKET){
        std::cout << "Error at Socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
    }
    else{
        std::cout << "Client Socket initialized\n";
    }

    //Connection
    sockaddr_in clientServ;
    u_short port = 1234;

    clientServ.sin_family = AF_INET;
    clientServ.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientServ.sin_port = htons(port);

    if(connect(clientSock, reinterpret_cast<SOCKADDR*>(&clientServ), sizeof(clientServ)) == SOCKET_ERROR){
        std::cout << "Client failed to connect: " << WSAGetLastError();
        std::cin.get();
        WSACleanup();
        return -1;
    }
    else {
        std::cout << "Client connected to " << inet_ntoa(clientServ.sin_addr) << "\n\n";
    }
    RenderWindow window(VideoMode(), "Multiplayer Poker", Style::Fullscreen);
    Poker::PokerGame* game = new Poker::PokerGame(window);


    try{
        game->init(window, &clientSock, clientServ);
        game->update(window, &clientSock);
    }
    catch(std::exception &e){
        Vector2f windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};

        sf::Text errorMsg;
        sf::Font font;
        font.loadFromFile(Game::FontPaths::blackLivesFont);
        errorMsg.setFont(font);
        std::string eString = e.what();
        window.clear();

        errorMsg.setCharacterSize(windowScale.x * 20.f );
        errorMsg.setFillColor(Color::Red);
        errorMsg.setPosition((Vector2f)window.getSize() / 2.f);
        errorMsg.setString(std::format("Error: {}", eString));

        window.draw(errorMsg);
        window.display();
        std::cerr << e.what();

        std::cin.get();
        delete game;
        closesocket(clientSock);
        WSACleanup();

        std::exit(EXIT_FAILURE);
    }

    delete game;
    closesocket(clientSock);
    WSACleanup();
}