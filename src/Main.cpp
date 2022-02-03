#define WIN
#ifdef WIN // Windows compilation path

#define Kilobytes(Value) ((Value)*1024)
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024) 

#if DEBUG
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>

#else // linux compilation path

// TODO: Add Linux Compilation Path

#endif

#include <stdio.h>

#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN Kilobytes(8)

int GetRequestParser(char *buffer)
{
    printf(buffer, "\n");
    return 0;
};

int main()
{
    WSADATA wsaData;

    int iResult;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ListenSocket == INVALID_SOCKET)
    {
        printf("Generating Socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if(iResult == SOCKET_ERROR)
    {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET ClientSocket = INVALID_SOCKET;

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;

    }

    char recvbuf[DEFAULT_BUFLEN];
    int sendResult;
    int recvbuflen = DEFAULT_BUFLEN;

    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);
            GetRequestParser(recvbuf);

            sendResult = send(ClientSocket, recvbuf, recvbuflen, 0);
            if(sendResult == SOCKET_ERROR)
            {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", sendResult);
        } else if(iResult == 0)
        {
            printf("Connection closing...\n");
        } else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    WSACleanup();

    return(0);
}