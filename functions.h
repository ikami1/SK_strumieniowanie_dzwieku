#ifndef SK_STRUMIENIOWANIE_DZWIEKU_FUNCTIONS_H
#define SK_STRUMIENIOWANIE_DZWIEKU_FUNCTIONS_H

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
#include <list>

#define SERVER_PORT 41111
#define SERVER_PORT_INPUT 41112
#define QUEUE_SIZE 5
#define BUFSIZE 1024
#define TITLE_SIZE 100

using namespace std;

class TitleOfAudio{

    char title[TITLE_SIZE];

public:
    char* getTitle(){ return title;}

    TitleOfAudio( char* title ) {
        strncpy ( title, this->title, TITLE_SIZE );
    }

};

/*** TCP ***/
void streamuj(int* socket);
/*** UDP ***/
//void streamuj(int* socket, sockaddr_in* stClientAddr);

void setUpOutputSocket (int* nSocket, sockaddr_in stAddr);
void setUpInputSocket (int* nSocket, sockaddr_in stAddr);
void exitProgram (int sig);

#endif //SK_STRUMIENIOWANIE_DZWIEKU_FUNCTIONS_H
