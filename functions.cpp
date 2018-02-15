#include "functions.h"




/**************** TCP ****************/
void streamuj(int* socket) {
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

    while(ile_wyslane < size/5){
        // set seek position to ile_wyslane
        pbuf->pubseekpos (ile_wyslane,ios::in);

        // get file data
        pbuf->sgetn (buffer,BUFSIZE);

        ile_wyslane += write(nClientSocket, buffer, BUFSIZE);

    }
    sourcestr.close();

    ile_wyslane = 0;

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
    sourcestr.close();

    close(nClientSocket);
}

// ##################### UDP #####################
/*void streamuj(int* socket, sockaddr_in* stClientAddr) {
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

void exitProgram (int sig){
    cout << endl << "Ctrl+C, zamykam serwer" << endl;
    exit(0);
}