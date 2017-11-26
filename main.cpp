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
#define QUEUE_SIZE 5
//do usuniecia w pozniejszej wersji
#define BUFSIZE 1024


using namespace std;


void Sumuj(int* socket) {
    int nClientSocket = *socket;
    //std::cout<<"[connection from " << inet_ntoa((struct in_addr)stClientAddr.sin_addr)<<std::endl;
    std::cout<<"Nowe polaczenie"<<std::endl;


    filebuf *pbuf;
    ifstream sourcestr;
    ofstream desstr;
    long size;
    char * buffer;
    long ile_razy_wyslane = 0;

    sourcestr.open("../Brooklynska rada zydow.mp3", ios::in | ios::binary);
    if (!sourcestr.good()) {
        cout<<"Blad otwarcia pliku zrodlowego"<<endl;
        exit(EXIT_FAILURE);
    }
    desstr.open("../Wynik.mp3", ios::out | ios::binary);

    // get pointer to associated buffer object
    pbuf=sourcestr.rdbuf();

    // get file size using buffer's members
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    // set seek position to 0
    pbuf->pubseekpos (0,ios::in);

    // allocate memory to contain file data
    buffer=new char[BUFSIZE];

    while(ile_razy_wyslane*BUFSIZE < size){
        // get file data
        pbuf->sgetn (buffer,BUFSIZE);
        ile_razy_wyslane++;
        write(nClientSocket, buffer, sizeof(buffer));
    }

    sourcestr.close();

    close(nClientSocket);
}


int main() {
    int nSocket;
    int nBind, nListen;
    int nFoo = 1;
    socklen_t nTmp;
    struct sockaddr_in stAddr, stClientAddr;
    //int question[BUFSIZE];
    int nClientSocket;


    memset(&stAddr, 0, sizeof(struct sockaddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stAddr.sin_port = htons(SERVER_PORT);

    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nSocket < 0){
        perror ("Can't create a socket.");
        exit (EXIT_FAILURE);
    }
    setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

    nBind = bind(nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
    if (nBind < 0){
        perror ("Can't bind a name to a socket");
        exit (EXIT_FAILURE);
    }

    nListen = listen(nSocket, QUEUE_SIZE);
    if (nListen < 0) {
        perror ("Can't set queue size.");
        exit(EXIT_FAILURE);
    }

    while(1){
        int* nClientSocket = new int;
        nTmp = sizeof(struct sockaddr);
        *nClientSocket = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
        if (nClientSocket < 0) {
            perror ("Can't create a connection's socket.");
            exit(EXIT_FAILURE);
        }

        thread second (Sumuj,nClientSocket);
        second.detach();
    }

    //TO DO: przechwytywanie sygnalu ctrl+c i zamykanie socketa
    close(nSocket);

    return 0;
}