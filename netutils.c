//win-specific impl
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "netutils.h"

#define PORT 9864

#define QUEUE 32

int serverFD, newSocket, readStat;
struct sockaddr_in addrin;
int opt = 1;

int InitServer()
{
    // mswin specific setup (untested)
#ifdef _WIN32
    WSADATA wsa;

    int wsa_status = WSAStartup(((2 << 8) + 2), &wsa);

    if (wsa_status) 
    {
        printf("WSA failure: %d.\n", wsa_status);
        return 2;
    }

    char major = wsa.wVersion & 255;
    char minor = (wsa.wVersion >> 8) & 255;

    if ((major) != 2 || (minor) != 2)
    {
        WSACleanup();
        printf("Expected Winsock Ver 2.2, got Ver %d.%d\n", );
        return 3;
    }
#endif

    // Creating socket file descriptor
    serverFD = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFD < 0) {
        printf("socketing failed: %s\n", serverFD);
        exit(1);
    }

    int sockOpt = setsockopt(serverFD, SOL_SOCKET,
                SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt)); 
    if (sockOpt){
        printf("socket optioning failed: %d\n", sockOpt);
        exit(2);
    }

    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = INADDR_ANY;
    addrin.sin_port = htons(PORT);

    struct sockaddr adress = *((struct sockaddr*)&addrin);

    int bindStat = bind(serverFD, &adress, sizeof(addrin));
    if (bindStat < 0) {
        printf("bind failed: %d\n", bindStat);
        exit(3);
    }

    int listenStat = listen(serverFD, 15);
    if (listenStat < 0) {
        printf("listener failed: %d\n", listenStat);
        exit(4);
    }

    printf("server init'd\n");
    return 0;
}

int ProcServer()
{
    printf("listening\n");
    int addrlen = sizeof(addrin);
    unsigned char buffer[1024] = { 0 };

    newSocket = accept(serverFD, (struct sockaddr*)&addrin, (socklen_t*)&addrlen);

    /*  Connection process is as follows
    *   Client greets server with custom header, and a requestorID token assigned to client via login (TODO)
    *   Server checks the token as valid (TODO), and twiddles the requestorID before returning to the client
    *   Client then verifies the returned RequestorID as being valid (not 0), and twiddles before sending with request
    */

    if (newSocket < 0) {
        printf("incoming transmission failure: %s\n" ,newSocket);
        return 1;
    }

    readStat = read(newSocket, buffer, 1024);
    if (readStat < 1) 
    {
        printf("initial read failure\n");
        return 2;
    }
    printf("guest says: %s %d\n", buffer, readStat);
    if (memcmp(buffer, "Hello!", readStat))
    {
        printf("guest was rude, said: \'");
        for (int i = 0; i < readStat; i++)
        printf("%c", buffer[i]);
         
        printf("\'. terminating connection...\n");
        send(newSocket, "Shoo!", 5, 0);
        return 0;
    }
    printf("responding: \"Hello!\"\n");
    send(newSocket, buffer, 6, 0);
    
    return 0;
}

int KillServer()
{
    // closing the connected socket
    close(newSocket);
    // closing the listening socket
    shutdown(serverFD, SHUT_RDWR);

    printf("server de-init'd\n");
    return 0;
}
