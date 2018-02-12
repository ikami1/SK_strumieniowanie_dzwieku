#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <thread>
#include <algorithm>

#define SERVER_PORT 41111
#define SERVER_PORT_INPUT 41112
#define QUEUE_SIZE 5
#define BUFSIZE 1024
#define TITLE_SIZE 100


using namespace std;

/**************** TCP ****************/
void Sumuj(int* socket) {
    int nClientSocket = *socket;
    //std::cout<<"[connection from " << inet_ntoa((struct in_addr)stClientAddr.sin_addr)<<std::endl;
    std::cout<<"Nowe polaczenie"<<std::endl;


    filebuf *pbuf;
    ifstream sourcestr;
    long size;
    char * buffer;
    long ile_wyslane = 0;

    sourcestr.open("./Brooklynska_rada_zydow.wav", ios::in | ios::binary);
    if (!sourcestr.good()) {
        cout<<"Blad otwarcia pliku zrodlowego"<<endl;
        exit(EXIT_FAILURE);
    }

    // get pointer to associated buffer object
    pbuf=sourcestr.rdbuf();

    // get file size using buffer's members
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    // set seek position to 0
    pbuf->pubseekpos (0,ios::in);

    // allocate memory to contain file data
    buffer=new char[BUFSIZE];

    while(ile_wyslane < size/10){
        // set seek position to ile_wyslane
        pbuf->pubseekpos (ile_wyslane,ios::in);

        // get file data
        pbuf->sgetn (buffer,BUFSIZE);

        ile_wyslane += write(nClientSocket, buffer, BUFSIZE);

    }
    sourcestr.close();

    /*ile_wyslane = 0;

    sourcestr.open("./audio/Wynik.wav", ios::in | ios::binary);
    if (!sourcestr.good()) {
        cout<<"Blad otwarcia pliku zrodlowego"<<endl;
        exit(EXIT_FAILURE);
    }

    // get pointer to associated buffer object
    pbuf=sourcestr.rdbuf();

    // get file size using buffer's members
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    // set seek position to 0
    pbuf->pubseekpos (0,ios::in);

    // allocate memory to contain file data
    buffer=new char[BUFSIZE];

    while(ile_wyslane < size){
        // set seek position to ile_wyslane
        pbuf->pubseekpos (ile_wyslane,ios::in);

        // get file data
        pbuf->sgetn (buffer,BUFSIZE);

        ile_wyslane += write(nClientSocket, buffer, BUFSIZE);

    }
    sourcestr.close();*/

    close(nClientSocket);
}


// ##################### UDP #####################
/*void Sumuj(int* socket, sockaddr_in* stClientAddr) {
    int nClientSocket = *socket;
    sockaddr_in stClient = *stClientAddr;
    std::cout<<"[connection from " << inet_ntoa((struct in_addr)stClient.sin_addr)<<std::endl;


    filebuf *pbuf;
    ifstream sourcestr;
    long size;
    char * buffer;
    long ile_wyslane = 0;

    sourcestr.open("./Brooklynska_rada_zydow.wav", ios::in | ios::binary);
    if (!sourcestr.good()) {
        cout<<"Blad otwarcia pliku zrodlowego"<<endl;
        exit(EXIT_FAILURE);
    }

    // get pointer to associated buffer object
    pbuf=sourcestr.rdbuf();

    // get file size using buffer's members
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    // set seek position to 0
    pbuf->pubseekpos (0,ios::in);

    // allocate memory to contain file data
    buffer=new char[BUFSIZE];

    while(ile_wyslane < size){
        // set seek position to ile_wyslane
        pbuf->pubseekpos (ile_wyslane,ios::in);

        // get file data
        pbuf->sgetn (buffer,BUFSIZE);

        ile_wyslane += sendto(nClientSocket, buffer, BUFSIZE, 0, (struct sockaddr*) &stClient, sizeof stClient);
        //ile_wyslane += write(nClientSocket, buffer, BUFSIZE);

    }
    sourcestr.close();

    close(nClientSocket);
}*/

