Hinty do klienta:
-

#include <fstream>

ofstream desstr;

desstr.open("../Pioseneczka.mp3", ios::out | ios::binary); //czy tam jakas inna sciezka

//jakies przetwarzanie

// write content to Wynik.mp3, where buffer is "char *buffer;"
desstr.write(buffer,size);

desstr.close();
