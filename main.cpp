#include "functions.cpp"

void listenCommands (int* listenSocket, list<TitleOfAudio*>* listOfTitles, int* last_number){
    int socket = *listenSocket;

    struct sockaddr_in stClientAdr;
    int * nClientSocket = new int;
    socklen_t nTmp = sizeof(struct sockaddr);
    long ile_odebrane;
    char * buffer;
    char * name_of_file = new char[TITLE_SIZE];

    while(true){
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
            uint sizeOfFile;
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

            ssize_t odebrane = read(*nClientSocket, name_of_file, TITLE_SIZE);
            cout<<odebrane<<endl;
            if (odebrane == 0){
                cout << "Client disconnected, cant download title" << endl;
                close(*nClientSocket);
                break;
            }
            if (odebrane == -1){
                perror ("Can't read title.");
                exit(EXIT_FAILURE);
            }

            listOfTitles->push_back(new TitleOfAudio(name_of_file));

            /*********************************/
            /******* read audio file *********/

            ofstream desstr;
            filebuf *pbuf;

            desstr.open("./audio/" + to_string((*last_number) + 1) + ".wav", ios::out | ios::binary);
            // get pointer to associated buffer object
            pbuf=desstr.rdbuf();
            // set seek position to 0
            pbuf->pubseekpos (0,ios::in);

            while (ile_odebrane < sizeOfFile){

                odebrane = read(*nClientSocket, buffer, BUFSIZE);
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
            (*last_number)++;
            cout << "Plik dodany do kolejki" << endl;
        }

        /*****************************/
        /******* send titles *********/

        if (command == '2'){
            uint size = htonl(listOfTitles->size());
            ssize_t wyslane = write(*nClientSocket, &size, sizeof(int));
            if (wyslane == 0){
                cout << "Client disconnected, I'm stopping sending number of titles" << endl;
                close(*nClientSocket);
                break;
            }
            if (wyslane == -1){
                perror ("Can't write number of titles.");
                exit(EXIT_FAILURE);
            }

            cout << "Wysylam " << ntohl(size) << " tytulow do klienta" << endl;

            for (list<TitleOfAudio*>::const_iterator iterator = listOfTitles->begin(), end = listOfTitles->end(); iterator != end; ++iterator) {
                wyslane = write(*nClientSocket, (*iterator)->getTitle(), TITLE_SIZE);
                cout << "Wyslany tytul: " << (*iterator)->getTitle() << endl;
                if (wyslane == 0){
                    cout << "Client disconnected, I'm stopping sending titles" << endl;
                    close(*nClientSocket);
                    break;
                }
                if (wyslane == -1){
                    perror ("Can't write title.");
                    exit(EXIT_FAILURE);
                }
            }

            cout << "Wyslalem " << ntohl(size) << " tytulow do klienta" << endl;

        }

        close(*nClientSocket);
    }

    delete name_of_file, buffer;
    close(*nClientSocket);
}

int main() {
    int nSocket, inputSocket;
    socklen_t nTmp;
    struct sockaddr_in stAddr, stAddrInput, stClientAddr;
    list<TitleOfAudio*> listOfTitles;

    int actual_number = 1, last_number = 0;


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

    thread listen_socket (listenCommands, &inputSocket, &listOfTitles, &last_number );
    listen_socket.detach();

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

        thread second (streamuj, nClientSocket, &actual_number, &last_number);
        second.detach();
    }

    return 0;
}