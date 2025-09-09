#include "Game.h"
#include <filesystem>

#ifdef __unix__
int closesocket(int socket){
    return close(socket);
}
int main(int argc, char** argv){

#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow){
    WSADATA wsaData;
    int wsaerr;
    WORD version = MAKEWORD(2,2);

    wsaerr = WSAStartup(version, &wsaData);
    if(wsaerr != 0){
        WSACleanup();

        throw std::runtime_error("Winsock not found...");
        return -1;
    }
#endif

    srand(time(NULL));


    //Socket Initialization
    SOCKET clientSock = INVALID_SOCKET;
    clientSock = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSock == INVALID_SOCKET){
        #ifdef _WIN32
        std::string errmsg = "Error at socket " + WSAGetLastError();
        WSACleanup();
        #endif
        #ifdef __unix__
        std::string errmsg = "Error at socket " + errno;
        #endif
        throw std::runtime_error(errmsg);
    }

    //Connection
    sockaddr_in clientServ;
    u_short port = 1234;

    clientServ.sin_family = AF_INET;
    clientServ.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientServ.sin_port = htons(port);
    if(connect(clientSock, reinterpret_cast<SOCKADDR*>(&clientServ), sizeof(clientServ)) == SOCKET_ERROR){
        #ifdef _WIN32

        std::string errmsg = "Client failed to connect: " + WSAGetLastError();
        WSACleanup();

        #endif

        #ifdef __unix__
        std::string errmsg = "Client failed to connect: " + std::to_string(errno);

        #endif

        throw std::runtime_error(errmsg);
        return -1;
    }
    RenderWindow window(VideoMode(), "Multiplayer Poker", State::Fullscreen);
    Poker::PokerGame* game = new Poker::PokerGame(window);


    try{
        game->init(window, &clientSock, clientServ);
        game->update(window, &clientSock);
    }
    catch(std::exception &e){
        Vector2f windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
        sf::Font font;
        sf::Text errorMsg(font);
        Game::loadFont(errorMsg, font, Game::FontPaths::blackLivesFont);

        std::string eString = e.what();
        window.clear();

        errorMsg.setCharacterSize(windowScale.x * 20.f );
        errorMsg.setFillColor(Color::Red);
        errorMsg.setPosition((Vector2f)window.getSize() / 2.f);
        errorMsg.setString(std::format("Error: {}.", eString));

        window.draw(errorMsg);
        window.display();
        #ifdef _WIN32
        std::cout << WSAGetLastError();
        #endif

        #ifdef __unix__
        std::cout << errno;
        #endif

        std::cin.get();
        delete game;
        closesocket(clientSock);

        #ifdef _WIN32
        WSACleanup();
        #endif
        std::exit(EXIT_FAILURE);
    }

    delete game;
    closesocket(clientSock);
    #ifdef _WIN32
    WSACleanup();
    #endif
    return 0;
}