void setUpOutputSocket (int* nSocket, sockaddr_in stAddr){
    int nListen, nBind;
    int nFoo = 1;

    *nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (*nSocket < 0){
        perror ("Can't create a socket.");
        exit (EXIT_FAILURE);
    }
    setsockopt(*nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

    nBind = bind(*nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
    if (nBind < 0){
        perror ("Can't bind a name to a socket");
        exit (EXIT_FAILURE);
    }

    nListen = listen(*nSocket, QUEUE_SIZE);
    if (nListen < 0) {
        perror ("Can't set queue size.");
        exit(EXIT_FAILURE);
    }
}

void setUpInputSocket (int* nSocket, sockaddr_in stAddr){
    int nListen, nBind;
    int nFoo = 1;

    *nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (*nSocket < 0){
        perror ("Can't create an input socket.");
        exit (EXIT_FAILURE);
    }
    setsockopt(*nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

    nBind = bind(*nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
    if (nBind < 0){
        perror ("Can't bind a name to an input socket");
        exit (EXIT_FAILURE);
    }

    nListen = listen(*nSocket, QUEUE_SIZE);
    if (nListen < 0) {
        perror ("Can't set queue size for input socket.");
        exit(EXIT_FAILURE);
    }
}

void listenCommands (int* listenSocket){
    int socket = *listenSocket;

    struct sockaddr_in stClientAdr;
    int * nClientSocket = new int;
    socklen_t nTmp = sizeof(struct sockaddr);
    long ile_odebrane;
    char * buffer;
    char * name_of_file = nullptr;

    while(1){
        *nClientSocket = accept(socket, (struct sockaddr*)&stClientAdr, &nTmp);
        if (nClientSocket < 0) {
            perror ("Can't create a connection's socket. Error in listen commands");
            exit(EXIT_FAILURE);
        }

        char command;
        ile_odebrane = read(*nClientSocket, &command, sizeof(char));
        if (ile_odebrane == 0){
            cout << "Client disconnected before sending command" << endl;
            close(*nClientSocket);
            continue;
        }
        if (ile_odebrane == -1){
            perror ("Can't read command.");
            exit(EXIT_FAILURE);
        }
        cout << "Otrzymalem komende " << command << endl;

        /*******************************/
        /******* read new file *********/

        if ( command == '1'){
            int sizeOfFile;
            ile_odebrane = read(*nClientSocket, &sizeOfFile, sizeof(int));
            if (ile_odebrane == 0){
                cout << "Client disconnected before sending size of file" << endl;
                close(*nClientSocket);
                continue;
            }
            if (ile_odebrane == -1){
                perror ("Can't read size of file.");
                exit(EXIT_FAILURE);
            }

            sizeOfFile = ntohl(sizeOfFile);
            ile_odebrane = 0;
            buffer = new char[BUFSIZE];

            /*********************************/
            /****** read name of file ********/

            name_of_file = new char[TITLE_SIZE];

            int odebrane = read(*nClientSocket, name_of_file, TITLE_SIZE);
            if (odebrane == 0){
                cout << "Client disconnected, end of downloading audio" << endl;
                close(*nClientSocket);
                break;
            }
            if (odebrane == -1){
                perror ("Can't read file.");
                exit(EXIT_FAILURE);
            }

            /*********************************/
            /******* read audio file *********/

            ofstream desstr;
            filebuf *pbuf;

            desstr.open("./audio/Wynik.wav", ios::out | ios::binary);
            // get pointer to associated buffer object
            pbuf=desstr.rdbuf();
            // set seek position to 0
            pbuf->pubseekpos (0,ios::in);

            while (ile_odebrane < sizeOfFile){

                int odebrane = read(*nClientSocket, buffer, BUFSIZE);
                if (odebrane == 0){
                    cout << "Client disconnected, end of downloading audio" << endl;
                    close(*nClientSocket);
                    break;
                }
                if (odebrane == -1){
                    perror ("Can't read file.");
                    exit(EXIT_FAILURE);
                }
                ile_odebrane += odebrane;

                // get file data
                pbuf->sputn (buffer,odebrane);
            }

            desstr.close();
            cout << "Plik dodany do kolejki" << endl;
        }

        /*****************************/
        /******* send titles *********/

        //TO DO wyslac ilosc tytulow i tyle razy tytul po 100 bajtow
        if (command == '2'){
            int ile = write(*nClientSocket, name_of_file, TITLE_SIZE);

            cout << "Wyslalem " << ile << endl;
        }

        close(*nClientSocket);
    }

    close(*nClientSocket);
}

void exitProgram (int sig){
    cout << endl << "Ctrl+C, zamykam serwer" << endl;
    exit(0);
}


int main() {
    int nSocket, inputSocket;
    socklen_t nTmp;
    struct sockaddr_in stAddr, stAddrInput, stClientAddr;


    /******************************************************************/
    /************* socket for streaming music to client ***************/

    memset(&stAddr, 0, sizeof(struct sockaddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stAddr.sin_port = htons(SERVER_PORT);

    setUpOutputSocket(&nSocket, stAddr);


    /******************************************************************/
    /*********** socket for listening commands from client ************/

    memset(&stAddrInput, 0, sizeof(struct sockaddr));
    stAddrInput.sin_family = AF_INET;
    stAddrInput.sin_addr.s_addr = htonl(INADDR_ANY);
    stAddrInput.sin_port = htons(SERVER_PORT_INPUT);

    setUpInputSocket(&inputSocket, stAddrInput);

    thread listen_socket (listenCommands, &inputSocket );

    // Catch ctrl+C
    signal(SIGINT, exitProgram);


    while(true){
        auto * nClientSocket = new int;
        nTmp = sizeof(struct sockaddr);
        *nClientSocket = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
        if (nClientSocket < 0) {
            perror ("Can't create a connection's socket.");
            exit(EXIT_FAILURE);
        }

        thread second (Sumuj,nClientSocket);
        second.detach();
    }

    /*while(1){
        nTmp = sizeof(struct sockaddr);

        int sample_message;
        int return_val = recvfrom(nSocket, &sample_message, sizeof(int), 0, (struct sockaddr*)&stClientAddr, &nTmp);
        if (return_val < 0) {
            perror ("Can't create a connection's socket.");
            exit(EXIT_FAILURE);
        }

        thread second (Sumuj,&nSocket, &stClientAddr);
        second.detach();
    }*/

    return 0;
}